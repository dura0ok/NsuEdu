package fit.nsu.labs.lab4.message;

import java.util.List;
import java.util.Objects;

/**
 * <p>Сообщение вида "успех" от сервера клиенту.</p>
 * <p>Примеры:</p>
 *
 * <p>- Регистрация</p>
 * <pre>
 *      &lt;success&gt;
 *          &lt;session&gt;UNIQUE_SESSION_ID&lt;/session&gt;
 *      &lt;/success&gt;
 * </pre>
 *
 * <p>- Запрос списка пользователей чата</p>
 * <pre>
 *      &lt;success&gt;
 *          &lt;listusers&gt;
 *              &lt;user&gt;
 *                  &lt;name&gt;USER_1&lt;/name&gt;
 *                  &lt;type&gt;CHAT_CLIENT_1&lt;/type&gt;
 *              &lt;/user&gt;
 *              ...
 *              &lt;user&gt;
 *                  &lt;name&gt;USER_N&lt;/name&gt;
 *                  &lt;type&gt;CHAT_CLIENT_N&lt;/type&gt;
 *              &lt;/user&gt;
 *          &lt;/listusers&gt;
 *      &lt;/success&gt;
 * </pre>
 *
 * <p>- Сообщение от клиента серверу</p>
 * <p>- Отключение</p>
 * <pre>
 *      &lt;success&gt;
 *      &lt;/success&gt;
 * </pre>
 */
public class Success {
    private final String session;
    private final List<User> listUsers;

    private Success(String session, List<User> listUsers) {
        this.session = session;
        this.listUsers = listUsers;
    }

    public static Builder builder() {
        return new Builder();
    }

    public String getSession() {
        return session;
    }

    public List<User> getListUsers() {
        return listUsers;
    }

    public static class Builder {
        private String session;
        private List<User> listUsers;

        public Builder session(String session) {
            this.session = Objects.requireNonNull(session);
            return this;
        }

        public Builder listUsers(List<User> listUsers) {
            this.listUsers = Objects.requireNonNull(listUsers);
            return this;
        }

        public Success build() {
            return new Success(session, listUsers);
        }
    }

    public static class User {
        private final String name;
        private final String type;

        public User(String name, String type) {
            this.name = Objects.requireNonNull(name);
            this.type = Objects.requireNonNull(type);
        }

        public String getName() {
            return name;
        }

        public String getType() {
            return type;
        }
    }
}
