package ru.rut.celcon.services;

import org.springframework.stereotype.Service;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import tools.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArraySet;

@Service
public class WebSocketEventService {
    private final ObjectMapper objectMapper;
    private final CopyOnWriteArraySet<WebSocketSession> sessions = new CopyOnWriteArraySet<>();
    private final Queue<Map<String, Object>> eventBuffer = new ConcurrentLinkedQueue<>();
    private final int MAX_BUFFER_SIZE = 100;

    public WebSocketEventService(ObjectMapper objectMapper) {
        this.objectMapper = objectMapper;
    }

    public void registerBrowserSession(WebSocketSession session) {
        sessions.add(session);
        System.out.println("WebSocket session registered. Total sessions: " + sessions.size());

        // Отправляем все буферизованные события при подключении
        if (!eventBuffer.isEmpty()) {
            System.out.println("Sending " + eventBuffer.size() + " buffered events to new session");
            sendBufferedEvents(session);
        }
    }

    private void sendBufferedEvents(WebSocketSession session) {
        while (!eventBuffer.isEmpty()) {
            Map<String, Object> event = eventBuffer.poll();
            if (event != null && session.isOpen()) {
                try {
                    String json = objectMapper.writeValueAsString(event);
                    session.sendMessage(new TextMessage(json));
                    System.out.println("Sent buffered event: " + event.get("event"));
                } catch (Exception e) {
                    System.err.println("Failed to send buffered event: " + e.getMessage());
                }
            }
        }
    }

    public void sendEventToBrowser(Map<String, Object> event) {
        if (sessions.isEmpty()) {
            // Буферизуем событие, если нет подключенных сессий
            if (eventBuffer.size() < MAX_BUFFER_SIZE) {
                eventBuffer.offer(new HashMap<>(event)); // Копируем, чтобы избежать изменений
                System.out.println("Buffered event (no sessions): " + event.get("event"));
            } else {
                System.out.println("Event buffer full, dropping: " + event.get("event"));
            }
            return;
        }

        try {
            String json = objectMapper.writeValueAsString(event);
            System.out.println("Broadcasting event to " + sessions.size() + " session(s): " + json);

            // Удаляем закрытые сессии
            sessions.removeIf(session -> !session.isOpen());

            for (WebSocketSession session : sessions) {
                if (session.isOpen()) {
                    try {
                        session.sendMessage(new TextMessage(json));
                        System.out.println("Event sent to session: " + session.getId());
                    } catch (IOException e) {
                        System.err.println("Failed to send to session " + session.getId() + ": " + e.getMessage());
                        sessions.remove(session);
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("Failed to serialize/send event: " + e.getMessage());
        }
    }

    // ... остальные методы

    /**
     * Удаляет WebSocket-сессию
     */
    public void unregisterBrowserSession(WebSocketSession session) {
        sessions.remove(session);
        System.out.println("WebSocket session unregistered. Total sessions: " + sessions.size());
    }

    /**
     * Отправляет событие во ВСЕ подключенные браузеры
     */
    public void sendEventToBrowser(Object eventObject) {
        if (sessions.isEmpty()) {
            System.out.println("No WebSocket sessions to send event: " + eventObject);
            return;
        }

        try {
            String json = objectMapper.writeValueAsString(eventObject);
            System.out.println("Broadcasting event to " + sessions.size() + " session(s): " + json);

            sessions.removeIf(session -> !session.isOpen()); // Удаляем закрытые сессии

            for (WebSocketSession session : sessions) {
                if (session.isOpen()) {
                    try {
                        session.sendMessage(new TextMessage(json));
                        System.out.println("Event sent to session: " + session.getId());
                    } catch (IOException e) {
                        System.err.println("Failed to send to session " + session.getId() + ": " + e.getMessage());
                    }
                }
            }
        } catch (Exception e) {
            System.err.println("Failed to serialize/send event: " + e.getMessage());
        }
    }
}