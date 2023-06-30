package fit.nsu.labs.lab4.server;

@FunctionalInterface
public interface Handler {

    void handle(Object object, ClientSocket clientSocket);
}
