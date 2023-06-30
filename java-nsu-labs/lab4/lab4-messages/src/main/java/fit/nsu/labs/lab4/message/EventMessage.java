package fit.nsu.labs.lab4.message;

import java.util.Objects;

/**
 * <p>Сообщение вида "событие" от сервера клиенту.</p>
 * <p>Примеры:</p>
 *
 * <p>- Сообщение от сервера клиенту</p>
 * <pre>
 *      &lt;event name=&quot;message&quot;&gt;
 *          &lt;message&gt;MESSAGE&lt;/message&gt;
 *          &lt;name&gt;CHAT_NAME_FROM&lt;/name&gt;
 *      &lt;/event&gt;
 * </pre>
 *
 * <p>- Новый клиент</p>
 * <pre>
 *      &lt;event name=”userlogin”&gt;
 *          &lt;name&gt;USER_NAME&lt;/name&gt;
 *      &lt;/event&gt;
 * </pre>
 *
 * <p>- Клиент отключился</p>
 * <pre>
 *      &lt;event name=”userlogout”&gt;
 *          &lt;name&gt;USER_NAME&lt;/name&gt;
 *      &lt;/event&gt;
 * </pre>
 */
public class EventMessage {
    /**
     * Наименование события
     */
    private final String eventName;

    private final String message;
    private final String name;

    private EventMessage(String eventName, String message, String name) {
        this.eventName = Objects.requireNonNull(eventName);
        this.message = message;
        this.name = name;
    }

    public static Builder builder() {
        return new Builder();
    }

    public String getEventName() {
        return eventName;
    }

    public String getMessage() {
        return message;
    }

    public String getName() {
        return name;
    }

    public static class Builder {
        private String eventName;
        private String message;
        private String name;

        public Builder event(String name) {
            this.eventName = Objects.requireNonNull(name);
            return this;
        }

        public Builder message(String message) {
            this.message = Objects.requireNonNull(message);
            return this;
        }

        public Builder name(String name) {
            this.name = Objects.requireNonNull(name);
            return this;
        }

        public EventMessage build() {
            return new EventMessage(eventName, message, name);
        }
    }
}
