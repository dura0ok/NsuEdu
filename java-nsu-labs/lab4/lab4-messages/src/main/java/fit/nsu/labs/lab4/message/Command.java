package fit.nsu.labs.lab4.message;

import java.util.Objects;

/**
 * <p>Сообщение вида "команда" от клиента серверу.</p>
 * <p>Примеры:</p>
 *
 * <p>- Регистрация</p>
 * <pre>
 *      &lt;command name=”login”&gt;
 *          &lt;name&gt;USER_NAME&lt;/name&gt;
 *          &lt;type&gt;CHAT_CLIENT_NAME&lt;/type&gt;
 *      &lt;/command&gt;
 * </pre>
 *
 * <p>- Запрос списка пользователей чата</p>
 * <pre>
 *      &lt;command name=”list”&gt;
 *          &lt;session&gt;UNIQUE_SESSION_ID&lt;/session&gt;
 *      &lt;/command&gt;
 * </pre>
 *
 * <p>- Сообщение от клиента серверу</p>
 * <pre>
 *      &lt;command name=”message”&gt;
 *          &lt;message&gt;MESSAGE&lt;/message&gt;
 *          &lt;session&gt;UNIQUE_SESSION_ID&lt;/session&gt;
 *      &lt;/command&gt;
 * </pre>
 *
 * <p>- Отключение</p>
 * <pre>
 *      &lt;command name=”logout”&gt;
 *          &lt;session&gt;UNIQUE_SESSION_ID&lt;/session&gt;
 *      &lt;/command&gt;
 * </pre>
 */
public class Command {
    /**
     * Наименование команды.
     */
    private final String commandName;

    private final String name;
    private final String type;
    private final String session;
    private final String message;

    private Command(String commandName, String name, String type, String session, String message) {
        this.commandName = commandName;
        this.name = name;
        this.type = type;
        this.session = session;
        this.message = message;
    }

    public static Builder builder() {
        return new Builder();
    }

    public String getCommandName() {
        return commandName;
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    public String getSession() {
        return session;
    }

    public String getMessage() {
        return message;
    }

    public static class Builder {
        private String command;
        private String name;
        private String type;
        private String session;
        private String message;

        public Builder command(String command) {
            this.command = Objects.requireNonNull(command);
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder type(String type) {
            this.type = type;
            return this;
        }

        public Builder session(String session) {
            this.session = session;
            return this;
        }

        public Builder message(String message) {
            this.message = message;
            return this;
        }

        public Command build() {
            return new Command(command, name, type, session, message);
        }
    }
}
