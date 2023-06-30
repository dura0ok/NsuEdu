package fit.nsu.labs.console;

import fit.nsu.labs.exceptions.MineSweeperGameException;
import fit.nsu.labs.model.*;

import java.util.InputMismatchException;

public class ConsoleController {

    private final ConsoleViewer view;
    private GameField model;
    private String playerName = null;

    public ConsoleController(ConsoleViewer view) {
        this.view = view;
        try {
            playerName = view.inputLine("Enter name: ");
            if (playerName.isEmpty()) {
                throw new MineSweeperGameException("invalid name");
            }

            view.printMenu();

            int menuChoice = view.inputNumber("Enter menu choice: ");

            switch (menuChoice) {
                case 1 -> {
                    var easyLevel = GameSettings.getEasyLevel();
                    model = new GameField(easyLevel, playerName);
                }
                case 2 -> {
                    var mediumLevel = GameSettings.getMediumLevel();
                    model = new GameField(mediumLevel, playerName);
                }
                case 3 -> {
                    var hardLevel = GameSettings.getHardLevel();
                    model = new GameField(hardLevel, playerName);
                }
                case 4 -> {

                    var columns = view.inputNumber("Enter cols: ");

                    var rows = view.inputNumber("Enter rows: ");

                    var bombsCounter = view.inputNumber("Enter bombs counter: ");

                    var flagsCounter = view.inputNumber("Enter limit flags: ");
                    model = new GameField(new GameSettings(columns, rows, bombsCounter, flagsCounter, GameLevels.CUSTOM), playerName);

                }
                case 5 -> {
                    var levelChoice = view.inputNumber("1 - Easy, 2 - Medium, 3 - Hard, 4 - Custom: ");
                    GameLevels level = null;
                    switch (levelChoice) {
                        case 1 -> level = GameLevels.EASY;
                        case 2 -> level = GameLevels.MEDIUM;
                        case 3 -> level = GameLevels.HARD;
                        case 4 -> level = GameLevels.CUSTOM;
                        case 5 -> System.exit(0);
                        default -> System.out.println("Something went wrong");
                    }
                    var manager = new RecordsManager();
                    var data = manager.readRecords(level);
                    for (var el : data) {
                        System.out.print(el);
                    }
                }
                case 6 -> view.printHelp();
                case 7 -> System.exit(0);
                default -> throw new IllegalArgumentException("Unexpected value: " + menuChoice);
            }

            if (model != null) {
                model.registerObserver(view);
            }

        } catch (MineSweeperGameException | IllegalArgumentException e) {
            System.err.println(e.getMessage());
        }
    }


    public void startGame() {
        if (model == null) {
            return;
        }


        this.model.startGame();
        while (model.getState() != GameField.GameState.GAME_OVER) {
            try {
                var type = view.inputNumber("Enter type command(dot - 0, flag - 1, time - 2, 3 - print field,  exit - 4): ");

                if (type == 2) {
                    System.out.println("Elapsed time: " + model.getElapsed());
                    continue;
                }

                if (type == 3) {
                    model.notifyObservers(new Event(EventType.REDRAW_REQUEST, model));
                    continue;
                }

                if (type == 4) {
                    System.exit(0);
                }


                if (type == 0 || type == 1) {

                    var x = view.inputNumber("Enter x: ");
                    var y = view.inputNumber("Enter y: ");


                    if (type == 0) {
                        model.click(new Dot(y, x));
                    }

                    if (type == 1) {
                        model.updateFlag(new Dot(y, x));
                    }

                    continue;
                }


                throw new IllegalArgumentException("invalid command type");

            } catch (IllegalArgumentException e) {
                System.out.println(e.getMessage());
            }

        }

        try {
            int res = view.inputNumber("Do you want to restart?: 1 - Yes, other input - No: ");
            if (res == 1) {
                model = new GameField(model.getSettings(), playerName);
                model.registerObserver(view);
                startGame();
            }
        } catch (InputMismatchException | IllegalArgumentException ignored) {
            System.exit(0);
        }

    }


}
