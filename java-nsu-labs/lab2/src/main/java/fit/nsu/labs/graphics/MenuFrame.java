package fit.nsu.labs.graphics;

import fit.nsu.labs.model.GameField;
import fit.nsu.labs.model.GameLevels;
import fit.nsu.labs.model.RecordsManager;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.util.ArrayList;
import java.util.List;

public class MenuFrame extends JFrame {


    private String playerName = null;

    MenuFrame() {
        JLabel name = new JLabel("Minesweeper game");
        name.setFont(new Font("Arial", Font.BOLD, 30));

        setLayout(new GridBagLayout());


        GridBagConstraints container = new GridBagConstraints();
        container.gridwidth = GridBagConstraints.REMAINDER;
        container.anchor = GridBagConstraints.NORTH;

        add(name, container);

        container.anchor = GridBagConstraints.CENTER;
        container.fill = GridBagConstraints.HORIZONTAL;

        JPanel buttonsPanel = new JPanel(new GridBagLayout());
        buttonsPanel.setBorder(new EmptyBorder(10, 10, 10, 10));
        container.weightx = 1;

        List<JButton> buttonsContainer = new ArrayList<>();
        var easyButton = new JButton("Easy Level");
        easyButton.setActionCommand(String.valueOf(GameLevels.EASY));
        easyButton.addActionListener(new MenuHandler(this));
        buttonsContainer.add(easyButton);

        var mediumButton = new JButton("Medium Level");
        mediumButton.setActionCommand(String.valueOf(GameLevels.MEDIUM));
        mediumButton.addActionListener(new MenuHandler(this));
        buttonsContainer.add(mediumButton);

        var hardButton = new JButton("Hard Level");
        hardButton.setActionCommand(String.valueOf(GameLevels.HARD));
        hardButton.addActionListener(new MenuHandler(this));
        buttonsContainer.add(hardButton);

        var customButton = new JButton("Custom Game");
        customButton.setActionCommand(String.valueOf(GameLevels.CUSTOM));
        customButton.addActionListener(new MenuHandler(this));
        buttonsContainer.add(customButton);

        var scoresButton = new JButton("Show scores");
        scoresButton.addActionListener(e -> {
            var frame = new JFrame("High Scores");
            String[] options = {GameLevels.EASY.name(), GameLevels.MEDIUM.name(), GameLevels.HARD.name(), GameLevels.CUSTOM.name()};
            String selectedOption = (String) JOptionPane.showInputDialog(null, "Choose level:", "Options",
                    JOptionPane.PLAIN_MESSAGE, null, options, options[0]);

            var recordsHandler = new RecordsManager();
            var easy = recordsHandler.readRecords(GameLevels.valueOf(selectedOption));
            var table = new JTable(new RecordsTable(easy, GameLevels.valueOf(selectedOption)));
            frame.add(new JScrollPane(table));
            frame.pack();
            frame.setVisible(true);
            frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        });
        buttonsContainer.add(scoresButton);

        var helpButton = new JButton("Help");
        helpButton.addActionListener(e -> JOptionPane.showMessageDialog(null, GameField.getHelp()));
        buttonsContainer.add(helpButton);

        for (var button : buttonsContainer) {
            buttonsPanel.add(button, container);
            container.insets = new Insets(10, 0, 0, 0);
        }


        add(buttonsPanel, container);

        setVisible(true);
        setSize(500, 500);
        setLocationRelativeTo(null);

        while (playerName == null) {
            playerName = JOptionPane.showInputDialog(null, "Enter name");
        }

        if (playerName.isEmpty()) {
            JOptionPane.showMessageDialog(null, "You entered empty name. This is incorrect :C", "Error", JOptionPane.ERROR_MESSAGE);
            dispose();
        }

        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
    }

    public String getPlayerName() {
        return playerName;
    }
}


