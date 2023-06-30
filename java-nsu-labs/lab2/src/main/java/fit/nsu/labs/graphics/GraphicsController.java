package fit.nsu.labs.graphics;


import fit.nsu.labs.model.Dot;
import fit.nsu.labs.model.GameField;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class GraphicsController extends MouseAdapter {
    private final GameField model;


    public GraphicsController(GameField model) {
        this.model = model;
    }

    @Override
    public void mousePressed(MouseEvent e) {
        SwingUtilities.invokeLater(() -> {
            FieldElement sourceBtn = (FieldElement) e.getSource();
            var x = Integer.parseInt(String.valueOf(sourceBtn.getDot().x()));
            var y = Integer.parseInt(String.valueOf(sourceBtn.getDot().y()));
            var clickedDot = new Dot(x, y);

            if (e.getButton() == MouseEvent.BUTTON1) {
                model.click(clickedDot);
            }

            if (e.getButton() == MouseEvent.BUTTON3) {
                model.updateFlag(clickedDot);
            }
        });
    }
}

class FieldElement extends JButton {
    private final Dot dot;

    public FieldElement(String text, Dot dot) {
        super(text);
        this.dot = new Dot(dot.y(), dot.x());
    }

    @Override
    public Dimension getPreferredSize() {
        Dimension size = super.getPreferredSize();
        Icon icon = getIcon();
        if (icon != null) {
            size.width = Math.max(size.width, icon.getIconWidth());
            size.height = Math.max(size.height, icon.getIconHeight());
        }
        return size;
    }

    @Override
    public Dimension getMaximumSize() {
        return getPreferredSize();
    }

    @Override
    public Dimension getMinimumSize() {
        return getPreferredSize();
    }

    public Dot getDot() {
        return dot;
    }

}