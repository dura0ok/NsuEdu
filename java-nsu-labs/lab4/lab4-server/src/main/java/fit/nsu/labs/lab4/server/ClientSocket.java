package fit.nsu.labs.lab4.server;

import fit.nsu.labs.lab4.exception.DeserializeException;
import fit.nsu.labs.lab4.exception.SerializeException;
import fit.nsu.labs.lab4.serializer.Deserializer;
import fit.nsu.labs.lab4.serializer.Serializer;

import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class ClientSocket implements AutoCloseable {
    private final BlockingQueue<OutgoingQueueItem> outgoingQueue = new LinkedBlockingQueue<>();
    private final Map<String, String> attributes = new HashMap<>();

    private final Socket socket;
    private final Serializer serializer;
    private final Deserializer deserializer;
    private final Map<Class<?>, Handler> handlers;
    private final Thread inThread;
    private final Thread outThread;

    private boolean connected = true;

    private Runnable closeTask;

    public ClientSocket(Socket socket, Serializer serializer, Deserializer deserializer, Map<Class<?>, Handler> handlers, int id) {
        this.socket = socket;
        this.serializer = serializer;
        this.deserializer = deserializer;
        this.handlers = handlers;

        this.inThread = new Thread(this::incoming, "(#%d) Input handler".formatted(id));
        this.outThread = new Thread(this::outgoing, "(#%d) Output handler".formatted(id));

        this.inThread.start();
        this.outThread.start();
    }

    public void send(Object object) {
        send(object, null);
    }

    public void send(Object object, Runnable after) {
        outgoingQueue.add(new OutgoingQueueItem(Objects.requireNonNull(object), after));
    }

    public void setAttribute(String name, String value) {
        attributes.put(name.toLowerCase(), value);
    }

    public String getAttribute(String name) {
        return attributes.get(name.toLowerCase());
    }

    public void onClose(Runnable runnable) {
        closeTask = runnable;
    }

    @Override
    public void close() throws Exception {
        outThread.interrupt();
        inThread.interrupt();
        socket.close();

        connected = false;
        if (closeTask != null) {
            closeTask.run();
        }
    }

    private void incoming() {
        while (!Thread.interrupted() && connected) {
            try {
                var inputStream = socket.getInputStream();
                var object = deserializer.deserialize(inputStream);

                var handler = handlers.get(object.getClass());
                if (handler != null) {
                    handler.handle(object, this);
                } else {
                    System.out.printf("[WARN] No handler for '%s'\n", object.getClass());
                }

            } catch (IOException | DeserializeException e) {
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

            }
        }
    }

    private void outgoing() {
        while (!Thread.interrupted() && connected) {
            try {
                var queueItem = outgoingQueue.take();
                var outputStream = socket.getOutputStream();
                var bytes = serializer.serialize(queueItem.object());

                outputStream.write(bytes);
                outputStream.flush();

                if (queueItem.after() != null) {
                    new Thread(queueItem.after()).start();
                }
            } catch (InterruptedException ignore) {
                break;
            } catch (IOException | SerializeException e) {
                if (e.getCause() instanceof SocketException se) {
                    if (se.getMessage().equalsIgnoreCase("Socket closed")) {
                        try {
                            close();
                        } catch (Exception ignore) {
                            //ignore
                        }
                        break;
                    }
                }

            }
        }
    }
}
