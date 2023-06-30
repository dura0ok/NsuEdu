package fit.nsu.labs.lab4.client;

import java.util.List;

public record Event(EventType type, List<String> data) {
    public enum EventType {
        UPDATE_MESSAGES_AREA,
        UPDATE_MEMBERS_AREA
    }
}
