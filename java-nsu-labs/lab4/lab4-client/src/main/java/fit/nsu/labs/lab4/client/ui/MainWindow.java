package fit.nsu.labs.lab4.client.ui;

import fit.nsu.labs.lab4.client.Event;
import fit.nsu.labs.lab4.client.OnEvent;

import javax.swing.*;
import java.awt.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.List;
import java.util.function.Consumer;

import static fit.nsu.labs.lab4.client.Event.EventType.UPDATE_MEMBERS_AREA;
import static fit.nsu.labs.lab4.client.Event.EventType.UPDATE_MESSAGES_AREA;

public class MainWindow extends JFrame implements OnEvent {
    private final DefaultListModel<String> memberListModel = new DefaultListModel<>();
    private final JTextArea chatHistory;
    private Consumer<String> sendClickHandler;
    private Runnable closeWindowHandle;
    private Runnable requestUpdateMembers;

    public MainWindow(String userName) throws HeadlessException {
        setTitle("Chat App");
        setLayout(new BorderLayout());
        setSize(1000, 500);

        var labelUsername = new JLabel("User: " + userName, SwingConstants.CENTER);

        chatHistory = new JTextArea();
        chatHistory.setEditable(false);
        chatHistory.setLineWrap(true);
        var chatScrollPane = new JScrollPane(chatHistory);

        var inputPanel = new JPanel();
        inputPanel.setLayout(new BorderLayout());

        var chatInput = new JTextArea();
        chatInput.setLineWrap(true);
        var sendButton = new JButton("Send");
        sendButton.addActionListener(event -> {
            if (sendClickHandler != null) {
                sendClickHandler.accept(chatInput.getText());
            }
        });

        inputPanel.add(chatInput, BorderLayout.CENTER);
        inputPanel.add(sendButton, BorderLayout.EAST);

        var showMembersButton = new JButton("Show Members");
        showMembersButton.addActionListener(event -> {
            if (requestUpdateMembers != null) {
                requestUpdateMembers.run();
            }

            var memberWindow = new JFrame("Member List");
            memberWindow.setLayout(new BorderLayout());
            memberWindow.add(new JScrollPane(new JList<>(memberListModel)), BorderLayout.CENTER);
            memberWindow.pack();
            memberWindow.setLocationRelativeTo(this);
            memberWindow.setVisible(true);
        });

        add(labelUsername, BorderLayout.NORTH);
        add(chatScrollPane, BorderLayout.CENTER);
        add(inputPanel, BorderLayout.SOUTH);
        add(showMembersButton, BorderLayout.WEST);

        addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent e) {
                if (closeWindowHandle != null) {
                    closeWindowHandle.run();
                }
            }
        });
    }

    public void putMessage(String message) {
        chatHistory.setText(chatHistory.getText() + "\n" + message);
    }

    public void onSendClick(Consumer<String> handler) {
        sendClickHandler = handler;
    }

    public void onCloseWindow(Runnable handle) {
        closeWindowHandle = handle;
    }

    public void onUpdateMembersList(List<String> members) {
        memberListModel.clear();
        members.forEach(memberListModel::addElement);
    }

    public void setRequestUpdateMembers(Runnable runnable) {
        requestUpdateMembers = runnable;
    }

    @Override
    public void notification(Event event) {
        if (event.type().equals(UPDATE_MESSAGES_AREA)) {
            putMessage(event.data().get(0));
        }

        if (event.type().equals(UPDATE_MEMBERS_AREA)) {
            onUpdateMembersList(event.data());
        }
    }
}
