package fit.nsu.labs.model;

public class BoardElement {
    private final Dot boardCoords;
    private final BoardElementType type;
    private boolean isOpened;

    private boolean isFlagged;
    private int bombsAroundCount;

    public BoardElement(Dot boardCoords, BoardElementType type) {
        this.type = type;
        this.boardCoords = boardCoords;
        isOpened = false;
        isFlagged = false;
    }

    public BoardElement(Dot boardCoords) {
        this(boardCoords, BoardElementType.REGULAR_FIELD);
    }

    public boolean isBomb() {
        return type == BoardElementType.BOMB;
    }

    public boolean isOpened() {
        return isOpened;
    }

    public Dot getBoardCoords() {
        return boardCoords;
    }

    public int getBombsAroundCount() {
        return bombsAroundCount;
    }

    public void setBombsAroundCount(int bombsAroundCount) {
        this.bombsAroundCount = bombsAroundCount;
    }

    public void open() {
        isOpened = true;
    }

    public boolean isFlagged() {
        return isFlagged;
    }

    public void updateFlagged() {
        isFlagged = !isFlagged;
    }

    enum BoardElementType {
        BOMB,
        REGULAR_FIELD
    }

}
