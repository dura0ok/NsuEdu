package fit.nsu.labs.model.component;

import java.util.concurrent.atomic.AtomicInteger;

public class CarAccessory extends CarComponent {
    private static final AtomicInteger id = new AtomicInteger(0);

    public CarAccessory() {
        id.incrementAndGet();
    }

    @Override
    public String toString() {
        return "Car Accessory: " + id.get();
    }

}
