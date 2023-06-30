package ui;

import model.Consumer;
import model.Producer;
import model.Storage;

import java.util.Scanner;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        var scanner = new Scanner(System.in);
        System.out.print("Enter n: ");
        var n = scanner.nextInt();
        System.out.print("Enter p: ");
        var p = scanner.nextInt();
        System.out.print("Enter c: ");
        var c = scanner.nextInt();

        System.out.print("Enter timeout: ");
        var timeout = scanner.nextInt();

        var storage = new Storage(c);

        try (var producers = Executors.newFixedThreadPool(p); var consumers = Executors.newFixedThreadPool(c)) {

            producers.submit((Runnable)new Producer(storage, p));
            consumers.submit((Runnable) new Consumer(storage));
            Thread.sleep(timeout * 1000L);


            producers.shutdownNow();
            consumers.shutdownNow();

        }



    }
}