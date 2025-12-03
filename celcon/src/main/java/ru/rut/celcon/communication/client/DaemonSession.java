package ru.rut.celcon.communication.client;

import tools.jackson.databind.ObjectMapper;import ru.rut.celcon.communication.client.websocket.WebSocketEventService;

import javax.net.ssl.SSLSocket;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;

public class DaemonSession implements Runnable {

    private final String sessionId;
    private final SSLSocket socket;
    private final ObjectMapper objectMapper;
    private final AtomicBoolean closed = new AtomicBoolean(false);

    private WebSocketEventService webSocketEventService; // отправка в браузер

    public DaemonSession(String sessionId, SSLSocket socket, ObjectMapper objectMapper) {
        this.sessionId = sessionId;
        this.socket = socket;
        this.objectMapper = objectMapper;
    }

    @Override
    public void run() {
        try (BufferedReader reader = new BufferedReader(
                new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8))) {

            String line;
            while (!closed.get() && (line = reader.readLine()) != null) {
                Map<String, Object> message = objectMapper.readValue(line, Map.class);

                if (message.containsKey("event")) {
                    // Это событие от демона — отправляем в WebSocket
                    webSocketEventService.sendEventToSession(sessionId, message);
                }
                // Ответы на команды обычно не приходят сюда (они синхронные),
                // но если демон шлёт асинхронные подтверждения — можно обработать
            }

        } catch (Exception e) {
            if (!closed.get()) {
                // Сессия разорвана — уведомить фронтенд
                webSocketEventService.sendEventToSession(sessionId, Map.of(
                        "event", "SESSION_BROKEN",
                        "session_id", sessionId,
                        "error", e.getMessage()
                ));
            }
        } finally {
            close();
        }
    }

    public void close() {
        if (closed.compareAndSet(false, true)) {
            try {
                socket.close();
            } catch (Exception ignored) {}
        }
    }

    public String getSessionId() {
        return sessionId;
    }
}