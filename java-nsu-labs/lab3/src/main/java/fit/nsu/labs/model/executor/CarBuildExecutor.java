package fit.nsu.labs.model.executor;

import fit.nsu.labs.model.CarManufacturer;
import fit.nsu.labs.model.CarProduct;
import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.factory.CarComponentFactory;
import fit.nsu.labs.model.storage.RamStorage;
import fit.nsu.labs.model.tasks.BuildCar;

import java.util.concurrent.*;

public class CarBuildExecutor implements Runnable {
    private final CarComponentFactory<CarBody> carBodyFactory;
    private final CarComponentFactory<CarEngine> carEngineFactory;
    private final CarComponentFactory<CarAccessory> carAccessoryFactory;
    private final RamStorage<CarProduct> carStorage;
    private final ThreadPoolExecutor executor;
    private final CarManufacturer model;

    private CarBuildExecutor(Builder builder) {
        //System.out.println(builder.workersCount);
        int workersCount = builder.workersCount;
        this.carBodyFactory = builder.carBodyFactory;
        this.carEngineFactory = builder.carEngineFactory;
        this.carAccessoryFactory = builder.carAccessoryFactory;
        this.carStorage = builder.carStorage;
        this.model = builder.model;

        ScheduledExecutorService executor = Executors.newScheduledThreadPool(workersCount);
        this.executor = (ThreadPoolExecutor) executor;
    }

    public void run() {
        var remain = carStorage.getRemainingCapacity();
        for (int i = 0; i < remain; i++) {
            executor.execute(new BuildCar(carBodyFactory, carEngineFactory, carAccessoryFactory, this.carStorage, model));

        }
    }

    public RamStorage<CarProduct> getCarStorage() {
        return carStorage;
    }

    public void start(ExecutorService carStorageControllerThread) {
        var scheduler = (ScheduledExecutorService) carStorageControllerThread;
        scheduler.scheduleAtFixedRate(this, 0, 500, TimeUnit.MILLISECONDS);
    }

    public static class Builder {
        private final int workersCount = Integer.parseInt(System.getProperty("WORKERS_BUILD_CAR_COUNT"));
        private final RamStorage<CarProduct> carStorage = new RamStorage<>(Integer.parseInt(System.getProperty("STORAGE_CARS_CAPACITY")));
        public CarManufacturer model;
        private CarComponentFactory<CarBody> carBodyFactory;
        private CarComponentFactory<CarEngine> carEngineFactory;
        private CarComponentFactory<CarAccessory> carAccessoryFactory;

        public Builder withCarBodyFactory(CarComponentFactory<CarBody> carBodyFactory) {
            this.carBodyFactory = carBodyFactory;
            return this;
        }

        public Builder withCarEngineFactory(CarComponentFactory<CarEngine> carEngineFactory) {
            this.carEngineFactory = carEngineFactory;
            return this;
        }

        public Builder withCarAccessoryFactory(CarComponentFactory<CarAccessory> carAccessoryFactory) {
            this.carAccessoryFactory = carAccessoryFactory;
            return this;
        }

        public Builder withModel(CarManufacturer model) {
            this.model = model;
            return this;
        }

        public CarBuildExecutor build() {

            return new CarBuildExecutor(this);
        }
    }
}