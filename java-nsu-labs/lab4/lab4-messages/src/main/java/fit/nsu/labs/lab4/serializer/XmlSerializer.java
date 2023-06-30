package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.message.Command;
import fit.nsu.labs.lab4.message.Error;
import fit.nsu.labs.lab4.message.EventMessage;
import fit.nsu.labs.lab4.message.Success;
import org.w3c.dom.Document;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.OutputStreamWriter;
import java.util.Map;
import java.util.function.Function;

public class XmlSerializer implements Serializer {
    private final DocumentBuilder documentBuilder;
    private final Transformer transformer;
    private final Map<Class<?>, Function<Object, byte[]>> mapSerializers;

    public XmlSerializer() {
        try {
            documentBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();

            var transformerFactory = TransformerFactory.newInstance();
            transformer = transformerFactory.newTransformer();
            transformer.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
            transformer.setOutputProperty(OutputKeys.INDENT, "no");
        } catch (ParserConfigurationException e) {
            throw new RuntimeException("Error create DocumentBuilder. WHAT!?", e);
        } catch (TransformerConfigurationException e) {
            throw new RuntimeException("Error create Transformer. WHAT!?", e);
        }

        mapSerializers = Map.of(
                Command.class, this::serializeCommand,
                Error.class, this::serializeError,
                EventMessage.class, this::serializeEvent,
                Success.class, this::serializeSuccess
        );
    }

    @Override
    public byte[] serialize(Object object) {
        var function = mapSerializers.get(object.getClass());
        if (function == null) {
            System.err.printf("[WARN] Unsupported serialize class '%s'\n", object.getClass());
            return new byte[0];
        }

        return function.apply(object);
    }

    private byte[] serializeCommand(Object object) {
        var command = (Command) object;

        var document = documentBuilder.newDocument();
        var rootElm = document.createElement("command");
        rootElm.setAttribute("name", command.getCommandName().toLowerCase());

        switch (command.getCommandName()) {
            case "login" -> {
                var nameElm = document.createElement("name");
                nameElm.setTextContent(command.getName());
                var typeElm = document.createElement("type");
                typeElm.setTextContent(command.getType());

                rootElm.appendChild(nameElm);
                rootElm.appendChild(typeElm);
            }
            case "logout", "list" -> {
                var sessionElm = document.createElement("session");
                sessionElm.setTextContent(command.getSession());

                rootElm.appendChild(sessionElm);
            }
            case "message" -> {
                var messageElm = document.createElement("message");
                messageElm.setTextContent(command.getMessage());
                var sessionElm = document.createElement("session");
                sessionElm.setTextContent(command.getSession());

                rootElm.appendChild(messageElm);
                rootElm.appendChild(sessionElm);
            }
            default -> System.err.printf("[WARN] Unsupported command '%s'\n", command.getCommandName());
        }

        document.appendChild(rootElm);
        return toBytes(document);
    }

    private byte[] serializeError(Object object) {
        var error = (Error) object;

        var document = documentBuilder.newDocument();
        var rootElm = document.createElement("error");

        var messageElm = document.createElement("message");
        messageElm.setTextContent(error.getMessage());

        rootElm.appendChild(messageElm);
        document.appendChild(rootElm);
        return toBytes(document);
    }

    private byte[] serializeEvent(Object object) {
        var event = (EventMessage) object;

        var document = documentBuilder.newDocument();
        var rootElm = document.createElement("event");
        rootElm.setAttribute("name", event.getEventName());

        switch (event.getEventName()) {
            case "message" -> {
                var messageElm = document.createElement("message");
                messageElm.setTextContent(event.getMessage());
                var nameElm = document.createElement("name");
                nameElm.setTextContent(event.getName());

                rootElm.appendChild(messageElm);
                rootElm.appendChild(nameElm);
            }
            case "userlogin", "userlogout" -> {
                var nameElm = document.createElement("name");
                nameElm.setTextContent(event.getName());

                rootElm.appendChild(nameElm);
            }
            default -> System.err.printf("[WARN] Unsupported event '%s'\n", event.getEventName());
        }

        document.appendChild(rootElm);
        return toBytes(document);
    }

    private byte[] serializeSuccess(Object object) {
        var success = (Success) object;

        var document = documentBuilder.newDocument();
        var rootElm = document.createElement("success");

        if (success.getSession() != null) {
            var sessionElm = document.createElement("session");
            sessionElm.setTextContent(success.getSession());

            rootElm.appendChild(sessionElm);
        } else if (success.getListUsers() != null) {
            var listusersElm = document.createElement("listusers");
            success.getListUsers().forEach(u -> {
                var userElm = document.createElement("user");
                var nameElm = document.createElement("name");
                nameElm.setTextContent(u.getName());
                var typeElm = document.createElement("type");
                typeElm.setTextContent(u.getType());

                userElm.appendChild(nameElm);
                userElm.appendChild(typeElm);

                listusersElm.appendChild(userElm);
            });

            rootElm.appendChild(listusersElm);
        }

        document.appendChild(rootElm);
        return toBytes(document);
    }

    private byte[] toBytes(Document document) {
        var source = new DOMSource(document);
        var baos = new ByteArrayOutputStream();
        var streamResult = new StreamResult(new OutputStreamWriter(baos));

        try {
            transformer.transform(source, streamResult);
            byte[] bytes = baos.toByteArray();

            baos = new ByteArrayOutputStream();
            var dos = new DataOutputStream(baos);
            dos.writeInt(bytes.length);
            dos.write(bytes);
            dos.flush();
        } catch (Exception e) {
            throw new RuntimeException("Error convert xml document to bytes", e);
        }

        return baos.toByteArray();
    }
}
