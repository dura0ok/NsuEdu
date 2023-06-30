package fit.nsu.labs.lab4.client.ui;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.util.ArrayList;
import java.util.List;

public class ConnectionDialog extends JDialog {
    private JTextField inputHost;
    private JTextField inputPort;
    private JTextField inputName;
    private boolean ok = false;

    public ConnectionDialog() throws HeadlessException {
        setModalityType(ModalityType.DOCUMENT_MODAL);
        setTitle("Connection Dialog");
        setSize(342, 164);
        setResizable(false);
        setLocationRelativeTo(null);
        setLayout(new BorderLayout());

        var mainPanel = new JPanel();
        mainPanel.setLayout(new BorderLayout());
        mainPanel.setBorder(new EmptyBorder(new Insets(5, 5, 5, 5)));

        var connectButton = new JButton();
        connectButton.setText("Connect");
        connectButton.addActionListener(this::connectButtonClick);

        mainPanel.add(createInputPanel(), BorderLayout.CENTER);
        mainPanel.add(connectButton, BorderLayout.SOUTH);

        add(mainPanel);
    }

    public String getHost() {
        return inputHost.getText();
    }

    public void setHost(String value) {
        inputHost.setText(value);
    }

    public Integer getPort() {
        try {
            return Integer.valueOf(inputPort.getText());
        } catch (NumberFormatException ignore) {
            return null;
        }
    }

    public void setPort(int port) {
        inputPort.setText(String.valueOf(port));
    }

    public String getUserName() {
        return inputName.getText();
    }

    public void setUserName(String value) {
        inputName.setText(value);
    }

    public boolean isOk() {
        return ok;
    }

    private JPanel createInputPanel() {
        var panel = new JPanel();
        panel.setBorder(new EmptyBorder(new Insets(0, 0, 5, 0)));
        panel.setLayout(new GridLayout(3, 2));

        var labelHost = new JLabel();
        labelHost.setText("Host:");
        panel.add(labelHost);

        inputHost = new JTextField();
        panel.add(inputHost);

        var labelPort = new JLabel();
        labelPort.setText("Port:");
        panel.add(labelPort);

        inputPort = new JTextField();
        panel.add(inputPort);

        var labelName = new JLabel();
        labelName.setText("User name:");
        panel.add(labelName);

        inputName = new JTextField();
        panel.add(inputName);

        return panel;
    }

    private void connectButtonClick(ActionEvent event) {
        List<String> messages = new ArrayList<>();
        if (getPort() == null) {
            if (inputPort.getText().isEmpty()) {
                messages.add("PORT IS EMPTY!");
            } else {
                messages.add("INCORRECT PORT '%s'!".formatted(inputPort.getText()));
            }
        }

        if (getHost().isEmpty()) {
            messages.add("HOST IS EMPTY!");
        }

        if (getUserName().isEmpty()) {
            messages.add("USER NAME IS EMPTY!");
        }

        if (!messages.isEmpty()) {
            JOptionPane.showMessageDialog(
                    this,
                    String.join("\n", messages),
                    "ERROR",
                    JOptionPane.ERROR_MESSAGE
            );
        } else {
            ok = true;
            dispose();
        }
    }
}
