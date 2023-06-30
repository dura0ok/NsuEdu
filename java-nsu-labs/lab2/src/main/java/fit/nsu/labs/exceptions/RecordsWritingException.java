package fit.nsu.labs.exceptions;

public class RecordsWritingException extends MineSweeperGameException {
    public RecordsWritingException(String message) {
        super(message);
    }

    public RecordsWritingException(String message, Throwable cause) {
        super(message, cause);
    }
}
