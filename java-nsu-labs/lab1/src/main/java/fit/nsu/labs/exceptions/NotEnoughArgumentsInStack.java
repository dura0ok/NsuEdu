package fit.nsu.labs.exceptions;

public class NotEnoughArgumentsInStack extends CalcException {
    public NotEnoughArgumentsInStack(String message) {
        super(message);
    }

    public NotEnoughArgumentsInStack(String commandName, String argName) {
        super(String.format("Empty Stack in command %s when trying to get argument %s", argName, commandName));
    }

    public NotEnoughArgumentsInStack(String commandName, int expectedSize, int realitySize) {
        super(String.format(
                "Error in %s Expected elements in stack is minimum %s but reality size is %s",
                commandName, expectedSize, realitySize)
        );
    }
}
