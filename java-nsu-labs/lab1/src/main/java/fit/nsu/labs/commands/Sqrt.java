package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.InvalidCommandArgument;

public class Sqrt extends AbstractCommand {

    public Sqrt(String[] inputArgs) throws CalcException {
        super(inputArgs);
    }

    @Override
    public String getCommandDescription() {
        return "Sqrt arithmetic root from num in stack. Result put in stack.";
    }

    @Override
    public int getNumberNeededArgs() {
        return 0;
    }

    @Override
    public int getNumberMinimumNeededStackSize() {
        return 1;
    }

    @Override
    public void execute(Context context) throws CalcException {
        double num = context.popStack();
        if (num < 0) {
            throw new InvalidCommandArgument(getCommandName(), "num must be >= 0");
        }
        context.pushStack(Math.sqrt(num));
    }
}
