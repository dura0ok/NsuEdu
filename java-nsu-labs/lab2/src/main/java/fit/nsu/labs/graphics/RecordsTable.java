package fit.nsu.labs.graphics;

import fit.nsu.labs.model.GameLevels;
import fit.nsu.labs.model.HighScore;

import javax.swing.table.AbstractTableModel;
import java.util.List;

public class RecordsTable extends AbstractTableModel {
    private final List<HighScore> highScores;
    private final GameLevels level;

    RecordsTable(List<HighScore> highScores, GameLevels level) {
        this.highScores = highScores;
        this.level = level;
    }

    @Override
    public int getRowCount() {
        return highScores.size();
    }

    @Override
    public int getColumnCount() {
        return 3;
    }

    @Override
    public Object getValueAt(int rowIndex, int columnIndex) {
        HighScore rowHighScore = highScores.get(rowIndex);
        return switch (columnIndex) {
            case 0 -> rowHighScore.name();
            case 1 -> rowHighScore.secondsTime();
            case 2 -> level;
            default -> throw new IndexOutOfBoundsException("Invalid column index");
        };
    }

    @Override
    public String getColumnName(int column) {
        return switch (column) {
            case 0 -> "Name";
            case 1 -> "Time(seconds)";
            case 2 -> "Level";
            default -> throw new IndexOutOfBoundsException("Invalid column index");
        };
    }
}
