package ru.nsu.neretin.lab1503.views;

import ru.nsu.neretin.lab1503.Event;
import ru.nsu.neretin.lab1503.EventType;
import ru.nsu.neretin.lab1503.Observer;
import ru.nsu.neretin.lab1503.SwingController;
import ru.nsu.neretin.lab1503.models.SumNumberGuesser;

import javax.swing.*;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;
import java.awt.*;

import static javax.swing.JOptionPane.showMessageDialog;

public class SwingView extends JFrame implements Observer {
    private final SumNumberGuesser model;
    private final JLabel questionTextModel;
    private final JButton btn;

    public SwingView(SumNumberGuesser model) {
        this.model = model;
        setLayout(new FlowLayout());

        Document textModel = new PlainDocument();


        questionTextModel = new JLabel();



        JTextField textField = new JTextField(10);
        textField.setDocument(textModel);



        SwingController controller = new SwingController(model, textModel);

        btn = new JButton();
        btn.setText("Click!");
        btn.addActionListener(controller);
        add(btn);


        add(textField);
        add(questionTextModel);
        pack();
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLocationRelativeTo(null);
    }


    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            var model = new SumNumberGuesser();
            SwingView w = new SwingView(model);
            model.registerView(w);
            model.start();
            w.setVisible(true);
            w.setSize(500, 500);

        });
    }

    @Override
    public void notification(Event event) {
        if (event.type().equals(EventType.CORRECT_ANSWER)) {
            showMessageDialog(null, "Answer is correct. Your scores: " + event.data());
        }
        if (event.type().equals(EventType.WRONG_ANSWER)) {
            showMessageDialog(null, "Wrong answer");
            return;
        }
        if (event.type().equals(EventType.NEW_QUESTION)) {
            questionTextModel.setText(event.data());
        }

        if (event.type().equals(EventType.LATE)) {
            showMessageDialog(null, "LATE");
            return;
        }
    }
}
