package ru.celkon.cli.client;

import javax.net.ssl.SSLSocket;
import java.io.*;

import ru.celkon.cli.dto.Envelope;

public class CelkonClient {

    private final TLSConnectionFactory tlsFactory = new TLSConnectionFactory();

    private final String host = "127.0.0.1";
    private final int port = 5555;   // порт демона — поменяй если другой


    public <T> T send(Envelope<?> request, Class<T> responseClass) {

        try (SSLSocket socket = tlsFactory.createSocket(host, port)) {

            // ---------- SEND REQUEST ----------
            String json = JsonMapper.get().writeValueAsString(request);

            BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
            writer.write(json);
            writer.write("\n");
            writer.flush();


            // ---------- READ RESPONSE ----------
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            String responseLine = reader.readLine();
            if (responseLine == null || responseLine.isEmpty()) {
                System.out.println("ERROR: empty response from server");
                return null;
            }

            return JsonMapper.get().readValue(responseLine, responseClass);

        } catch (Exception e) {
            System.out.println("Communication error: " + e.getMessage());
            return null;
        }
    }
}
