package ru.nsu.neretin.lab1702.UI;

import ru.nsu.neretin.lab1702.Model.Counter;
import ru.nsu.neretin.lab1702.Model.INIParser;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;


public class Main {
    public static void main(String[] args) {

        try(var file = new BufferedReader(new FileReader("/home/durachok/workspace/java-nsu-labs/microtasks/lab3/test.ini"))) {
            var parser = new INIParser(file);
            Counter.printerSum(parser);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }
}