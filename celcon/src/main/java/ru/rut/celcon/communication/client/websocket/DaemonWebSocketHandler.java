package ru.rut.celcon.communication.client.websocket;

import org.springframework.stereotype.Component;
import org.springframework.web.socket.*;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import org.springframework.web.util.UriComponentsBuilder;

import java.util.Map;

@Component
public class DaemonWebSocketHandler extends TextWebSocketHandler {

    private final WebSocketEventService eventService;

    public DaemonWebSocketHandler(WebSocketEventService eventService) {
        this.eventService = eventService;
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        // Ожидаем, что фронтенд передаст session_id в параметре: ws://.../ws?session_id=123
        Map<String, String> params = UriComponentsBuilder
                .fromUri(session.getUri())
                .build()
                .getQueryParams()
                .toSingleValueMap();

        String daemonSessionId = params.get("session_id");
        if (daemonSessionId != null) {
            eventService.registerSession(daemonSessionId, session);
        }
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
        // Найти и удалить привязку
        eventService.unregisterSession(getSessionIdFromSession(session));
    }

    private String getSessionIdFromSession(WebSocketSession session) {
        // Можно хранить в атрибутах при подключении
        Object id = session.getAttributes().get("daemonSessionId");
        return id == null ? null : id.toString();
    }
}