package fit.nsu.labs.lab4.client.model.handlers;

import fit.nsu.labs.lab4.client.Event;
import fit.nsu.labs.lab4.client.model.Handler;
import fit.nsu.labs.lab4.client.model.Lab4Client;
import fit.nsu.labs.lab4.message.Success;

import java.util.concurrent.atomic.AtomicReference;

import static fit.nsu.labs.lab4.client.Event.EventType.UPDATE_MEMBERS_AREA;

public class SuccessHandler implements Handler {
    private final AtomicReference<String> refSessionId;

    public SuccessHandler(AtomicReference<String> refSessionId) {
        this.refSessionId = refSessionId;
    }

    @Override
    public Event handle(Object object, Lab4Client lab4Client) {
        var success = (Success) object;

        if (success.getSession() != null) {
            refSessionId.set(success.getSession());
            return null;
        } else if (success.getListUsers() != null) {
            var users = success.getListUsers().stream()
                    .map(Success.User::getName)
                    .toList();
            return new Event(UPDATE_MEMBERS_AREA, users);
        }
        return null;
    }
}