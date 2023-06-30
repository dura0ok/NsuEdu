package ru.nsu.neretin.lab1503.models;

public class Ticker extends Thread {

    private final SumNumberGuesser game;

    public Ticker(SumNumberGuesser game) {
        this.game = game;
    }

    @Override
    public void run() {
        while (!interrupted()) {
            try {
                Thread.sleep(5000);
                game.handleLate();
            } catch (InterruptedException ignored) {
                interrupt();
            }
        }
    }


}
