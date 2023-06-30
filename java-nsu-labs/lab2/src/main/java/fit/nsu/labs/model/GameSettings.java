package fit.nsu.labs.model;

public record GameSettings(int cols, int rows, int bombsCount, int flagsCount, GameLevels level) {
    public static GameSettings getEasyLevel() {
        return new GameSettings(2, 2, 1, 1, GameLevels.EASY);
    }

    public static GameSettings getMediumLevel() {
        return new GameSettings(5, 5, 4, 4, GameLevels.MEDIUM);
    }

    public static GameSettings getHardLevel() {
        return new GameSettings(10, 10, 20, 20, GameLevels.HARD);
    }
}
