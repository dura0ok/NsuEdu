package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;

public interface Command {
    String getCommandName();

    String getCommandDescription();

    String[] getArgs();

    void execute(Context context) throws CalcException;


    int getNumberNeededArgs();

    int getNumberMinimumNeededStackSize();

}
