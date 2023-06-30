package fit.nsu.labs;

public class Utils {
    public static boolean isNotDouble(String string) {
        try {
            Double.parseDouble(string);
        } catch (NumberFormatException e) {
            return true;
        }
        return false;
    }
}
