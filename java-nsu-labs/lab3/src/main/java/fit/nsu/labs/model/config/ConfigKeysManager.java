package fit.nsu.labs.model.config;

import fit.nsu.labs.model.CarProduct;
import fit.nsu.labs.model.component.CarAccessory;
import fit.nsu.labs.model.component.CarBody;
import fit.nsu.labs.model.component.CarEngine;
import fit.nsu.labs.model.component.ComponentInfo;
import fit.nsu.labs.model.exceptions.ConfigException;

import java.util.HashMap;
import java.util.Map;

abstract class ComponentKeysProvider {
    public abstract Map<String, String> getKeys();
    public static final String RATE_KEY = "rate";
    public static final String CAPACITY_KEY = "capacity";
    public static final String WORKERS_COUNT_KEY = "workersCount";
}

class CarBodyKeysProvider extends ComponentKeysProvider {


    @Override
    public Map<String, String> getKeys() {
        Map<String, String> keys = new HashMap<>();
        keys.put(RATE_KEY, "BODY_SPEED_RATE");
        keys.put(CAPACITY_KEY, "STORAGE_BODY_CAPACITY");
        keys.put(WORKERS_COUNT_KEY, "WORKERS_BODY_COUNT");
        return keys;
    }
}

class CarEngineKeysProvider extends ComponentKeysProvider {
    @Override
    public Map<String, String> getKeys() {
        Map<String, String> keys = new HashMap<>();
        keys.put(RATE_KEY, "ENGINE_SPEED_RATE");
        keys.put(CAPACITY_KEY, "STORAGE_ENGINE_CAPACITY");
        keys.put(WORKERS_COUNT_KEY, "WORKERS_ENGINE_COUNT");
        return keys;
    }
}

class CarAccessoryKeysProvider extends ComponentKeysProvider {
    @Override
    public Map<String, String> getKeys() {
        Map<String, String> keys = new HashMap<>();
        keys.put(RATE_KEY, "ACCESSORY_SPEED_RATE");
        keys.put(CAPACITY_KEY, "STORAGE_ACCESSORY_CAPACITY");
        keys.put(WORKERS_COUNT_KEY, "WORKERS_ACCESSORY_COUNT");
        return keys;
    }
}

class CarProductKeysProvider extends ComponentKeysProvider {
    @Override
    public Map<String, String> getKeys() {
        Map<String, String> keys = new HashMap<>();
        keys.put(RATE_KEY, "CAR_BUILD_SPEED_RATE");
        keys.put(CAPACITY_KEY, "STORAGE_CARS_CAPACITY");
        keys.put(WORKERS_COUNT_KEY, "WORKERS_BUILD_CAR_COUNT");
        return keys;
    }
}

public class ConfigKeysManager {
    private static final Map<Class<? extends ComponentInfo>, ComponentKeysProvider> componentKeysMap = new HashMap<>();

    static {
        componentKeysMap.put(CarBody.class, new CarBodyKeysProvider());
        componentKeysMap.put(CarEngine.class, new CarEngineKeysProvider());
        componentKeysMap.put(CarAccessory.class, new CarAccessoryKeysProvider());
        componentKeysMap.put(CarProduct.class, new CarProductKeysProvider());
    }

    public static Map<String, String> getComponentKeys(Class<? extends ComponentInfo> componentClass) throws ConfigException {
        ComponentKeysProvider keysProvider = componentKeysMap.get(componentClass);
        if (keysProvider == null) {
            throw new ConfigException(componentClass);
        }
        return keysProvider.getKeys();
    }
}

