package fit.nsu.labs.commands.strategies;

public class MultiplyStrategy implements Strategy{
    @Override
    public double performOperation(double a, double b) {
        return a * b;
    }
}
