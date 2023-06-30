package model;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.Callable;

public class Producer implements Runnable, Callable<String> {
    private final Storage<String> storage;
    private static int ID = 0;
    private int produced = 0;
    private final int productID;


    
    public Producer(Storage<String> storage, int productID){
        this.storage = storage;
        this.productID = productID;
        ID++;
    }
    @Override
    public void run() {
        while(!Thread.interrupted()) {
            try {
                storage.enqueue("p - " +  Thread.currentThread().getName() + " - " + productID);
            } catch (InterruptedException e) {
                break;
            }
            produced++;
        }
    }


    @Override
    public String call() throws Exception {
       return ID + " " + produced;
    }
}
