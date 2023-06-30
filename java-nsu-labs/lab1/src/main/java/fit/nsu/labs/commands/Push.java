package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.InvalidCommandArgument;

import java.util.logging.Level;

import static fit.nsu.labs.Utils.isNotDouble;

public class Push extends AbstractCommand {

    public Push(String[] inputArgs) throws CalcException {
        super(inputArgs);
    }

    @Override
    public String getCommandDescription() {
        return "Push variable or num to stack";
    }

    @Override
    public int getNumberNeededArgs() {
        return 1;
    }

    @Override
    public int getNumberMinimumNeededStackSize() {
        return 0;
    }

    @Override
    public void execute(Context context) throws CalcException {
        var arg = getArgs()[0];

        if (isNotDouble(arg)) {


            try {
                var value = context.getDefinedByKey(arg);
                context.pushStack(value);
            } catch (CalcException e) {
                getLogger().log(
                        Level.WARNING,
                        e.getMessage()
                );
                throw new InvalidCommandArgument(
                        getCommandName(), "push argument must be number or defined value"
                );
            }
            return;
        }

        context.pushStack(Double.parseDouble(arg));

    }


}
