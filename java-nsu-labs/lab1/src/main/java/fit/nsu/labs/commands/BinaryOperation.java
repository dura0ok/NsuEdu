package fit.nsu.labs.commands;

import fit.nsu.labs.commands.strategies.Strategy;
import fit.nsu.labs.exceptions.CalcException;

public class BinaryOperation extends AbstractCommand{
    private final Strategy strategy;
    public BinaryOperation(String[] inputArgs, Strategy strategy) throws CalcException {
        super(inputArgs);
        this.strategy = strategy;
    }

    @Override
    public int getNumberNeededArgs() {
        return 0;
    }

    @Override
    public int getNumberMinimumNeededStackSize() {
        return 2;
    }

    @Override
    public String getCommandDescription() {
        return "Add two numbers from stack. Result put in stack.";
    }

    @Override
    public void execute(Context context) throws CalcException {
        var firstNum = context.popStack();
        var secondNum = context.popStack();
        context.pushStack(strategy.performOperation(firstNum, secondNum));
    }
}
