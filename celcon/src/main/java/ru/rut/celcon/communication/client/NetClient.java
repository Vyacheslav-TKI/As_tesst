package ru.rut.celcon.communication.client;
import ru.rut.celcon.FileInfo;
import ru.rut.celcon.entities.User;
import tools.jackson.databind.ObjectMapper;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;
import ru.rut.celcon.communication.client.websocket.WebSocketEventService;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import java.io.*;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Component
public class NetClient {

    private final SSLSocketFactory socketFactory;
    private final ObjectMapper objectMapper;
    private final WebSocketEventService webSocketEventService; // ← внедряем один раз
    private final ExecutorService eventExecutor = Executors.newCachedThreadPool();
    private final Map<String, SessionConnection> activeConnections;

    @Value("${daemon.host:localhost}")
    private String daemonHost;

    @Value("${daemon.port:9999}")
    private int daemonPort;

    // Хранение активных сессий: session_id → сессионный контекст
    private final Map<String, DaemonSession> activeSessions = new ConcurrentHashMap<>();

    public NetClient(SSLContext sslContext, ObjectMapper objectMapper, WebSocketEventService webSocketEventService) {
        this.socketFactory = sslContext.getSocketFactory();
        this.objectMapper = objectMapper;
        this.webSocketEventService = webSocketEventService;
        activeConnections = new HashMap<>();
    }

    public int getDaemonPort() {
        return daemonPort;
    }

    public String getDaemonHost() {
        return daemonHost;
    }

    public SSLSocketFactory getSocketFactory() {
        return socketFactory;
    }

    public User auth(String login, String token) {
        Map<String, Object> command = Map.of("cmd", "AUTH", "login", login, "token", token);
        try (SSLSocket socket = (SSLSocket) socketFactory.createSocket()) {
            socket.connect(new InetSocketAddress(daemonHost, daemonPort), 5000); // 5 сек на подключение
            socket.setSoTimeout(10000); // 10 сек на чтение ответа

            socket.startHandshake();

            try (BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8));
                 BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8))) {

                String jsonRequest = objectMapper.writeValueAsString(command);
                writer.write(jsonRequest);
                writer.newLine();
                writer.flush();

                String line = reader.readLine(); // ← может зависнуть без setSoTimeout()
                if (line == null) {
                    throw new RuntimeException("Daemon closed connection unexpectedly");
                }
                Map<String, Object> response = objectMapper.readValue(line, Map.class);

                if (response == null) throw new RuntimeException("No response from daemon");

                String sessionId = null;
                String fio = null;
                String post = null;
                int role = -1;
                if (Integer.valueOf(200).equals(response.get("code"))) {
                    sessionId = (String) response.get("session_id");
                    fio = (String) response.get("fio");
                    post = (String) response.get("post");
                    role = (int) response.get("role");
                } else {
                    throw new RuntimeException((String)response.get("answ"));
                }
                registerSession(sessionId, socket, writer, reader);

                return new User(fio, post, role, sessionId);
            }
        } catch (Exception e) {
            throw new RuntimeException("Daemon communication failed", e);
        }
    }

    public List<FileInfo> sync(String sessionId) {
        // Формируем команду
        Map<String, Object> command = Map.of("cmd", "SYNC", "session_id", sessionId);

        // Отправляем команду через существующее соединение
        Map<String, Object> response = sendCommand(sessionId, command);

        // Проверяем код ответа
        Integer code = (Integer) response.get("code");
        if (code == null || code != 200) {
            String errorMsg = (String) response.getOrDefault("answ", "Unknown error");
            throw new RuntimeException("SYNC failed: " + errorMsg);
        }

        // Извлекаем список файлов
        List<Map<String, Object>> fileList = (List<Map<String, Object>>) response.get("files");
        if (fileList == null) {
            return List.of(); // или new ArrayList<>()
        }

        // Преобразуем в FileInfo
        return fileList.stream()
                .map(fileObj -> {
                    String path = (String) fileObj.get("path");
                    String name = extractFileName(path);
                    // changed пока неизвестен — можно добавить позже через STAT или события
                    return new FileInfo(name, path, false);
                })
                .toList();
    }

    // Вспомогательный метод для извлечения имени файла из пути
    private String extractFileName(String path) {
        if (path == null || path.isEmpty()) return "unknown";
        int lastSlash = path.lastIndexOf('/');
        return lastSlash >= 0 ? path.substring(lastSlash + 1) : path;
    }

    public void registerSession(String sessionId, SSLSocket socket, BufferedWriter writer, BufferedReader reader) {
        activeConnections.put(sessionId, new SessionConnection(socket, writer, reader));
    }

    public Map<String, Object> sendCommand(String sessionId, Map<String, Object> command) {
        SessionConnection conn = activeConnections.get(sessionId);
        if (conn == null) {
            throw new RuntimeException("Session not found: " + sessionId);
        }

        try {
            String json = objectMapper.writeValueAsString(command);
            conn.getWriter().write(json);
            conn.getWriter().newLine();
            conn.getWriter().flush();

            String responseLine = conn.getReader().readLine();
            if (responseLine == null) {
                throw new RuntimeException("Daemon closed session");
            }
            return objectMapper.readValue(responseLine, Map.class);
        } catch (Exception e) {
            // Закрыть сессию при ошибке
            activeConnections.remove(sessionId);
            try { conn.getSocket().close(); } catch (Exception ignored) {}
            throw new RuntimeException("Command failed", e);
        }
    }

    // Установка сессии: вызывается после успешного AUTH
    public void startSessionListening(String sessionId, SSLSocket socket) {
        DaemonSession session = new DaemonSession(sessionId, socket, objectMapper);
        activeSessions.put(sessionId, session);
        eventExecutor.submit(session);
    }

    // Получить сессию по ID (для отправки последующих команд)
    public DaemonSession getSession(String sessionId) {
        return activeSessions.get(sessionId);
    }

    public void removeSession(String sessionId) {
        DaemonSession session = activeSessions.remove(sessionId);
        if (session != null) {
            session.close();
        }
    }
}