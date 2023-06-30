package ru.nsu.neretin.lab1503;

public record Event(EventType type, String data) {
    public Event(EventType type) {
        this(type, null);
    }
}
