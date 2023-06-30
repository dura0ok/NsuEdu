package fit.nsu.labs.model;


import fit.nsu.labs.exceptions.RecordsWritingException;

import java.util.*;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;


public class GameField implements Observable {
    private static final Random randomService = new Random();
    private final BoardElement[][] board;
    private final Set<Dot> bombs = new HashSet<>();

    private final GameLevels level;
    private final int columnSize;
    private final int rowSize;

    private final int boardElementsCount;
    private final List<onEvent> onEvents = new ArrayList<>();
    private final String name;
    private final GameTime timer = new GameTime(this);
    private final GameSettings settings;
    private int availableFlagsCounter;
    private int openedFieldsCount = 0;
    private GameField.GameState state;
    private ScheduledExecutorService executor;
    private ScheduledFuture<?> future;


    public GameField(GameSettings settings, String name) {
        this.settings = settings;
        this.columnSize = settings.cols();
        this.rowSize = settings.rows();
        int bombsCounter = settings.bombsCount();
        this.availableFlagsCounter = settings.flagsCount();
        this.name = name;
        this.level = settings.level();
        boardElementsCount = columnSize * rowSize;
        validate(bombsCounter);

        board = new BoardElement[columnSize][rowSize];
        generateBombs(bombsCounter);
        initializeField();
        updateBombsAroundCount();
        state = GameState.GAME_OVER;
    }

    private static int getRandomNumber(int max) {
        return randomService.nextInt(max);
    }

    public static String getHelp() {
        return """
                The objective of the game is to clear a rectangular board containing hidden "mines" without detonating any of them. \n To help you, the board is divided into cells, and each cell contains a number indicating how many mines are adjacent to it.

                \n To play the game, you can click on a cell to reveal its contents. If the cell contains a mine, the game is over. \n If the cell contains a number, it indicates how many mines are adjacent to the cell. If the cell is blank, it means that none of the adjacent cells contain mines, so you can safely reveal all adjacent cells.

                To flag a cell that you suspect contains a mine, you can right-click on the cell. This will mark the cell with a flag, indicating that you think it contains a mine. \n You can remove a flag by right-clicking on the cell again.

                The game is won when all non-mine cells are revealed. Good luck!""";
    }

    public GameSettings getSettings() {
        return settings;
    }

    public long getCurrentTimer() {
        return timer.getElapsed();
    }

    public int getAvailableFlagsCounter() {
        return availableFlagsCounter;
    }

    public void startGame() {
        state = GameState.RUNNING;
        executor = Executors.newScheduledThreadPool(1);
        timer.reset();
        future = executor.scheduleWithFixedDelay(timer, 0, 1, TimeUnit.SECONDS);
        notifyObservers(new Event(EventType.REDRAW_REQUEST, this));
    }

    public GameState getState() {
        return state;
    }

    public int getColumnSize() {
        return columnSize;
    }

    public int getRowSize() {
        return rowSize;
    }

    public int getBombsCounter() {
        return bombs.size();
    }

    private void initializeField() {
        for (int i = 0; i < columnSize; i++) {
            for (int j = 0; j < rowSize; j++) {
                var currentDot = new Dot(i, j);
                if (bombs.contains(currentDot)) {
                    board[i][j] = new BoardElement(new Dot(i, j), BoardElement.BoardElementType.BOMB);
                    continue;
                }

                board[i][j] = new BoardElement(new Dot(i, j));
            }
        }
    }

    ArrayList<BoardElement> getNeighbours(Dot dot) {
        var res = new ArrayList<BoardElement>();
        var dotX = dot.x();
        var dotY = dot.y();
        try {
            for (int y = dotY - 1; y <= dotY + 1; y++) {
                for (int x = dotX - 1; x <= dotX + 1; x++) {
                    if (y < 0 || y >= rowSize) {
                        continue;
                    }
                    if (x < 0 || x >= columnSize) {
                        continue;
                    }
                    if (x == dotX && y == dotY) {
                        continue;
                    }
                    res.add(getElementByCoords(new Dot(x, y)));
                }
            }
        } catch (IndexOutOfBoundsException ignored) {
        }
        return res;
    }

    private void updateBombsAroundCount() {
        for (int i = 0; i < columnSize; i++) {
            for (int j = 0; j < rowSize; j++) {
                int localCounter = 0;
                var neighbours = getNeighbours(new Dot(i, j));
                for (var neighbour : neighbours) {
                    if (neighbour.isBomb()) {
                        localCounter += 1;
                    }
                }
                board[i][j].setBombsAroundCount(localCounter);
            }
        }
    }

    private void generateBombs(int bombsCounter) {
        while (getBombsCounter() != bombsCounter) {
            var element = generateRandomDot();
//            if (!bombs.contains(element)) {
//                System.out.println("Bomb coords " + element);
//            }
            bombs.add(element);
        }
    }

    private Dot generateRandomDot() {
        int x = getRandomNumber(columnSize);
        int y = getRandomNumber(rowSize);
        return new Dot(x, y);
    }

    public BoardElement getElementByCoords(Dot coords) {
        return board[coords.x()][coords.y()];
    }

    public void click(Dot dot) {

        if (dot.x() >= columnSize || dot.y() >= rowSize) {
            throw new IllegalArgumentException("Invalid clicked dot coords");
        }

        if (dot.x() < 0 || dot.y() < 0) {
            throw new IllegalArgumentException("Invalid clicked dot coords");
        }

        var el = getElementByCoords(dot);

        if (el.isFlagged()) {
            notifyObservers(new Event(EventType.ALREADY_FLAGGED, this));
            return;
        }

        if (isOpened(dot)) {
            var neighbours = getNeighbours(dot);
            int flaggedAround = 0;
            for(var neighbour: neighbours){
                if(neighbour.isFlagged()){
                    flaggedAround++;
                }
            }

            if(flaggedAround == getElementByCoords(dot).getBombsAroundCount()){
                for(var neighbour: neighbours){
                    var currentDot = neighbour.getBoardCoords();

                    if(getElementByCoords(currentDot).isFlagged()){
                        continue;
                    }

                    if (handleDotBomb(currentDot)){
                        return;
                    }
                    getElementByCoords(neighbour.getBoardCoords()).open();
                }
                notifyObservers(new Event(EventType.REDRAW_REQUEST, this));
                return;
            }

            notifyObservers(new Event(EventType.ALREADY_CLICKED, this));
        }

        if (handleDotBomb(dot)){
            return;
        }

        openElement(dot);
        checkWinState();
        notifyObservers(new Event(EventType.REDRAW_REQUEST, this));
    }

    private boolean handleDotBomb(Dot dot) {
        if (isDotBomb(dot)) {
            //System.out.println("throw bomb opened");
            notifyObservers(new Event(EventType.BOMB_OPENED, this));
            state = GameState.GAME_OVER;
            notifyObservers(new Event(EventType.REDRAW_REQUEST, this));

            future.cancel(true);
            executor.shutdownNow();
            return true;
        }
        return false;
    }

    private void openElement(Dot dot) {
        if (isOpened(dot) || isDotBomb(dot)) {
            return;
        }

        getElementByCoords(dot).open();
        openedFieldsCount++;
        openAroundArea(dot);
    }

    private void openAroundArea(Dot dot) {
        if (getElementByCoords(dot).getBombsAroundCount() == 0) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    var newPoint = new Dot(dot.x() + i, dot.y() + j);
                    if (newPoint.x() < 0 || newPoint.x() >= columnSize) {
                        continue;
                    }

                    if (newPoint.y() < 0 || newPoint.y() >= rowSize) {
                        continue;
                    }

                    if (getElementByCoords(newPoint).isOpened()) {
                        continue;
                    }

                    if (!getElementByCoords(newPoint).isOpened()) {
                        openElement(newPoint);
                    }
                }
            }
        }
    }

    public boolean isOpened(Dot dot) {
        return getElementByCoords(dot).isOpened();
    }

    public boolean isDotBomb(Dot dot) {
        return getElementByCoords(dot).isBomb();
    }

    @Override
    public void registerObserver(onEvent o) {
        onEvents.add(o);
    }

    public void notifyObservers(Event event) {
        for (onEvent onEvent : onEvents) {
            onEvent.notification(event);
        }
    }

    private void checkWinState() {
        if (boardElementsCount - getBombsCounter() == openedFieldsCount) {
            state = GameState.GAME_OVER;
            notifyObservers(new Event(EventType.USER_WIN, this));
            var recordsManager = new RecordsManager();

            try {
                recordsManager.writeRecord(level, new HighScore(name, timer.getElapsed()));
            } catch (RecordsWritingException ignored) {
                notifyObservers(new Event(EventType.RECORDS_ERROR, this));
            }


            System.out.println("FINISHING " + timer.getElapsed());
            future.cancel(true);
            executor.shutdownNow();
        }
    }

    private void validate(int bombsCount) {
        if (0 >= bombsCount) {
            throw new IllegalArgumentException("Bombs counter must be positive");
        }

        if (bombsCount > boardElementsCount) {
            throw new IllegalArgumentException("bombs counter must be less-equal field size");
        }
    }

    public void updateFlag(Dot clickedDot) {
        var el = getElementByCoords(clickedDot);
        if (availableFlagsCounter <= 0 && !el.isFlagged()) {
            return;
        }


        if (el.isFlagged()) {
            availableFlagsCounter++;
        } else {
            availableFlagsCounter--;
        }

        el.updateFlagged();
        notifyObservers(new Event(EventType.REDRAW_REQUEST, this));
    }

    public long getElapsed() {
        return timer.getElapsed();
    }

    public enum GameState {
        RUNNING,
        GAME_OVER,
    }

}


