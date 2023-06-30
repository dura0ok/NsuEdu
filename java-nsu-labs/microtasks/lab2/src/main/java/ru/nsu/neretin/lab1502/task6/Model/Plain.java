package ru.nsu.neretin.lab1502.task6.Model;

import java.util.*;

public class Plain {
    private final Map<Dot, ArrayList<Integer>> dots;

    public Plain(int n) {
        dots = new TreeMap<>();
        Random rand = new Random();
        for(int i = 0; i < n; i++){
            double x = rand.nextDouble() * 1000;
            double y = rand.nextDouble() * 1000;
            ArrayList<Integer> data = generateData();
            dots.put(new Dot(x,y), data);
        }
    }

    private static ArrayList<Integer> generateData() {
        ArrayList<Integer> data = new ArrayList<>();
        var dataLength = getRandomNumber(0, 200);
        for(int j = 0; j < dataLength; j++){
            var generatedRandomElement = getRandomNumber(10, 20);
            data.add(generatedRandomElement);
        }
        return data;
    }

    public AbstractMap.SimpleEntry<Dot, ArrayList<Integer>> getKthDot(int k){
        if(k > dots.size()){
            throw new IndexOutOfBoundsException();
        }
        var values = new ArrayList<>(dots.keySet());
        var dot = values.get(k);
        var value = dots.get(dot);
        return new AbstractMap.SimpleEntry<>(dot, value);
    }

    private static int getRandomNumber(int min, int max) {
        return (int) ((Math.random() * (max - min)) + min);
    }
}
