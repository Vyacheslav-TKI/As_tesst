package ru.rut.celcon.communication.client.websocket;

import org.springframework.stereotype.Service;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import tools.jackson.databind.ObjectMapper;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

@Service
public class WebSocketEventService {

    // session_id (демона) → WebSocketSession (браузера)
    private final Map<String, WebSocketSession> sessionMapping = new ConcurrentHashMap<>();

    public void registerSession(String daemonSessionId, WebSocketSession wsSession) {
        sessionMapping.put(daemonSessionId, wsSession);
    }

    public void unregisterSession(String daemonSessionId) {
        sessionMapping.remove(daemonSessionId);
    }

    public void sendEventToSession(String daemonSessionId, Map<String, Object> event) {
        WebSocketSession wsSession = sessionMapping.get(daemonSessionId);
        if (wsSession != null && wsSession.isOpen()) {
            try {
                String json = new ObjectMapper().writeValueAsString(event);
                wsSession.sendMessage(new TextMessage(json));
            } catch (Exception e) {
                // логирование ошибки
            }
        }
    }
}
