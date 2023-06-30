package fit.nsu.labs.model.exceptions;

public class ConfigException extends ManufactoryException {
    public ConfigException(Class<?> className) {
        super("Config exception. Can`t Find keys for component: " + className.getName());
    }
}
