import fit.nsu.labs.commands.MemoryContext;
import fit.nsu.labs.commands.Sqrt;
import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.InvalidCommandArgument;
import fit.nsu.labs.exceptions.NotEnoughArgumentsInStack;
import org.junit.jupiter.api.Test;

import java.util.ArrayDeque;
import java.util.HashMap;

import static org.junit.Assert.assertThrows;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;

public class SqrtCommandTest {

    @Test
    void sqrtNaturalSquare() {
        var stack = new ArrayDeque<Double>();
        stack.push(16.0);
        var ctx = new MemoryContext(stack, new HashMap<>());
        try {
            var sqrtCommand = new Sqrt(new String[]{});
            sqrtCommand.execute(ctx);
            assertEquals(4, ctx.popStack());

        } catch (Exception ignored) {
            fail();
        }

    }

    @Test
    void sqrtRealNumber() {
        var stack = new ArrayDeque<Double>();
        stack.push(2.25);
        try {
            var ctx = new MemoryContext(stack, new HashMap<>());
            var addCommand = new Sqrt(new String[]{});
            addCommand.execute(ctx);
            assertEquals(1.5, ctx.popStack());
        } catch (Exception ignored) {
            fail();
        }

    }

    @Test
    void sqrtNegativeNumber() throws CalcException {
        var stack = new ArrayDeque<Double>();
        stack.push(-1.0);
        var sqrtCommand = new Sqrt(new String[]{});
        assertThrows(InvalidCommandArgument.class, () -> sqrtCommand.execute(new MemoryContext(stack, new HashMap<>())));

    }

    @Test
    void emptyStackError() throws CalcException {
        var stack = new ArrayDeque<Double>();
        var sqrtCommand = new Sqrt(new String[]{});
        assertThrows(NotEnoughArgumentsInStack.class, () -> sqrtCommand.execute(new MemoryContext(stack, new HashMap<>())));
    }
}
