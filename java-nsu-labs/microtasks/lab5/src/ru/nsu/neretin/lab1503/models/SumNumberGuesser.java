package ru.nsu.neretin.lab1503.models;


import ru.nsu.neretin.lab1503.Event;
import ru.nsu.neretin.lab1503.EventType;
import ru.nsu.neretin.lab1503.Observable;
import ru.nsu.neretin.lab1503.Observer;
import ru.nsu.neretin.lab1503.views.View;

import java.util.ArrayList;
import java.util.List;

public class SumNumberGuesser implements Observable {
    private final List<Observer> observers = new ArrayList<>();


    private Ticker ticker = new Ticker(this);

    private int number;

    private int points = 0;

    public SumNumberGuesser() {
    }

    public void start(){
        updateNumber();
        ticker.start();
    }

    public void registerView(Observer view){
        registerObserver(view);
    }

    private static int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }

    public void makeGuess(int guess) {
        ticker.interrupt();
        ticker = new Ticker(this);
        ticker.start();

        if (sumDigitsNum(number) == guess) {
            points++;
            notifyObservers(new Event(EventType.CORRECT_ANSWER, String.valueOf(points)));
            updateNumber();
            return;
        }
        notifyObservers(new Event(EventType.WRONG_ANSWER));
        updateNumber();
    }

    private int sumDigitsNum(int num) {
        int sum = 0;
        while (num > 0) {
            sum = sum + (num % 10);
            num = num / 10;
        }
        return sum;
    }

    public void updateNumber() {
        number = getRandomNumber(0, 10000);
        notifyObservers(new Event(EventType.NEW_QUESTION, String.valueOf(number)));
    }

    public void handleLate() {
        notifyObservers(new Event(EventType.LATE));
        updateNumber();
    }

    @Override
    public void registerObserver(Observer o) {
        observers.add(o);
    }

    @Override
    public void notifyObservers(Event eventType) {

        for (Observer observer : observers) {
            observer.notification(eventType);
        }
    }
}
