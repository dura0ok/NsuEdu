package ru.nsu.neretin.lab1003.task4.ui;

import ru.nsu.neretin.lab1003.task4.model.Consumer;
import ru.nsu.neretin.lab1003.task4.model.Producer;
import ru.nsu.neretin.lab1003.task4.model.Storage;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        var scanner = new Scanner(System.in);
        System.out.print("Enter n: ");
        var n = scanner.nextInt();
        System.out.print("Enter p: ");
        var p = scanner.nextInt();
        System.out.print("Enter c: ");
        var c = scanner.nextInt();

        var storage = new Storage<String>(n);

        for(int i = 0; i < p; i++){
            var producer = new Producer(storage, p);
            producer.start();
        }

        for(int i = 0; i < c; i++){
            var consumer = new Consumer(storage);
            consumer.start();

        }


    }
}