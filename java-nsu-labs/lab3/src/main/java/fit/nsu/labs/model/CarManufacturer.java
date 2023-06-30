package fit.nsu.labs.model;

import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.exceptions.ManufactoryException;
import fit.nsu.labs.model.executor.CarBuildExecutor;
import fit.nsu.labs.model.executor.ComponentExecutor;
import fit.nsu.labs.model.tasks.SellCar;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class CarManufacturer implements Observable {
    private final CarBuildExecutor carBuildExecutor;
    private final ComponentExecutor<CarBody> bodyExecutor;
    private final ComponentExecutor<CarEngine> engineExecutor;
    private final ComponentExecutor<CarAccessory> accessoryExecutor;
    private final ExecutorService carStorageControllerThread;
    private final ExecutorService dealersExecutor;
    private final List<OnEvent> onEvents = new ArrayList<>();

    public CarManufacturer() throws ManufactoryException {

        bodyExecutor = new ComponentExecutor<>(CarBody.class, this);
        engineExecutor = new ComponentExecutor<>(CarEngine.class, this);
        accessoryExecutor = new ComponentExecutor<>(CarAccessory.class, this);

        carBuildExecutor = new CarBuildExecutor.Builder()
                .withCarBodyFactory(bodyExecutor.getFactory())
                .withCarEngineFactory(engineExecutor.getFactory())
                .withCarAccessoryFactory(accessoryExecutor.getFactory())
                .withModel(this)
                .build();


        carStorageControllerThread = Executors.newSingleThreadScheduledExecutor();
        carBuildExecutor.start(carStorageControllerThread);

        var dealersCount = getDealersCount();
        dealersExecutor = Executors.newScheduledThreadPool(dealersCount);
    }

    private static int getDealersCount() {
        return Integer.parseInt(System.getProperty("WORKERS_SELL_CAR_COUNT"));
    }

    private static int getSellCarSpeedRate() {
        return Integer.parseInt(System.getProperty("SELL_CAR_SPEED_RATE"));
    }

    public ComponentExecutor<CarBody> getBodyExecutor() {
        return bodyExecutor;
    }

    public ComponentExecutor<CarEngine> getEngineExecutor() {
        return engineExecutor;
    }

    public ComponentExecutor<CarAccessory> getAccessoryExecutor() {
        return accessoryExecutor;
    }

    public void start() {
        bodyExecutor.start();
        engineExecutor.start();
        accessoryExecutor.start();
        carBuildExecutor.start(carStorageControllerThread);
        var dealersScheduler = (ScheduledExecutorService) dealersExecutor;
        var dealersRate = getSellCarSpeedRate();
        dealersScheduler.scheduleAtFixedRate(new SellCar(carBuildExecutor.getCarStorage(), this), 0, dealersRate * 500L, TimeUnit.MILLISECONDS);
    }

    @Override
    public void registerObserver(OnEvent o) {
        onEvents.add(o);
    }

    public void notifyObservers(Event event) {
        for (var onEvent : onEvents) {
            onEvent.notification(event);
        }
    }
}
