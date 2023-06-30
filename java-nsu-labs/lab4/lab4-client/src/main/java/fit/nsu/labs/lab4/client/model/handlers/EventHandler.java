package fit.nsu.labs.lab4.client.model.handlers;

import fit.nsu.labs.lab4.client.Event;
import fit.nsu.labs.lab4.client.model.Handler;
import fit.nsu.labs.lab4.client.model.Lab4Client;
import fit.nsu.labs.lab4.message.EventMessage;

import java.util.Collections;

import static fit.nsu.labs.lab4.client.Event.EventType.UPDATE_MESSAGES_AREA;

public class EventHandler implements Handler {

    public EventHandler() {
    }

    @Override
    public Event handle(Object object, Lab4Client lab4Client) {
        if (!(object instanceof EventMessage event)) {
            System.out.println("[WARN] Object is not an instance of EventMessage");
            return null;
        }

        String message = "";

        switch (event.getEventName()) {
            case "message" -> {
                message = String.format("%s: %s", event.getName(), event.getMessage());
                System.out.printf("<chat> %s%n", message);
            }
            case "userlogin" -> message = String.format("<system> %s join server", event.getName());
            case "userlogout" -> {
                message = String.format("<system> %s leave server", event.getName());
                System.out.println(message);
            }
            default -> {
                System.out.printf("[WARN] Unknown event '%s'%n", event.getEventName());
                return null;
            }
        }

        return new Event(UPDATE_MESSAGES_AREA, Collections.singletonList(message));
    }

}
