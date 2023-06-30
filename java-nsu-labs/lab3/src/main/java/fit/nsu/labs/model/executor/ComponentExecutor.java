package fit.nsu.labs.model.executor;

import fit.nsu.labs.model.CarManufacturer;
import fit.nsu.labs.model.component.CarComponent;
import fit.nsu.labs.model.config.ConfigKeysManager;
import fit.nsu.labs.model.exceptions.ManufactoryException;
import fit.nsu.labs.model.factory.CarComponentFactory;
import fit.nsu.labs.model.tasks.ConsumeComponent;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.*;

public class ComponentExecutor<T extends CarComponent> {
    private final CarComponentFactory<T> factory;
    private final ThreadPoolExecutor executor;
    private final CarManufacturer model;
    private final List<ScheduledFuture<?>> future = new ArrayList<>();

    private int rate;
    private int workersCount;

    public ComponentExecutor(Class<T> componentClass, CarManufacturer model) throws ManufactoryException {
        var config = ConfigKeysManager.getComponentKeys(componentClass);
        this.model = model;
        ScheduledExecutorService executor = Executors.newScheduledThreadPool(getWorkersCount());
        this.executor = (ThreadPoolExecutor) executor;
        factory = new CarComponentFactory<>(componentClass);
        rate = Integer.parseInt(System.getProperty(config.get("rate")));
        setWorkersCount(Integer.parseInt(System.getProperty(config.get("workersCount"))));
    }

    public CarComponentFactory<T> getFactory() {
        return factory;
    }

    public int getWorkersCount() {
        return workersCount;
    }

    public void setWorkersCount(int workersCount) {
        //System.out.println("workers count " + workersCount);
        this.workersCount = workersCount;
        executor.setCorePoolSize(workersCount);
        reschedule(getRate());
    }

    public void start() {
        scheduleExecutor();
    }

    private void scheduleExecutor() {
        var scheduler = (ScheduledExecutorService) executor;
        for (int i = 0; i < getWorkersCount(); i++) {
            future.add(scheduler.scheduleAtFixedRate(() -> executor.submit(new ConsumeComponent<>(factory, model)), 0, getRate() * 50L, TimeUnit.MILLISECONDS));
        }
    }

    public void reschedule(int newRate) {
        setRate(newRate);
        for (var item : future) {
            item.cancel(false);
        }
        scheduleExecutor();
    }

    public int getRate() {
        return rate;
    }

    public void setRate(int rate) {
        this.rate = rate;
    }
}
