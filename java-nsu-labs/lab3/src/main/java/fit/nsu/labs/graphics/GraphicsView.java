package fit.nsu.labs.graphics;

import fit.nsu.labs.Main;
import fit.nsu.labs.model.CarManufacturer;
import fit.nsu.labs.model.CarProduct;
import fit.nsu.labs.model.Event;
import fit.nsu.labs.model.OnEvent;
import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.component.ComponentInfo;
import fit.nsu.labs.model.exceptions.ConfigException;
import fit.nsu.labs.model.exceptions.ManufactoryException;
import io.github.cdimascio.dotenv.Dotenv;
import io.github.cdimascio.dotenv.DotenvException;

import javax.swing.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

public class GraphicsView extends JFrame implements OnEvent {
    private final StatisticPanel bodyStat = new StatisticPanel("body stat");
    private final StatisticPanel engineStat = new StatisticPanel("engine stat");
    private final StatisticPanel accessoryStat = new StatisticPanel("accessory stat");
    private final StatisticPanel carStat = new StatisticPanel("car stat");

    private final Map<Class<? extends ComponentInfo>, StatisticPanel> eventHandlerPlace = new HashMap<>();

    public GraphicsView(CarManufacturer carManuFacturer) throws ConfigException {
        initHandlers();
        carManuFacturer.registerObserver(this);
        JPanel bodySpeedSlider = new SliderPanel("body rate", carManuFacturer, CarBody.class);
        JPanel engineSpeedSlider = new SliderPanel("engine rate", carManuFacturer, CarEngine.class);
        JPanel accesorySpeedSlider = new SliderPanel("accessory rate", carManuFacturer, CarAccessory.class);

        JPanel bodyWorkersSlider = new SliderPanel("body workers", carManuFacturer, CarBody.class);
        JPanel engineWorkersSlider = new SliderPanel("engine workers", carManuFacturer, CarEngine.class);
        JPanel accessoryWorkersSlider = new SliderPanel("accessory workers", carManuFacturer, CarAccessory.class);

        JPanel carSpeedSlider = new SliderPanel("car rate", carManuFacturer, CarProduct.class);
        JPanel carWorkersSlider = new SliderPanel("car workers", carManuFacturer, CarProduct.class);
        // Set up the JFrame
        setTitle("Factory");

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setSize(500, 500);
        setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));

        add(bodySpeedSlider);
        add(engineSpeedSlider);
        add(accesorySpeedSlider);


        add(bodyWorkersSlider);
        add(engineWorkersSlider);
        add(accessoryWorkersSlider);

        add(bodyStat);
        add(engineStat);
        add(accessoryStat);

        add(carSpeedSlider);
        add(carWorkersSlider);
        add(carStat);

        setVisible(true);
        carManuFacturer.start();
    }

    public static void main(String[] args) {
        try {
            Dotenv.configure().systemProperties().load();
        } catch (DotenvException ignored) {
            Dotenv.configure().directory(Objects.requireNonNull(Main.class.getResource("/")).getPath()).systemProperties().load();
        }

        SwingUtilities.invokeLater(() -> {
            try {
                new GraphicsView(new CarManufacturer());
            } catch (ManufactoryException e) {
                throw new RuntimeException(e);
            }
        });
    }

    private void initHandlers() {
        eventHandlerPlace.put(CarBody.class, bodyStat);
        eventHandlerPlace.put(CarEngine.class, engineStat);
        eventHandlerPlace.put(CarAccessory.class, accessoryStat);
        eventHandlerPlace.put(CarProduct.class, carStat);
    }

    @Override
    public void notification(Event event) {
        var handler = eventHandlerPlace.get(event.type());
        handler.setStorageSize(event.storageSize());
        handler.updateProducedNumber(event.totalProduced());
    }
}
