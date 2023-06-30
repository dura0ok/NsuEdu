package fit.nsu.labs.model;

public record HighScore(String name, long secondsTime) {

    @Override
    public String toString() {
        return String.format("%s %d\n", name, secondsTime);
    }
}
