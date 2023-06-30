package ru.nsu.neretin.lab1003.task4.model;

import java.util.ArrayDeque;

public class Storage <T>{
    private final int capacity;
    private final ArrayDeque<T> products = new ArrayDeque<>();
    public Storage(int maxCapacity) {
        capacity = maxCapacity;
    }

    synchronized void putProduct(T product) throws InterruptedException {
        while(products.size() >= capacity){
            wait();
        }
        products.push(product);
        notifyAll();
    }

    synchronized T getProduct() throws InterruptedException {
        while(products.isEmpty()){
            wait();
        }

        var exited =  products.pop();
        notifyAll();
        return exited;
    }
}
