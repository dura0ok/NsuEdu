package ru.nsu.neretin.lab1503.views;

import ru.nsu.neretin.lab1503.Event;
import ru.nsu.neretin.lab1503.EventType;
import ru.nsu.neretin.lab1503.Observer;

public class View implements Observer {
    @Override
    public void notification(Event event) {
        if (event.type().equals(EventType.CORRECT_ANSWER)) {
            printCorrectAnswer(Integer.parseInt(event.data()));
            return;
        }
        if (event.type().equals(EventType.WRONG_ANSWER)) {
            printWrongAnswer();
            return;
        }
        if (event.type().equals(EventType.NEW_QUESTION)) {
            System.out.println("What is the sum of the digits of this number -> " + event.data() + " ?");
        }

        if (event.type().equals(EventType.LATE)) {
            System.out.println("LATE");
        }
    }

    private void printCorrectAnswer(int points) {
        System.out.println("Answer is correct. Your correct points: " + points);
    }

    private void printWrongAnswer() {
        System.out.println("Answer is not correct; Try again!");
    }

    public void printErrorNonNumber() {
        System.err.println("Your input not a number!");
    }
}
