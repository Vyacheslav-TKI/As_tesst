package ru.rut.celcon.communication.client;

import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import ru.rut.celcon.services.WebSocketEventService;
import org.springframework.stereotype.Component;

@Component
public class WebSocketHandler extends TextWebSocketHandler {

    private final WebSocketEventService webSocketEventService;

    public WebSocketHandler(WebSocketEventService webSocketEventService) {
        this.webSocketEventService = webSocketEventService;
    }

    @Override
    public void afterConnectionEstablished(WebSocketSession session) throws Exception {
        webSocketEventService.registerBrowserSession(session);
        System.out.println("WebSocket connected: " + session.getId());
        session.sendMessage(new TextMessage("{\"status\":\"connected\"}"));
    }

    @Override
    public void afterConnectionClosed(WebSocketSession session, org.springframework.web.socket.CloseStatus status) throws Exception {
        webSocketEventService.unregisterBrowserSession(session);
        System.out.println("WebSocket disconnected: " + session.getId() + ", reason: " + status);
    }
}
