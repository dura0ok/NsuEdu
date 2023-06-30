package fit.nsu.labs.exceptions;

public class BadNumberOfArguments extends CalcException {
    public BadNumberOfArguments(String command, int expected, int got) {
        super(String.format("Bad number of arguments in command %s. Expected %d but got %d", command, expected, got));
    }

    public BadNumberOfArguments(String s) {
        super(s);
    }
}
