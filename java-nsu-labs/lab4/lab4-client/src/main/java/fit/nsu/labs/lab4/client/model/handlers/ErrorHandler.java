package fit.nsu.labs.lab4.client.model.handlers;

import fit.nsu.labs.lab4.client.Event;
import fit.nsu.labs.lab4.client.model.Handler;
import fit.nsu.labs.lab4.client.model.Lab4Client;
import fit.nsu.labs.lab4.message.Error;

import java.util.Collections;

import static fit.nsu.labs.lab4.client.Event.EventType.UPDATE_MESSAGES_AREA;

public class ErrorHandler implements Handler {

    public ErrorHandler() {
    }

    @Override
    public Event handle(Object object, Lab4Client lab4Client) {
        var error = (Error) object;
        var message = "<system> ERROR: %s".formatted(error.getMessage());
        System.out.println(message);
        return new Event(UPDATE_MESSAGES_AREA, Collections.singletonList(message));
    }
}
