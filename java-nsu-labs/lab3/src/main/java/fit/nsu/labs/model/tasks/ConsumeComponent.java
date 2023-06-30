package fit.nsu.labs.model.tasks;

import fit.nsu.labs.model.CarManufacturer;
import fit.nsu.labs.model.Event;
import fit.nsu.labs.model.component.CarComponent;
import fit.nsu.labs.model.factory.CarComponentFactory;

public class ConsumeComponent<T extends CarComponent> implements Runnable {
    private final CarComponentFactory<T> factory;
    private final CarManufacturer model;

    public ConsumeComponent(CarComponentFactory<T> factory, CarManufacturer model) {
        this.factory = factory;
        this.model = model;
    }

    @Override
    public void run() {
        try {
            //System.out.printf("%s Capacity %d Size %d\n", factory.getProductClass().getName(), factory.getStorage().getStorageCapacity(), factory.getStorage().getSize());
            factory.produceElement();
            model.notifyObservers(new Event(factory.getProductClass(), factory.getTotalProduced(), factory.getStorageSize()));

        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }
}
