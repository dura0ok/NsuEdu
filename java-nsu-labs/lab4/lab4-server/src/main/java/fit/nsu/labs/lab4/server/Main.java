package fit.nsu.labs.lab4.server;

import fit.nsu.labs.lab4.message.Command;
import fit.nsu.labs.lab4.message.Error;
import fit.nsu.labs.lab4.message.EventMessage;
import fit.nsu.labs.lab4.message.Success;
import fit.nsu.labs.lab4.serializer.XmlDeserializer;
import fit.nsu.labs.lab4.serializer.XmlSerializer;

import java.util.HashSet;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.atomic.AtomicInteger;

public class Main {
    private static final int DEFAULT_PORT = 8888;
    private static final int COUNT_MESSAGES = 10;

    public static void main(String[] args) throws InterruptedException {
        int port = Optional.ofNullable(System.getenv("LAB4_SERVER_PORT"))
                .map(portStr -> {
                    try {
                        return Integer.parseInt(portStr);
                    } catch (NumberFormatException e) {
                        e.printStackTrace();
                        return DEFAULT_PORT;
                    }
                })
                .orElse(DEFAULT_PORT);

        var sessionGenerator = new AtomicInteger(0);
        Set<User> users = new HashSet<>();
        BlockingDeque<ChatMessage> lastMessages = new LinkedBlockingDeque<>(COUNT_MESSAGES);

        var lab4Server = new Lab4Server(
                new XmlSerializer(),
                new XmlDeserializer());

        lab4Server.registerHandler(Command.class, (object, clientSocket) -> {
            var command = (Command) object;

            final var session = clientSocket.getAttribute("session");
            switch (command.getCommandName().toLowerCase()) {
                case "login" -> {
                    if (users.stream().anyMatch(u -> u.name().equals(command.getName()))) {
                        clientSocket.send(new Error("Пользователь с именем '%s' уже существует".formatted(command.getName())));
                    } else {
                        users.add(new User(command.getName(), session));
                        clientSocket.send(Success.builder()
                                .session(session)
                                .build());

                        lastMessages.stream()
                                .map(msg -> EventMessage.builder()
                                        .event("message")
                                        .message(msg.message())
                                        .name(msg.name())
                                        .build())
                                .forEach(clientSocket::send);

                        var eventLogin = EventMessage.builder()
                                .event("userlogin")
                                .name(command.getName())
                                .build();
                        lab4Server.getClients().forEach(cs -> cs.send(eventLogin));
                    }
                }
                case "logout" -> {
                    if (!command.getSession().equals(session)) {
                        clientSocket.send(new Error("Указана чужая или не существующая сессия"));
                    } else {
                        Optional<User> optionalUser = users.stream()
                                .filter(u -> u.session().equals(session))
                                .findFirst();

                        if (optionalUser.isEmpty()) {
                            clientSocket.send(new Error("Указана не существующая сессия"));
                            return;
                        }

                        clientSocket.send(Success.builder().build(), () -> {
                            try {
                                clientSocket.close();
                            } catch (Exception ignore) {
                                //ignore
                            }
                        });

                        var eventLogout = EventMessage.builder()
                                .event("userlogout")
                                .name(optionalUser.get().name())
                                .build();
                        lab4Server.getClients().forEach(cs -> cs.send(eventLogout));
                    }
                }
                case "list" -> {
                    if (!command.getSession().strip().equals(session.strip())) {
                        clientSocket.send(new Error("Указана чужая или не существующая сессия"));
                    } else {
                        var success = Success.builder()
                                .listUsers(users.stream()
                                        .map(u -> new Success.User(u.name(), u.session()))
                                        .toList())
                                .build();
                        clientSocket.send(success);
                    }
                }
                case "message" -> {
                    if (!command.getSession().strip().equals(session.strip())) {
                        clientSocket.send(new Error("Указана чужая или не существующая сессия"));
                    } else {
                        if (lastMessages.size() == COUNT_MESSAGES) lastMessages.removeFirst();
                        var user = users.stream()
                                .filter(u -> u.session().equals(session))
                                .findFirst()
                                .orElseThrow(() -> new RuntimeException("КУДА ПОЛЬЗОВАТЕЛЯ ДЕЛИ??? | session: %s".formatted(session)));
                        var chatMessage = new ChatMessage(user.name(), command.getMessage());
                        try {
                            lastMessages.putLast(chatMessage);

                            var event = EventMessage.builder()
                                    .event("message")
                                    .message(chatMessage.message())
                                    .name(chatMessage.name())
                                    .build();
                            lab4Server.getClients().forEach(cs -> cs.send(event));
                        } catch (InterruptedException ignore) {
                            //ignore
                        }
                    }
                }
            }
        });

        lab4Server.onNewConnection(clientSocket ->
                clientSocket.setAttribute("session", String.valueOf(sessionGenerator.incrementAndGet())));

        System.out.printf("SERVER LISTEN %d port\n", port);
        var serverThread = new Thread(() -> lab4Server.open(port), "Lab4Server");
        serverThread.start();
        serverThread.join();
    }
}
