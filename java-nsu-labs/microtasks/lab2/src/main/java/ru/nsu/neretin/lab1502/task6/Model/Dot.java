package ru.nsu.neretin.lab1502.task6.Model;

public class Dot implements Comparable<Dot>{
    private final double x;
    private final double y;

    public Dot(double x, double y) {
        this.x = x;
        this.y = y;
    }

    private double distance(){
        return x*x + y*y;
    }

    @Override
    public String toString() {
        return "Dot{" +
                "x=" + x +
                ", y=" + y +
                '}';
    }

    @Override
    public int compareTo(Dot o) {
        return Double.compare(this.distance(), o.distance());
    }
}
