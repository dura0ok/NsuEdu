package fit.nsu.labs.model.storage;

public interface IStorage<T> {
    int getStorageCapacity();

    int getRemainingCapacity();

    int getSize();

    void put(T newItem) throws InterruptedException;

    T get() throws InterruptedException;
}
