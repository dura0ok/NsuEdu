import fit.nsu.labs.commands.Define;
import fit.nsu.labs.commands.MemoryContext;
import fit.nsu.labs.exceptions.BadNumberOfArguments;
import fit.nsu.labs.exceptions.CalcException;
import org.junit.jupiter.api.Test;

import java.util.ArrayDeque;
import java.util.HashMap;

import static org.junit.Assert.assertThrows;
import static org.junit.jupiter.api.Assertions.*;

public class DefineCommandTest {
    @Test
    void defineOneCorrectConst() {
        try {
            var defines = new HashMap<String, Double>();
            var ctx = new MemoryContext(new ArrayDeque<>(), defines);
            var defineCommand = new Define(new String[]{"a", "4.0"});
            defineCommand.execute(ctx);
            assertTrue(ctx.isDefined("a"));
            assertEquals(4.0, ctx.getDefinedByKey("a"));
        } catch (Exception ignored) {
            fail();
        }
    }


    @Test
    void TryToDefineWithoutArgs() throws CalcException {
        var defines = new HashMap<String, Double>();
        var defineCommand = new Define(new String[]{});
        assertThrows(BadNumberOfArguments.class, () -> defineCommand.execute(new MemoryContext(new ArrayDeque<>(), defines)));
    }

    @Test
    void TryToDefineWithoutValue() throws CalcException {
        var defines = new HashMap<String, Double>();
        var defineCommand = new Define(new String[]{"a"});
        assertThrows(BadNumberOfArguments.class, () -> defineCommand.execute(new MemoryContext(new ArrayDeque<>(), defines)));
    }


}
