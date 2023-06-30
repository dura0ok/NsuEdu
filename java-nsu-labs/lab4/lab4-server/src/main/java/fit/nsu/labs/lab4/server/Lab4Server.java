package fit.nsu.labs.lab4.server;

import fit.nsu.labs.lab4.serializer.Deserializer;
import fit.nsu.labs.lab4.serializer.Serializer;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Consumer;

public class Lab4Server {
    private final AtomicInteger idGenerator = new AtomicInteger(1);
    private final Set<ClientSocket> clients = new HashSet<>();
    private final Map<Class<?>, Handler> handlers = new HashMap<>();

    private final Serializer serializer;
    private final Deserializer deserializer;

    private Consumer<ClientSocket> consumerNewConnection;

    public Lab4Server(Serializer serializer, Deserializer deserializer) {
        this.serializer = serializer;
        this.deserializer = deserializer;
    }

    public void registerHandler(Class<?> clazz, Handler handler) {
        handlers.put(Objects.requireNonNull(clazz), Objects.requireNonNull(handler));
    }

    public void open(int port) {
        try (var serverSocket = new ServerSocket(port)) {
            while (!Thread.interrupted()) {
                var socket = serverSocket.accept();
                var clientSocket = new ClientSocket(socket, serializer, deserializer, handlers, idGenerator.getAndIncrement());
                clientSocket.onClose(() -> clients.remove(clientSocket));
                clients.add(clientSocket);

                if (consumerNewConnection != null) {
                    consumerNewConnection.accept(clientSocket);
                }
            }
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        } finally {
            System.out.println("SERVER CLOSED");
        }
    }

    public List<ClientSocket> getClients() {
        return List.copyOf(clients);
    }

    public void onNewConnection(Consumer<ClientSocket> consumer) {
        this.consumerNewConnection = consumer;
    }
}
