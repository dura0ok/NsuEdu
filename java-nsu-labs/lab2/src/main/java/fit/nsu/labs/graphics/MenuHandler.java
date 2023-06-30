package fit.nsu.labs.graphics;

import fit.nsu.labs.model.GameField;
import fit.nsu.labs.model.GameLevels;
import fit.nsu.labs.model.GameSettings;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Objects;

class MenuHandler implements ActionListener {

    private final MenuFrame menu;
    private GraphicsViewer viewer;
    private GameField model = null;

    public MenuHandler(MenuFrame menuFrame) {
        menu = menuFrame;
    }

    @Override
    public void actionPerformed(ActionEvent e) {

        try {
            if (e.getActionCommand().equals(String.valueOf(GameLevels.EASY))) {
                var easyLevel = GameSettings.getEasyLevel();
                model = new GameField(easyLevel, menu.getPlayerName());
            }

            if (e.getActionCommand().equals(String.valueOf(GameLevels.MEDIUM))) {
                var mediumLevel = GameSettings.getMediumLevel();
                model = new GameField(mediumLevel, menu.getPlayerName());
            }

            if (e.getActionCommand().equals(String.valueOf(GameLevels.HARD))) {
                var hardLevel = GameSettings.getHardLevel();
                model = new GameField(hardLevel, menu.getPlayerName());
            }


            if (e.getActionCommand().equals(String.valueOf(GameLevels.CUSTOM))) {

                int columns = Integer.parseInt(JOptionPane.showInputDialog(null, "Enter cols"));
                int rows = Integer.parseInt(JOptionPane.showInputDialog(null, "Enter rows"));
                int bombsCounter = Integer.parseInt(JOptionPane.showInputDialog(null, "Enter bombs counter"));
                int flagsCounter = Integer.parseInt(JOptionPane.showInputDialog(null, "Enter limit flags"));

                model = new GameField(new GameSettings(columns, rows, bombsCounter, flagsCounter, GameLevels.CUSTOM), menu.getPlayerName());

            }


            viewer = new GraphicsViewer(model);

            model.registerObserver(viewer);
            model.startGame();
            viewer.setVisible(true);
            menu.setVisible(false);
            viewer.addWindowListener(new WindowAdapter() {
                @Override
                public void windowClosed(WindowEvent windowEvent) {
                    String[] options = {"Exit", "Restart"};
                    String selectedOption = (String) JOptionPane.showInputDialog(null, "What you want to do?", "Exit options", JOptionPane.PLAIN_MESSAGE, null, options, options[1]);

                    if (Objects.equals(selectedOption, options[0])) {
                        menu.setVisible(true);
                        viewer.dispose();
                    } else {
                        viewer.setVisible(false);
                        model = new GameField(model.getSettings(), menu.getPlayerName());
                        viewer = new GraphicsViewer(model);
                        model.registerObserver(viewer);
                        model.startGame();
                        viewer.setVisible(true);
                        menu.setVisible(false);

                        addWindowListenerToViewer();
                        addWindowListenerToMenu();
                    }
                }
            });
            menu.addWindowListener(new java.awt.event.WindowAdapter() {
                @Override
                public void windowClosed(java.awt.event.WindowEvent windowEvent) {
                    menu.dispose();
                }
            });
        } catch (IllegalArgumentException ex) {
            JOptionPane.showMessageDialog(null, ex.getMessage(), "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void addWindowListenerToViewer() {
        viewer.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent windowEvent) {
                String[] options = {"Exit", "Restart"};
                String selectedOption = (String) JOptionPane.showInputDialog(null, "What you want to do?", "Exit options", JOptionPane.PLAIN_MESSAGE, null, options, options[1]);

                if (Objects.equals(selectedOption, options[0])) {
                    menu.setVisible(true);
                    viewer.dispose();
                } else {
                    viewer.setVisible(false);
                    model = new GameField(model.getSettings(), menu.getPlayerName());
                    viewer = new GraphicsViewer(model);
                    model.registerObserver(viewer);
                    model.startGame();
                    viewer.setVisible(true);
                    addWindowListenerToViewer();
                }
            }
        });
    }


    private void addWindowListenerToMenu() {
        menu.addWindowListener(new java.awt.event.WindowAdapter() {
            @Override
            public void windowClosed(java.awt.event.WindowEvent windowEvent) {
                menu.dispose();
            }
        });
    }
}
