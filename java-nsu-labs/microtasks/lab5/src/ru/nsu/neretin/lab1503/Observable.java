package ru.nsu.neretin.lab1503;

// наблюдатель
public interface Observable {
    void registerObserver(Observer o);

    void notifyObservers(Event event);
}
