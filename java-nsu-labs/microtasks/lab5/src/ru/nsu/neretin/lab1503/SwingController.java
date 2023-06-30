package ru.nsu.neretin.lab1503;

import ru.nsu.neretin.lab1503.models.SumNumberGuesser;

import javax.print.Doc;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class SwingController implements ActionListener {
    private final SumNumberGuesser model;
    private final Document inputModel;

    public SwingController(SumNumberGuesser model, Document inputModel) {
        this.model = model;
        this.inputModel = inputModel;

    }

    @Override
    public void actionPerformed(ActionEvent e) {
        try {
            var input = inputModel.getText(0, inputModel.getLength());
            var numInput = Integer.parseInt(input);
            model.makeGuess(numInput);
        } catch (BadLocationException | NumberFormatException ex) {
            ex.printStackTrace();
        }
    }
}
