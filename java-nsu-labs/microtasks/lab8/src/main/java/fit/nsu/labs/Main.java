package fit.nsu.labs;

import java.net.*;
import java.io.*;
import java.util.HashMap;
import java.util.Map;


public class Main {
    public static void main(String[] args) {
        var host = args[0];
        var resource = args[1];

        try (Socket socket = new Socket(host, 8080);) {
            PrintWriter out = new PrintWriter(socket.getOutputStream());
            out.println("GET " + resource + " HTTP/1.0\r\n");
            out.println("Host: " + resource + "\r\n\r\n");
            out.println("");
            out.flush();

            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line;
            boolean isRedirected = false;
            Map<String, String> headers = new HashMap<>();

            while ((line = in.readLine()) != null) {
                //System.out.println(line);
                if (line.contains("404 Not Found")) {
                    System.out.println("The requested resource was not found on the server.");
                    break;
                }

                if (line.contains("301 Moved Permanently")) {
                    //System.out.println("The requested resource was moved");
                    isRedirected = true;
                }
//


                String[] parts = line.split(": ");
                if(parts.length == 2){
                    String name = parts[0];
                    String value = parts[1];
                    headers.put(name, value);
                }



            }

            if(isRedirected){
                System.out.println("Site redirect to " + headers.get("Location"));
            }


        } catch (UnknownHostException e) {
            System.out.println("Host not found or not available: " + host);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

    }
}