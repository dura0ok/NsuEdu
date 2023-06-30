package fit.nsu.labs;

import fit.nsu.labs.exceptions.CalcException;
import fit.nsu.labs.exceptions.ConfigurationException;

import java.io.IOException;
import java.io.InputStream;
import java.util.logging.Logger;

import static java.util.logging.LogManager.getLogManager;

public class CalcLogger {
    private CalcLogger() {
    }

    public static Logger getLogger(Class<?> InputClass) throws CalcException {
        try {
            try (InputStream stream = ClassLoader.getSystemResourceAsStream("logging.properties")) {
                getLogManager().readConfiguration(stream);
                return Logger.getLogger(InputClass.getSimpleName());
            }
        } catch (IOException exception) {
            throw new ConfigurationException("can`t open/find log file. Detailed: " + exception.getMessage(), exception);
        }
    }
}
