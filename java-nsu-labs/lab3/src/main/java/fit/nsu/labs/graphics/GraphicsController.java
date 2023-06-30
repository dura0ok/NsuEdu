package fit.nsu.labs.graphics;

import fit.nsu.labs.model.CarManufacturer;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class GraphicsController implements ChangeListener {
    private static final String WORKERS = "workers";
    private static final String BODY = "body";
    private static final String ENGINE = "engine";
    private static final String ACCESSORY = "accessory";
    private final String name;
    private final JLabel label;
    private final CarManufacturer factory;

    public GraphicsController(CarManufacturer factory, JLabel label, String name) {
        this.name = name;
        this.label = label;
        this.factory = factory;
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        JSlider slider = (JSlider) e.getSource();
        //System.out.println(slider);
        setLabelText(slider);
        var components = name.split(" ");
        var componentName = components[0];
        var newValue = slider.getValue();
        if (components[1].equalsIgnoreCase(WORKERS)) {
            if (componentName.equalsIgnoreCase(BODY)) {
                factory.getBodyExecutor().setWorkersCount(newValue);
            }

            if (componentName.equalsIgnoreCase(ENGINE)) {
                factory.getEngineExecutor().setWorkersCount(newValue);
            }

            if (componentName.equalsIgnoreCase(ACCESSORY)) {
                factory.getAccessoryExecutor().setWorkersCount(newValue);
            }
            return;
        }
        if (componentName.equalsIgnoreCase(BODY)) {
            factory.getBodyExecutor().reschedule(newValue);
        }

        if (componentName.equalsIgnoreCase(ENGINE)) {
            factory.getEngineExecutor().reschedule(newValue);
        }

        if (componentName.equalsIgnoreCase(ACCESSORY)) {
            factory.getAccessoryExecutor().reschedule(newValue);
        }
    }

    private void setLabelText(JSlider slider) {
        label.setText(name + ": " + slider.getValue());
    }
}
