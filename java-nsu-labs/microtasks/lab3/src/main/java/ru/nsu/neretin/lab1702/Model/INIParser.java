package ru.nsu.neretin.lab1702.Model;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.regex.Pattern;

public class INIParser {
    private final BufferedReader file;

    public INIParser(BufferedReader stream){
        file = stream;
    }


    public HashMap<String, String> getVariables(){
        String line = null;
        var res = new HashMap<String, String>();
        try {
            line = file.readLine();
            while (line != null) {

                var splittedData = line.trim().split(Pattern.quote("="));
                if(splittedData.length == 2){
                    var key = splittedData[0];
                    var value = splittedData[1];

                    if(res.containsKey(key)){
                        throw new RuntimeException("key " + key + " already exists");
                    }
                    res.put(key, value);
                }

                // read next line
                line = file.readLine();
            }

            return res;
        }
        catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

}
