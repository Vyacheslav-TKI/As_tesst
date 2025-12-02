package ru.celkon.cli.client;

import com.fasterxml.jackson.databind.json.JsonMapper;
import ru.celkon.cli.dto.Envelope;

import javax.net.ssl.SSLSocket;
import java.io.*;

public class CelkonClient implements AutoCloseable {

    private static final JsonMapper jsonMapper = new JsonMapper();

    private final SSLSocket socket;
    private final BufferedReader reader;
    private final OutputStreamWriter writer;

    public CelkonClient(String host, int port) {
        try {
            TLSConnectionFactory tlsFactory = new TLSConnectionFactory();
            this.socket = tlsFactory.createSocket(host, port);

            // Устанавливаем таймаут на чтение (например, 10 секунд)
            socket.setSoTimeout(10_000);

            this.reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            this.writer = new OutputStreamWriter(socket.getOutputStream());
        } catch (Exception e) {
            throw new RuntimeException("Failed to connect to " + host + ":" + port, e);
        }
    }

    public CelkonClient() {
        this("127.0.0.1", 9999);
    }

    public <T> T send(Envelope<?> request, Class<T> responseType) {
        try {
            String json = jsonMapper.writeValueAsString(request);

            // Отправляем запрос
            writer.write(json);
            writer.write('\n');
            writer.flush();

            // Читаем ОДНУ строку ответа
            String responseLine = reader.readLine();
            if (responseLine == null) {
                throw new IOException("Server closed connection (received null)");
            }

            return jsonMapper.readValue(responseLine, responseType);

        } catch (IOException e) {
            // Чёткое сообщение при таймауте или разрыве
            throw new RuntimeException("Failed to read response (server did not reply in time or closed connection)", e);
        } catch (Exception e) {
            throw new RuntimeException("Serialization/deserialization error", e);
        }
    }

    @Override
    public void close() throws IOException {
        try { writer.close(); } catch (IOException ignored) {}
        try { reader.close(); } catch (IOException ignored) {}
        socket.close();
    }
}