package fit.nsu.labs;

import fit.nsu.labs.commands.Command;
import fit.nsu.labs.commands.Context;
import fit.nsu.labs.commands.MemoryContext;
import fit.nsu.labs.exceptions.BadNumberOfArguments;
import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.NotEnoughArgumentsInStack;

import java.io.InputStream;
import java.util.logging.Level;
import java.util.logging.Logger;

public class CalcExecutor {
    private final Logger logger;
    private final CommandParser parser;

    public CalcExecutor(InputStream input) throws CalcException {
        parser = new CommandParser(input);
        logger = CalcLogger.getLogger(this.getClass());
    }

    public void calculate() throws Exception {
        var context = new MemoryContext();
        logger.log(Level.INFO, "start execute commands");
        Command command;
        while ((command = parser.parseCommand()) != null) {
            logger.log(Level.INFO, "Start execute: " + command.getCommandName() + "\n");
            try {
                validateNumberOfArgs(command);
                validateMinimumNeededStackSize(command, context);
                command.execute(context);
                logger.log(Level.INFO, "end execute without errors");
            } catch (CalcException e) {
                logger.warning(e.getMessage());
            }

        }
        logger.log(Level.INFO, "Execute all commands. Finish");
    }

    private void validateNumberOfArgs(Command command) throws CalcException {
        if (command.getArgs().length != command.getNumberNeededArgs()) {
            throw new BadNumberOfArguments(command.getCommandName(), 0, command.getArgs().length);
        }
    }

    private void validateMinimumNeededStackSize(Command command, Context context) throws CalcException {
        if (context.getStackSize() < command.getNumberMinimumNeededStackSize()) {
            throw new NotEnoughArgumentsInStack(command.getCommandName(), command.getNumberMinimumNeededStackSize(), context.getStackSize());
        }
    }
}
