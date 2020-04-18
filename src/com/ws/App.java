package com.ws;


import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.logging.Logger;

public class App implements Runnable{

    private static String serverUrl = "http://192.168.1.51/";

    public static final Logger log = Logger.getLogger("logger");

    @Override
    public void run() {
        sendDataRequest();
    }

    public static void main(String[] args) {
        log.info("Start app!");
        ScheduledExecutorService scheduler = Executors.newSingleThreadScheduledExecutor();
        Runnable task = new App();
        int delay = 10;
        scheduler.scheduleAtFixedRate(task, delay, delay, TimeUnit.SECONDS);
    }

    private static void sendDataRequest()  {
        try {
            HttpClient client = HttpClient.newHttpClient();
            HttpRequest request = HttpRequest.newBuilder(new URI(serverUrl)).build();
            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() == 200) {
                String data = response.body();
                log.fine("Get data from sensor " + data);
                writeData(data);
            } else {
                log.severe("Error when try get data");
            }
        } catch (URISyntaxException | InterruptedException | IOException e) {
            log.severe(e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private static void writeData(String data) throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter("data.txt", true));
        writer.append(data);
        writer.close();
    }
}
