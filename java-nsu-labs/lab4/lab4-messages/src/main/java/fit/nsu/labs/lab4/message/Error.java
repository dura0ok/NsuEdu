package fit.nsu.labs.lab4.message;

import java.util.Objects;

/**
 * <p>Сообщение вида "ошибка" от сервера клиенту.</p>
 * <p>Пример:</p>
 * <pre>
 *      &lt;error&gt;
 *          &lt;message&gt;REASON&lt;/message&gt;
 *      &lt;/error&gt;
 * </pre>
 */
public class Error {
    private final String message;

    public Error(String message) {
        this.message = Objects.requireNonNull(message);
    }

    public String getMessage() {
        return message;
    }
}
