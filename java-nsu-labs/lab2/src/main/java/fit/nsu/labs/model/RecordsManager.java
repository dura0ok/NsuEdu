package fit.nsu.labs.model;

import fit.nsu.labs.exceptions.MineSweeperGameException;
import fit.nsu.labs.exceptions.RecordsWritingException;

import java.io.*;
import java.util.ArrayList;
import java.util.EnumMap;
import java.util.List;
import java.util.Map;

public class RecordsManager {
    private final Map<GameLevels, String> RECORD_FILES;

    public RecordsManager() {
        RECORD_FILES = new EnumMap<>(GameLevels.class) {{
            put(GameLevels.EASY, "easy.txt");
            put(GameLevels.MEDIUM, "medium.txt");
            put(GameLevels.HARD, "hard.txt");
            put(GameLevels.CUSTOM, "custom.txt");
        }};
    }


    public List<HighScore> readRecords(GameLevels level) throws MineSweeperGameException {
        List<HighScore> highScores = new ArrayList<>();
        try (var reader = new BufferedReader(new FileReader(RECORD_FILES.get(level)))) {
            String line = reader.readLine();
            while (line != null) {
                var splitted = line.split("\\s+");
                if (splitted.length != 2) {
                    line = reader.readLine();
                    continue;
                }

                String currentName = splitted[0];
                long currentTime = Long.parseLong(splitted[1]);

                highScores.add(new HighScore(currentName, currentTime));
                line = reader.readLine();
            }
        } catch (FileNotFoundException ignored) {
            return highScores;
        } catch (Exception e) {
            throw new MineSweeperGameException("error in read records", e);
        }
        return highScores;
    }

    public void writeRecord(GameLevels level, HighScore newHighScore) {

        try {
            var data = readRecords(level);
            data.add(newHighScore);
            data.sort((o1, o2) -> (int) (o1.secondsTime() - o2.secondsTime()));


            if (data.size() > 10) {
                data.remove(10);
            }


            try (var writer = new BufferedWriter(new FileWriter(RECORD_FILES.get(level), false))) {
                for (var item : data) {
                    writer.write(item.toString());
                }
            }


        } catch (Exception e) {
            throw new RecordsWritingException("records writing error");
        }


    }


}
