package fit.nsu.labs.graphics;

import javax.swing.*;
import java.awt.*;

class InfoPanel extends JPanel {
    private final JLabel timeElapsedTextField = new JLabel();
    private final JLabel flagsLeftTextField = new JLabel();

    InfoPanel(int flagsLeft) {
        setLayout(new GridLayout(1, 2));

        timeElapsedTextField.setFont(new Font("Verdana", Font.BOLD, 13));
        add(timeElapsedTextField);

        setFlagsLeftTextField(flagsLeft);
        flagsLeftTextField.setFont(new Font("Verdana", Font.BOLD, 13));
        add(flagsLeftTextField);
    }

    public void setTimeElapsedTextField(long time) {
        var minutes = time / 60;
        timeElapsedTextField.setText(String.format("Time: %d minutes %d seconds", minutes, time % 60));
    }


    void setFlagsLeftTextField(int flagsLeft) {
        flagsLeftTextField.setText("Flags left: " + flagsLeft);
    }
}