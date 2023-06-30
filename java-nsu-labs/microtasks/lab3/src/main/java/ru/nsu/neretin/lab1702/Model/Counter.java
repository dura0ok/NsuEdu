package ru.nsu.neretin.lab1702.Model;

import java.util.Map;

public class Counter {
    public static void printerSum(INIParser parser){
        int resultSum = 0;
        var variables = parser.getVariables();
        for(var entry: variables.entrySet()){
            try {
                int num  = Integer.parseInt(entry.getValue());
                resultSum += num;
            } catch (NumberFormatException ignored) {
            }
        }

        System.out.println(resultSum);
    }
}
