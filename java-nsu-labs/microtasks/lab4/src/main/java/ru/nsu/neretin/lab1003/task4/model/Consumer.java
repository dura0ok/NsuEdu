package ru.nsu.neretin.lab1003.task4.model;

public class Consumer extends Thread{
    private final Storage<String> storage;

    public Consumer(Storage<String> storage) {
        this.storage = storage;
    }

    @Override
    public void run() {
        while(!Thread.interrupted()) {
            try {
                System.out.println("c - " + this.getName() + " consumes - " + storage.getProduct());
                Thread.sleep(100);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }
}
