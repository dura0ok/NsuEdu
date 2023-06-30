package fit.nsu.labs.lab4.server;

public record OutgoingQueueItem(Object object, Runnable after) {
}
