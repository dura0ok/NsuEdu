package ru.nsu.neretin.lab1502.task6.Model;

public class DotsFactory {
    public static Plain create(int n){
        return new Plain(n);
    }
}
