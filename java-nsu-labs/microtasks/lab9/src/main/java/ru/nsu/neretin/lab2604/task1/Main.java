package ru.nsu.neretin.lab2604.task1;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class Main {

    public static void main(String[] args) {
        try {
            URL url = new URL("https://habr.com/ru/rss/best/daily/");
            InputStream in = url.openStream();
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = factory.newDocumentBuilder();
            Document doc = db.parse(in);
            NodeList items = doc.getElementsByTagName("item");
            System.out.println(items);
            for (int i = 0; i < items.getLength(); i++) {
                Element item = (Element) items.item(i);
                String title = item.getElementsByTagName("title").item(0).getTextContent();
                String link = item.getElementsByTagName("link").item(0).getTextContent();
                String author = item.getElementsByTagName("dc:creator").item(0).getTextContent();
                System.out.printf("Title: %s Link: %s Author %s%n", title, link, author);
            }

        } catch (IOException | ParserConfigurationException | SAXException e) {
            e.printStackTrace();
        }
    }
}