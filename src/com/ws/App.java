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

public class App {

    public static void main(String[] args) throws InterruptedException {
        ScheduledExecutorService pool = Executors.newScheduledThreadPool(2);
        while (true) {
            try {
                pool.schedule(App :: sendDataRequest, 10, TimeUnit.SECONDS);
            } catch (Exception e) {
                System.out.println(e.getMessage());
                throw e;
            }
            Thread.sleep(10000);
        }
    }

    private static void sendDataRequest()  {
        try {
            HttpClient client = HttpClient.newHttpClient();
            HttpRequest request = HttpRequest.newBuilder(new URI("http://192.168.1.51/")).build();
            HttpResponse<String> response = client.send(request, HttpResponse.BodyHandlers.ofString());
            if (response.statusCode() == 200) {
                String data = response.body();
                System.out.println(data);
                writeData(data);
            } else {
                System.out.println("Error when try get data");
            }
        } catch (URISyntaxException | InterruptedException | IOException e) {
            System.out.println(e.getMessage());
        }
    }

    private static void writeData(String data) throws IOException {
        BufferedWriter writer = new BufferedWriter(new FileWriter("data.txt", true));
        writer.append(data);
        writer.close();
    }
}
