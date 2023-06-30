package fit.nsu.labs.model.exceptions;

public class ManufactoryException extends Exception {
    public ManufactoryException(String message) {
        super(message);
    }

    public ManufactoryException(String message, Throwable cause) {
        super(message, cause);
    }
}
