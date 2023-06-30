package ru.nsu.neretin.lab0802.task7.UI;

import ru.nsu.neretin.lab0802.task7.Model.ArrayModifierFactory;

import java.util.Scanner;

public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.print("Введите размер массива: ");
        int n = scanner.nextInt();
        System.out.print("Введите какой индекс вы хотите удалить: ");
        int k = scanner.nextInt();
        var modifyMethod = ArrayModifierFactory.createArrayModifier();
        var generatedArray = generateArray(n);
        var newArr = modifyMethod.modify(k, generatedArray);
        for(var el: newArr){
            System.out.println(el);
        }
    }

    private static int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }

    public static int [] generateArray(int n){
        int[] res = new int[n];
        for(int i = 0; i < n; i++){
            res[i] = getRandomNumber(-100, 100);
        }
        return res;
    }


}