package model;

import java.util.concurrent.ArrayBlockingQueue;

public class Storage<T> {
    private final ArrayBlockingQueue<T> queue;
    public static int processed = 0;

    public Storage(int c) {
        this.queue = new ArrayBlockingQueue<T>(c);
    }

    public void enqueue(T value) throws InterruptedException {
        queue.put(value);
    }

    public T dequeue() throws InterruptedException {
        return queue.take();
    }
}
