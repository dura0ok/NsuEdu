package fit.nsu.labs.model.component;

import java.util.concurrent.atomic.AtomicInteger;

public class CarEngine extends CarComponent {
    private static final AtomicInteger id = new AtomicInteger(0);

    public CarEngine() {
        id.incrementAndGet();
    }


    @Override
    public String toString() {
        return "Car Engine: " + id.get();
    }
}
