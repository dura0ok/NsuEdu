package fit.nsu.labs.lab4.client.model;

import fit.nsu.labs.lab4.message.Command;

public record OutgoingQueueItem(Command command, Runnable after) {
}
