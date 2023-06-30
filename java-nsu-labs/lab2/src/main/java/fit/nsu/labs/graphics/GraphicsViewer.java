package fit.nsu.labs.graphics;

import fit.nsu.labs.model.Event;
import fit.nsu.labs.model.*;

import javax.swing.*;
import java.awt.*;
import java.util.Objects;

import static javax.swing.JOptionPane.showMessageDialog;


public class GraphicsViewer extends JFrame implements onEvent {

    private static final String CLOSED_ICON_NAME = "closed.png";
    private static final String FLAGGED_ICON_NAME = "flagged.png";
    private static final String BOMB_ICON_NAME = "bomb.png";
    private static final int BORDER_SIZE = 100;
    private static final int ICON_SIZE = 50;
    private final FieldElement[][] buttons;
    private final GraphicsController controller;
    private final InfoPanel panel;
    private final GameField model;


    public GraphicsViewer(GameField model) {
        this.model = model;
        controller = new GraphicsController(this.model);
        buttons = new FieldElement[model.getColumnSize()][model.getRowSize()];
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        int size = model.getColumnSize() * model.getRowSize();
        setSize(size, size);
        setLocationRelativeTo(null);
        setResizable(false);
        panel = new InfoPanel(100);
        add(panel, BorderLayout.PAGE_START);
        add(new FieldElementsGrid(model.getColumnSize(), model.getRowSize(), buttons));
        addActionListener(buttons);
        pack();
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(MenuFrame::new);
    }

    private void addActionListener(FieldElement[][] buttons) {
        for (JButton[] jButtons : buttons) {
            for (JButton jButton : jButtons) {
                jButton.addMouseListener(controller);
            }
        }
    }

    @Override
    public void notification(Event event) {
        if (event.type().equals(EventType.REDRAW_REQUEST)) {
            SwingUtilities.invokeLater(() -> reDraw(event));
            return;
        }

        if (event.type().equals(EventType.RECORDS_ERROR)) {
            showMessageDialog(null, "Error when writing records");
        }

        if (event.type().equals(EventType.USER_WIN)) {
            showMessageDialog(null, "You win!");
            return;
        }

        if (event.type().equals(EventType.BOMB_OPENED)) {
            showMessageDialog(null, "Bomb opened");
            return;
        }
        if (event.type().equals(EventType.ALREADY_CLICKED)) {
            //showMessageDialog(null, "Already opened", "Warning", JOptionPane.WARNING_MESSAGE);
            return;
        }


        if (event.type().equals(EventType.FLAG_STATE_UPDATE)) {
            SwingUtilities.invokeLater(() -> reDraw(event));
            return;
        }

        if (event.type().equals(EventType.REDRAW_TIMER)) {
            panel.setTimeElapsedTextField(event.field().getCurrentTimer());
            panel.setFlagsLeftTextField(event.field().getAvailableFlagsCounter());
        }

        if (event.type().equals(EventType.ALREADY_FLAGGED)) {
            showMessageDialog(null, "this coordinates already flagged", "Warning", JOptionPane.WARNING_MESSAGE);
        }
    }

    private void reDraw(Event event) {
        var cols = event.field().getColumnSize();
        var rows = event.field().getRowSize();
        var field = event.field();

        for (int i = 0; i < cols; i++) {
            for (int j = 0; j < rows; j++) {
                var el = field.getElementByCoords(new Dot(i, j));

                ImageIcon icon;
                var state = model.getState();
                var imageName = String.format("num%d.png", el.getBombsAroundCount());
                if (state.equals(GameField.GameState.GAME_OVER) || el.isOpened()) {
                    if (state.equals(GameField.GameState.GAME_OVER) && el.isBomb()) {
                        imageName = BOMB_ICON_NAME;
                        buttons[i][j].setEnabled(false);
                    }

                    if (state.equals(GameField.GameState.GAME_OVER) && !el.isOpened()) {
                        el.open();
                    }

                } else {

                    imageName = CLOSED_ICON_NAME;
                    if (el.isFlagged()) {
                        imageName = FLAGGED_ICON_NAME;
                    }
                }
                icon = new ImageIcon(Objects.requireNonNull(ClassLoader.getSystemResource(imageName)));
                buttons[i][j].setIcon(icon);
            }
        }
    }

    static class FieldElementsGrid extends JPanel {
        public FieldElementsGrid(int columnSize, int rowSize, FieldElement[][] buttons) {
            setLayout(new GridLayout(columnSize, rowSize, 3, 3));
            setBorder(BorderFactory.createEmptyBorder(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE));
            for (int row = 0; row < buttons.length; row++) {
                for (int col = 0; col < buttons[row].length; col++) {
                    FieldElement button = new FieldElement("", new Dot(col, row));
                    var icon = new ImageIcon(Objects.requireNonNull(ClassLoader.getSystemResource(CLOSED_ICON_NAME)));
                    button.setIcon(icon);
                    button.setPreferredSize(new Dimension(ICON_SIZE, ICON_SIZE));

                    add(button);

                    buttons[row][col] = button;
                }
            }

        }
    }
}