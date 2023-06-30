package fit.nsu.labs.model;

public class GameTime implements Runnable {
    private final GameField field;
    private long secondsAlreadyRun;

    public GameTime(GameField field) {
        this.field = field;
    }

    public void reset() {
        secondsAlreadyRun = 0;
    }

    public void updateTimer() {
        secondsAlreadyRun++;
    }

    public long getElapsed() {
        return secondsAlreadyRun;
    }

    @Override
    public void run() {
        updateTimer();
        field.notifyObservers(new Event(EventType.REDRAW_TIMER, field));
    }
}