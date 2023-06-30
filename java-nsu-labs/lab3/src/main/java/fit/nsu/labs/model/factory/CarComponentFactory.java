package fit.nsu.labs.model.factory;

import fit.nsu.labs.model.component.CarComponent;
import fit.nsu.labs.model.config.ConfigKeysManager;
import fit.nsu.labs.model.exceptions.ConfigException;
import fit.nsu.labs.model.storage.RamStorage;

import java.lang.reflect.InvocationTargetException;

public class CarComponentFactory<T extends CarComponent> {
    private final Class<T> componentClass;
    private final RamStorage<T> storage;
    private int totalProduced = 0;

    public CarComponentFactory(Class<T> componentClass) throws ConfigException {
        this.componentClass = componentClass;
        var config = ConfigKeysManager.getComponentKeys(this.componentClass);
        this.storage = new RamStorage<>(Integer.parseInt(System.getProperty(config.get("capacity"))));
    }

    public int getTotalProduced() {
        return totalProduced;
    }

    public RamStorage<T> getStorage() {
        return storage;
    }

    public int getStorageSize() {
        return storage.getSize();
    }

    public Class<? extends CarComponent> getProductClass() {
        return componentClass;
    }

    private T createComponent() {
        try {
            return componentClass.getConstructor().newInstance();
        } catch (InvocationTargetException | NoSuchMethodException | InstantiationException |
                 IllegalAccessException e) {
            //System.out.println(componentClass.getName());
            throw new RuntimeException(e);
        }
    }

    public void produceElement() throws InterruptedException {
        var el = createComponent();
        storage.put(el);
        //System.out.println("[Produce component] " + el);
        totalProduced++;
    }
}