package fit.nsu.labs.exceptions;

public class MineSweeperGameException extends RuntimeException {
    public MineSweeperGameException(String message) {
        super(message);
    }

    public MineSweeperGameException(String message, Throwable cause) {
        super(message, cause);
    }
}