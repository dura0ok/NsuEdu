package fit.nsu.labs.model;

import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.component.ComponentInfo;

import java.util.concurrent.atomic.AtomicInteger;

public class CarProduct extends ComponentInfo {
    private static final AtomicInteger id = new AtomicInteger(0);

    private final CarBody body;
    private final CarEngine engine;
    private final CarAccessory accessory;

    public CarProduct(CarBody body, CarEngine engine, CarAccessory accessory) {
        this.body = body;
        this.engine = engine;
        this.accessory = accessory;
        id.incrementAndGet();
    }

    public static int getID() {
        return id.get();
    }

    @Override
    public String toString() {
        return "CarProduct{" +
                "id=" + id.get() +
                ", body=" + body +
                ", engine=" + engine +
                ", accessory=" + accessory +
                '}';
    }
}
