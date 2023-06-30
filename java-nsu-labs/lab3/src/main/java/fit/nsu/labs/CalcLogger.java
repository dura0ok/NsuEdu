package fit.nsu.labs;

import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

import static java.util.logging.LogManager.getLogManager;

public class CalcLogger {
    private CalcLogger() {
    }

    public static Logger getLogger(Class<?> InputClass) {
        try {
            try (InputStream stream = ClassLoader.getSystemResourceAsStream("logging.properties")) {
                getLogManager().readConfiguration(stream);
                return Logger.getLogger(InputClass.getSimpleName());
            }
        } catch (IOException exception) {
            throw new RuntimeException("can`t open/find log file. Detailed: " + exception.getMessage(), exception);
        }
    }
}