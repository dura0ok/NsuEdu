package fit.nsu.labs.model.tasks;

import fit.nsu.labs.model.CarManufacturer;
import fit.nsu.labs.model.CarProduct;
import fit.nsu.labs.model.Event;
import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.factory.CarComponentFactory;
import fit.nsu.labs.model.storage.RamStorage;

public class BuildCar implements Runnable {
    private final CarComponentFactory<CarBody> carBodyStorage;
    private final CarComponentFactory<CarEngine> carEngineStorage;
    private final CarComponentFactory<CarAccessory> carAccessoryStorage;
    private final RamStorage<CarProduct> carStorage;
    private final CarManufacturer model;

    public BuildCar(CarComponentFactory<CarBody> carBodyStorage, CarComponentFactory<CarEngine> carEngineStorage,
                    CarComponentFactory<CarAccessory> carAccessoryStorage, RamStorage<CarProduct> carStorage, CarManufacturer model) {
        this.carBodyStorage = carBodyStorage;
        this.carEngineStorage = carEngineStorage;
        this.carAccessoryStorage = carAccessoryStorage;
        this.carStorage = carStorage;
        this.model = model;
    }

    @Override
    public void run() {
        try {

            var body = carBodyStorage.getStorage().get();
            var engine = carEngineStorage.getStorage().get();
            var accessory = carAccessoryStorage.getStorage().get();
            var product = new CarProduct(body, engine, accessory);
            Thread.sleep(Integer.parseInt(System.getProperty("CAR_BUILD_SPEED_RATE")) * 1000L);

            carStorage.put(product);
            //System.out.println("[Build Car] " + product);
            model.notifyObservers(new Event(CarBody.class, carBodyStorage.getTotalProduced(), carBodyStorage.getStorageSize()));
            model.notifyObservers(new Event(CarEngine.class, carEngineStorage.getTotalProduced(), carEngineStorage.getStorageSize()));
            model.notifyObservers(new Event(CarAccessory.class, carAccessoryStorage.getTotalProduced(), carAccessoryStorage.getStorageSize()));
            model.notifyObservers(new Event(CarProduct.class, CarProduct.getID(), carStorage.getSize()));

        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

    }
}
