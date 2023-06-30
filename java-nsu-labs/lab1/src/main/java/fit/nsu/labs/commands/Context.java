package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;

public interface Context {
    double popStack();

    int getStackSize();

    void pushStack(double input);

    double peekStack() throws CalcException;

    void defineNumber(String key, double value);

    boolean isDefined(String key);

    double getDefinedByKey(String key) throws CalcException;
}
