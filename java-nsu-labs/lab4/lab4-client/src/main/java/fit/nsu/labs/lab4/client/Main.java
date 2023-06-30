package fit.nsu.labs.lab4.client;

import fit.nsu.labs.lab4.client.model.Lab4Client;
import fit.nsu.labs.lab4.client.model.handlers.ErrorHandler;
import fit.nsu.labs.lab4.client.model.handlers.EventHandler;
import fit.nsu.labs.lab4.client.model.handlers.SuccessHandler;
import fit.nsu.labs.lab4.client.ui.ConnectionDialog;
import fit.nsu.labs.lab4.client.ui.MainWindow;
import fit.nsu.labs.lab4.message.Error;
import fit.nsu.labs.lab4.message.EventMessage;
import fit.nsu.labs.lab4.message.Success;
import fit.nsu.labs.lab4.serializer.XmlDeserializer;
import fit.nsu.labs.lab4.serializer.XmlSerializer;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.util.concurrent.atomic.AtomicReference;

public class Main {

    public static void main(String[] args) throws IOException {
        var connectionDialog = new ConnectionDialog();
        connectionDialog.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
        //connectionDialog.setHost("127.0.0.1");
        connectionDialog.setHost("95.170.154.53");
        //connectionDialog.setPort(7777);
        connectionDialog.setPort(8888);
        connectionDialog.setUserName("Durachok");
        connectionDialog.setVisible(true);

        if (!connectionDialog.isOk()) {
            System.exit(0);
            return;
        }

        var mainWindow = new MainWindow(connectionDialog.getUserName());
        var dim = Toolkit.getDefaultToolkit().getScreenSize();
        mainWindow.setLocation(
                dim.width / 2 - mainWindow.getSize().width / 2,
                dim.height / 2 - mainWindow.getSize().height / 2);
        mainWindow.setVisible(true);


        var lab4Client = new Lab4Client(new XmlSerializer(), new XmlDeserializer());


        var refSessionId = new AtomicReference<String>();
        lab4Client.registerHandler(Success.class, new SuccessHandler(refSessionId));
        lab4Client.registerHandler(EventMessage.class, new EventHandler());
        lab4Client.registerHandler(Error.class, new ErrorHandler());

        lab4Client.registerObserver(mainWindow);

        mainWindow.onSendClick(message -> lab4Client.sendMessage(refSessionId.get(), message));
        mainWindow.onCloseWindow(() -> lab4Client.logout(refSessionId.get(), () -> {
            mainWindow.dispose();
            try {
                lab4Client.close();
            } catch (Exception ignore) {
                //ignore
            }
            System.exit(0);
        }));
        mainWindow.setRequestUpdateMembers(() -> lab4Client.requestMembersList(refSessionId.get()));

        lab4Client.connect(connectionDialog.getHost(), connectionDialog.getPort());
        lab4Client.login(connectionDialog.getUserName());
    }
}
