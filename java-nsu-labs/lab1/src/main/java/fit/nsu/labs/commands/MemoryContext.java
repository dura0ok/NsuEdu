package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.InvalidCommandArgument;
import fit.nsu.labs.exceptions.NotEnoughArgumentsInStack;

import java.util.ArrayDeque;
import java.util.Deque;
import java.util.HashMap;
import java.util.Map;

public class MemoryContext implements Context {
    private final Deque<Double> stack;

    private final Map<String, Double> defines;

    public MemoryContext() {
        defines = new HashMap<>();
        stack = new ArrayDeque<>();
    }

    public MemoryContext(Deque<Double> inputStack, Map<String, Double> inputDefines) {
        defines = new HashMap<>(inputDefines);
        stack = new ArrayDeque<>(inputStack);
    }

    public double popStack() {
        return stack.pop();
    }

    public int getStackSize() {
        return stack.size();
    }

    public void pushStack(double input) {
        stack.push(input);
    }

    public double peekStack() throws CalcException {
        var retValue = stack.peek();
        if (retValue == null) {
            throw new NotEnoughArgumentsInStack("Stack empty");
        }
        return retValue;
    }

    public void defineNumber(String key, double value) {
        defines.put(key, value);
    }

    public boolean isDefined(String key) {
        return defines.containsKey(key);
    }

    public double getDefinedByKey(String key) throws CalcException {
        if (!defines.containsKey(key)) {
            throw new InvalidCommandArgument("can`t find in defined variables this key: " + key);
        }
        return defines.get(key);
    }
}
