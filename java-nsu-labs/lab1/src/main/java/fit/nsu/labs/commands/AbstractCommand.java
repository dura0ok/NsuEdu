package fit.nsu.labs.commands;

import fit.nsu.labs.CalcLogger;
import fit.nsu.labs.exceptions.BadNumberOfArguments;
import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.NotEnoughArgumentsInStack;

import java.util.logging.Logger;

public abstract class AbstractCommand implements Command {
    private final Logger logger;
    private final String[] args;

    AbstractCommand(String[] inputArgs) throws CalcException {
        args = inputArgs;
        logger = CalcLogger.getLogger(this.getClass());
    }


    public abstract String getCommandDescription();

    public abstract void execute(Context context) throws CalcException;

    public String getCommandName() {
        return this.getClass().getName();
    }

    public String[] getArgs() {
        return args;
    }

    protected Logger getLogger() {
        return logger;
    }


}
