package fit.nsu.labs.commands;

import fit.nsu.labs.commands.strategies.*;
import fit.nsu.labs.exceptions.CalcException;

import java.util.Map;

public class ArithmeticCommand implements Command {
    private String commandName;
    private final Map<String, Strategy> realizations = Map.of(
            "+", new AddStrategy(),
            "-", new SubstractStrategy(),
            "*", new MultiplyStrategy(),
            "/", new DivideStrategy()
    );

    public void setCommandName(String commandName) {
        this.commandName = commandName;
    }

    @Override
    public String getCommandName() {
        return commandName;
    }

    @Override
    public String getCommandDescription() {
        return null;
    }

    @Override
    public String[] getArgs() {
        return new String[]{};
    }


    @Override
    public void execute(Context context) throws CalcException {
        var res = realizations.get(getCommandName()).performOperation(context.popStack(), context.popStack());
        context.pushStack(res);
    }

    @Override
    public int getNumberNeededArgs() {
        return 0;
    }

    @Override
    public int getNumberMinimumNeededStackSize() {
        return 0;
    }
}
