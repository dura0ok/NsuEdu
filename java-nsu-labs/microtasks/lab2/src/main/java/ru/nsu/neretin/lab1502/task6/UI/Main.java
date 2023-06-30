package ru.nsu.neretin.lab1502.task6.UI;

import ru.nsu.neretin.lab1502.task6.Model.DotsFactory;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Введите количество точек: ");
        var n = scanner.nextInt();
        System.out.print("Введите k: ");
        var k = scanner.nextInt();

        var plain = DotsFactory.create(n);
        var el = plain.getKthDot(k);
        System.out.println(el.getKey());

        for(int i = 0; i < k; i++){
            System.out.println(el.getValue().get(i));
        }

    }
}