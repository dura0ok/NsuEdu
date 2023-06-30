package fit.nsu.labs;

import fit.nsu.labs.console.ConsoleViewer;
import fit.nsu.labs.graphics.GraphicsViewer;

public class Main {
    public static void main(String[] args) {
        if (args.length == 0) {
            GraphicsViewer.main(new String[]{});
        }

        if (args.length == 1) {
            if (args[0].equalsIgnoreCase("--console")) {
                ConsoleViewer.main(new String[]{});
            } else {
                System.out.println("if you want to run in gui mode, you don`t need add arguments");
                System.out.println("but if you want to run console game version, add --console to arguments");
            }


        }
    }
}
