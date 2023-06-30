package fit.nsu.labs.commands;

import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.NotEnoughArgumentsInStack;

public class Print extends AbstractCommand {
    public Print(String[] inputArgs) throws CalcException {
        super(inputArgs);
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
    public String getCommandDescription() {
        return "Print num from stack. Not pop!";
    }

    @Override
    public void execute(Context context) throws CalcException {
        try {
            System.out.println(context.peekStack());
        } catch (CalcException e) {
            throw new NotEnoughArgumentsInStack(
                    getCommandName(),
                    "Nothing to print. Stack empty."
            );
        }
    }
}
