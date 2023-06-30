package ru.nsu.neretin.lab1003.task4.model;

public class Producer extends Thread{
    private final Storage<String> storage;
    private final int productID;

    public Producer(Storage<String> storage, int productID){
        this.storage = storage;
        this.productID = productID;
    }
    @Override
    public void run() {
        while(!Thread.interrupted()){
            try {
                storage.putProduct("p - " +  this.getName() + " - " + productID);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }

    }
}
