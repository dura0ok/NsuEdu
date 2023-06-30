package model;

import java.util.Queue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;

public class Consumer implements Runnable, Callable<String> {
    private final Storage storage;
    private static int ID = 0;
    private int consumed = 0;

    public Consumer(Storage storage) {
        this.storage = storage;
        ID++;
    }
    @Override
    public void run() {
        while(!Thread.interrupted()) {
            try {
                System.out.println("c - " + Thread.currentThread().getName() + " consumes - " + storage.dequeue());
                Thread.sleep(100);
            } catch (InterruptedException e) {
                break;
            }
            consumed++;
        }
    }

    @Override
    public String call() throws Exception {
        return ID + " " + consumed;
    }
}
