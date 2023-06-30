package fit.nsu.labs.lab4.client.model;

import fit.nsu.labs.lab4.client.Event;
import fit.nsu.labs.lab4.client.OnEvent;
import fit.nsu.labs.lab4.exception.DeserializeException;
import fit.nsu.labs.lab4.exception.SerializeException;
import fit.nsu.labs.lab4.message.Command;
import fit.nsu.labs.lab4.serializer.Deserializer;
import fit.nsu.labs.lab4.serializer.Serializer;

import java.io.EOFException;
import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.util.*;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class Lab4Client implements AutoCloseable, Observable {
    private final BlockingQueue<OutgoingQueueItem> outgoingQueue = new LinkedBlockingQueue<>();
    private final Map<Class<?>, Handler> mapHandlers = new HashMap<>();

    private final Serializer serializer;
    private final Deserializer deserializer;
    private final List<OnEvent> onEvents = new ArrayList<>();

    private boolean connected = false;

    private Socket socket;
    private Thread inThread;
    private Thread outThread;

    public Lab4Client(Serializer serializer, Deserializer deserializer) {
        this.serializer = serializer;
        this.deserializer = deserializer;
    }

    public void registerHandler(Class<?> clazz, Handler handler) {
        mapHandlers.put(Objects.requireNonNull(clazz), Objects.requireNonNull(handler));
    }

    public void connect(String host, int port) throws IOException {
        socket = new Socket(host, port);

        inThread = new Thread(this::incoming, "Incoming handled");
        outThread = new Thread(this::outgoing, "Outgoing handled");

        inThread.start();
        outThread.start();

        connected = true;

    }

    public void login(String username) {
        if (!connected) throw new RuntimeException("Client not connected!");

        outgoingQueue.add(new OutgoingQueueItem(
                Command.builder()
                        .command("login")
                        .name(username)
                        .type("Lab4-Recode")
                        .build(),
                null));
    }

    public void logout(String sessionId, Runnable after) {
        if (!connected) throw new RuntimeException("Client not connected!");

        outgoingQueue.add(new OutgoingQueueItem(
                Command.builder()
                        .command("logout")
                        .session(sessionId)
                        .build(),
                after));
    }

    public void sendMessage(String sessionId, String message) {
        if (!connected) throw new RuntimeException("Client not connected!");

        outgoingQueue.add(new OutgoingQueueItem(
                Command.builder()
                        .command("message")
                        .session(sessionId)
                        .message(message)
                        .build(),
                null));
    }

    public void requestMembersList(String sessionId) {
        if (!connected) throw new RuntimeException("Client not connected!");

        outgoingQueue.add(new OutgoingQueueItem(
                Command.builder()
                        .command("list")
                        .session(Objects.requireNonNull(sessionId))
                        .build(),
                null));
    }

    @Override
    public void close() throws Exception {
        outThread.interrupt();
        inThread.interrupt();
        socket.close();

        connected = false;
    }

    private void incoming() {
        int recurringException = 0;

        while (!Thread.interrupted()) {
            try {
                var inputStream = socket.getInputStream();
                var object = deserializer.deserialize(inputStream);

                var callback = mapHandlers.get(object.getClass());
                if (callback != null) {
                    System.out.println(callback);
                    var event = callback.handle(object, this);
                    notifyObservers(event);
                } else {
                    System.out.printf("[WARN] No handler for '%s'\n", object.getClass());
                }

            } catch (EOFException ignored) {
                System.err.println("EOF error");
                return;
            } catch (IOException | DeserializeException e) {
                //noinspection DuplicatedCode
                if (e.getCause() instanceof SocketException se) {
                    if (se.getMessage().equalsIgnoreCase("Socket closed")) {
                        // Соединение закрыли? Ну тогда и мы закрываемся
                        try {
                            close();
                        } catch (Exception ignore) {
                            //ignore
                        }
                        break;
                    }
                }
                e.printStackTrace();
            }
        }
    }

    private void outgoing() {
        while (!Thread.interrupted()) {
            try {
                var queueItem = outgoingQueue.take();
                var outputStream = socket.getOutputStream();

                var bytes = serializer.serialize(queueItem.command());
                outputStream.write(bytes);
                outputStream.flush();

                if (queueItem.after() != null) {
                    new Thread(queueItem.after()).start();
                }

            } catch (InterruptedException ignore) {
                break;
            } catch (IOException | SerializeException e) {
                //noinspection DuplicatedCode
                if (e.getCause() instanceof SocketException se) {
                    if (se.getMessage().equalsIgnoreCase("Socket closed")) {
                        // Соединение закрыли? Ну тогда и мы закрываемся
                        try {
                            close();
                        } catch (Exception ignore) {
                            //ignore
                        }
                        break;
                    }
                }
                e.printStackTrace();
            }
        }
    }

    @Override
    public void registerObserver(OnEvent o) {
        onEvents.add(o);
    }

    public void notifyObservers(Event event) {
        if (event == null) {
            return;
        }

        for (var onEvent : onEvents) {
            onEvent.notification(event);
        }
    }

}
