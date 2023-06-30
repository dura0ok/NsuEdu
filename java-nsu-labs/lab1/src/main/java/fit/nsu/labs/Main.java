package fit.nsu.labs;

import fit.nsu.labs.commands.CommandFactory;
import fit.nsu.labs.exceptions.BadNumberOfArguments;

import java.nio.file.Files;
import java.nio.file.Paths;


public class Main {
    public static void main(String[] args) {
        try {
            var factory = new CommandFactory();
            factory.printAvailableCommandsInfo();
            switch (args.length) {
                case 0 -> {
                    System.out.println("Enter your commands directly into the terminal.");
                    System.out.println("Next press ctrl-d on Linux or ctrl-z to make it count or use exit command");

                    CalcExecutor calc = new CalcExecutor(System.in);
                    calc.calculate();
                }

                case 1 -> {
                    System.out.println("I read your commands now from file and calculate");
                    try (var input = Files.newInputStream(Paths.get(args[0]))) {
                        CalcExecutor calc = new CalcExecutor(input);
                        calc.calculate();
                    }
                }
                default -> throw new BadNumberOfArguments(
                        "You can add only 1 file to read from or stdin can be used by default"
                );
            }
        } catch (Exception e) {
            System.err.println(e.getMessage());
        }
    }
}