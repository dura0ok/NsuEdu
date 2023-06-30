package fit.nsu.labs.model;

public record Dot(int x, int y) {
    @Override
    public String toString() {
        return "Dot{" +
                "x=" + x +
                ", y=" + y +
                '}';
    }
}
