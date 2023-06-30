package fit.nsu.labs.model.storage;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BlockingQueue;

public class RamStorage<T> implements IStorage<T> {
    private final BlockingQueue<T> components;
    private final int capacity;

    public RamStorage(int capacity) {
        this.capacity = capacity;
        this.components = new ArrayBlockingQueue<>(this.capacity);
    }

    @Override
    public void put(T newItem) throws InterruptedException {
        components.put(newItem);
    }

    @Override
    public T get() throws InterruptedException {
        return components.take();
    }

    @Override
    public int getStorageCapacity() {
        return this.capacity;
    }

    @Override
    public int getRemainingCapacity() {
        return components.remainingCapacity();
    }

    @Override
    public int getSize() {
        return components.size();
    }


}
