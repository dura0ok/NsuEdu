package fit.nsu.labs.graphics;

import javax.swing.*;

public class StatisticPanel extends JPanel {
    private final String componentName;
    private final JLabel producedNumber = new JLabel();
    private final JLabel storageSize = new JLabel();

    public StatisticPanel(String componentName) {
        this.componentName = componentName;

        updateProducedNumber(0);
        setStorageSize(0);
        add(producedNumber);
        add(storageSize);
    }

    public void updateProducedNumber(int newValue) {
        producedNumber.setText(componentName + ": " + newValue);
    }

    public void setStorageSize(int size) {
        storageSize.setText("Storage " + componentName + " size: " + size);
    }
}
