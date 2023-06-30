package ui;

import model.Printer;

import java.util.Scanner;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        var scanner = new Scanner(System.in);
        var n = scanner.nextInt();

        var lock = new ReentrantLock();
        var condition = lock.newCondition();


        try(var threads = Executors.newFixedThreadPool(n)){
            for(int i = 0; i < n; i++){
                threads.execute(new Printer(i, lock, condition));
            }
        }catch (Exception e){
            e.printStackTrace();
        }

    }
}
