package ru.nsu.neretin.lab1503;

import ru.nsu.neretin.lab1503.models.SumNumberGuesser;
import ru.nsu.neretin.lab1503.views.View;

import java.util.InputMismatchException;
import java.util.Scanner;

public class GameController {
    private final Scanner scanner = new Scanner(System.in);
    private final SumNumberGuesser model = new SumNumberGuesser();

    private final View view = new View();

    void startGame() {
        while (true) {
            try {
                var input = scanner.nextInt();
                model.makeGuess(input);
            } catch (InputMismatchException ignored) {
                view.printErrorNonNumber();
                scanner.next();
            }
        }
    }
}
