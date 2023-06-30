package fit.nsu.labs.lab4.serializer;

import fit.nsu.labs.lab4.exception.DeserializeException;
import fit.nsu.labs.lab4.message.Command;
import fit.nsu.labs.lab4.message.Error;
import fit.nsu.labs.lab4.message.EventMessage;
import fit.nsu.labs.lab4.message.Success;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.SAXException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.function.Function;

public class XmlDeserializer implements Deserializer {
    private final DocumentBuilder documentBuilder;
    private final Map<String, Function<Element, Object>> mapDeserializers;

    public XmlDeserializer() {
        try {
            documentBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
        } catch (ParserConfigurationException e) {
            throw new RuntimeException("Error create DocumentBuilder. WHAT!?", e);
        }

        mapDeserializers = Map.of(
                "command", this::deserializeCommand,
                "error", this::deserializeError,
                "event", this::deserializeEvent,
                "success", this::deserializeSuccess
        );
    }

    private static String trimXML(String input) {
        BufferedReader reader = new BufferedReader(new StringReader(input));
        StringBuilder result = new StringBuilder();
        try {
            String line;
            while ((line = reader.readLine()) != null)
                result.append(line.trim());
            return result.toString();
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public Object deserialize(InputStream inputStream) throws DeserializeException {
        Document document;
        try {
            var dis = new DataInputStream(inputStream);
            var length = dis.readInt();
            var xmlBytes = dis.readNBytes(length);


            document = documentBuilder.parse(new ByteArrayInputStream(trimXML(new String(xmlBytes, StandardCharsets.UTF_8)).getBytes()));
        } catch (IOException e) {
            throw new DeserializeException("Error read from InputStream", e);
        } catch (SAXException e) {
            throw new DeserializeException("Error parse XML from InputStream", e);
        }

        var rootElm = document.getDocumentElement();
        var rootElmTag = rootElm.getTagName().toLowerCase();
        var function = mapDeserializers.get(rootElmTag);
        if (function == null) {
            System.err.printf("[WARN] Unsupported message '%s'\n", rootElmTag);
            return null;
        }

        return function.apply(rootElm);
    }

    private Command deserializeCommand(Element rootElm) {
        var commandName = rootElm.getAttribute("name").toLowerCase();
        var builder = Command.builder().command(commandName);
        var nodeList = rootElm.getChildNodes();

        switch (commandName) {
            case "login" -> {
                var nameElm = (Element) nodeList.item(0);
                builder.name(nameElm.getTextContent().strip());
                var typeElm = (Element) nodeList.item(1);
                builder.type(typeElm.getTextContent().strip());
            }
            case "list", "logout" -> {
                var sessionElm = (Element) nodeList.item(0);
                builder.session(sessionElm.getTextContent());
            }
            case "message" -> {
                var messageElm = (Element) nodeList.item(0);
                builder.message(messageElm.getTextContent());
                var sessionElm = (Element) nodeList.item(1);
                builder.session(sessionElm.getTextContent());
            }
            default -> System.err.printf("[WARN] Unsupported command '%s'\n", commandName);
        }

        return builder.build();
    }

    private Error deserializeError(Element rootElm) {
        var nodeList = rootElm.getChildNodes();
        var messageElm = (Element) nodeList.item(0);

        return new Error(messageElm.getTextContent());
    }

    private EventMessage deserializeEvent(Element rootElm) {
        var eventName = rootElm.getAttribute("name").toLowerCase();
        var builder = EventMessage.builder().event(eventName);
        var nodeList = rootElm.getChildNodes();

        switch (eventName) {
            case "message" -> {
                var messageElm = (Element) nodeList.item(0);
                builder.message(messageElm.getTextContent());
                var nameElm = (Element) nodeList.item(1);
                builder.name(nameElm.getTextContent());
            }
            case "userlogin", "userlogout" -> {
                var nameElm = (Element) nodeList.item(0);
                builder.name(nameElm.getTextContent());
            }
            default -> System.err.printf("[WARN] Unsupported event '%s'\n", eventName);
        }

        return builder.build();
    }

    private Success deserializeSuccess(Element rootElm) {
        var builder = Success.builder();
        var nodeList = rootElm.getChildNodes();

        if (nodeList.getLength() > 0) {
            var firstElm = (Element) nodeList.item(0);

            var firstElmTag = firstElm.getTagName().toLowerCase();
            switch (firstElmTag) {
                case "session" -> builder.session(firstElm.getTextContent());
                case "listusers" -> {
                    var userNodes = firstElm.getChildNodes();
                    List<Success.User> listUsers = new ArrayList<>(userNodes.getLength());

                    for (int i = 0; i < userNodes.getLength(); i++) {
                        var userElm = userNodes.item(i);
                        var userDataNodes = userElm.getChildNodes();

                        listUsers.add(new Success.User(
                                userDataNodes.item(0).getTextContent(), //name
                                userDataNodes.item(1).getTextContent()  //type
                        ));
                    }

                    builder.listUsers(listUsers);
                }
                default -> System.err.printf("[WARN] Unsupported success tag '%s'\n", firstElmTag);
            }
        }

        return builder.build();
    }
}

