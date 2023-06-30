package model;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Printer implements Runnable {
    private final int n;
    private static volatile int k = 0;
    private final ReentrantLock lock;
    private final Condition condition;

    public Printer(int n, ReentrantLock lock, Condition condition) {
        this.n = n;
        this.lock = lock;
        this.condition = condition;
    }

    @Override
    public void run() {
        try {
            Thread.sleep(getRandomNumber(1, 2));
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

        lock.lock();
        while (n != k) {
            try {
                condition.await();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

        System.out.println(n + 1);
        k++;
        condition.signalAll();
        lock.unlock();
    }

    private int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }
}
