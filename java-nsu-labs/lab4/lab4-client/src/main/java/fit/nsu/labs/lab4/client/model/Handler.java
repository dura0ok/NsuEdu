package fit.nsu.labs.lab4.client.model;

import fit.nsu.labs.lab4.client.Event;

@FunctionalInterface
public interface Handler {

    Event handle(Object object, Lab4Client lab4Client);
}
