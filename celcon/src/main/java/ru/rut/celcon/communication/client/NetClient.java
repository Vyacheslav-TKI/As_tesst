package ru.rut.celcon.communication.client;

import org.springframework.web.socket.WebSocketSession;
import ru.rut.celcon.entities.*;
import ru.rut.celcon.services.WebSocketEventService;
import tools.jackson.databind.ObjectMapper;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Component;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import java.io.*;
import java.net.InetSocketAddress;
import java.nio.charset.StandardCharsets;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

@Component
public class NetClient implements AutoCloseable {

    private final SSLSocketFactory socketFactory;
    private final ObjectMapper objectMapper;
    private final WebSocketEventService webSocketEventService;

    // Синхронизация доступа к сокету
    private final Object writeLock = new Object();
    private final AtomicBoolean expectingResponse = new AtomicBoolean(false);
    private final BlockingQueue<Map<String, Object>> commandResponseQueue = new LinkedBlockingQueue<>();

    private volatile boolean running = true;
    private SessionConnection sessionConnection;
    private Thread eventReaderThread;

    @Value("${daemon.host:localhost}")
    private String daemonHost = "localhost";

    @Value("${daemon.port:9999}")
    private int daemonPort = 9999;

    public NetClient(SSLContext sslContext, ObjectMapper objectMapper, WebSocketEventService webSocketEventService) {
        this.socketFactory = sslContext.getSocketFactory();
        this.objectMapper = objectMapper;
        this.webSocketEventService = webSocketEventService;
        initializeConnection();
    }

    private synchronized void initializeConnection() {
        try {
            SSLSocket socket = (SSLSocket) socketFactory.createSocket();
            socket.connect(new InetSocketAddress(daemonHost, daemonPort), 10000);
            socket.setSoTimeout(0); // Бесконечный таймаут

            socket.startHandshake();

            BufferedWriter writer = new BufferedWriter(
                    new OutputStreamWriter(socket.getOutputStream(), StandardCharsets.UTF_8)
            );
            BufferedReader reader = new BufferedReader(
                    new InputStreamReader(socket.getInputStream(), StandardCharsets.UTF_8)
            );

            this.sessionConnection = new SessionConnection(socket, writer, reader);

            this.eventReaderThread = new Thread(this::readEvents, "DaemonEventReader");
            this.eventReaderThread.setDaemon(true);
            this.eventReaderThread.start();

        } catch (Exception e) {
            throw new RuntimeException("Failed to initialize daemon connection", e);
        }
    }

    private void readEvents() {
        try {
            String line;
            while (running && (line = sessionConnection.getReader().readLine()) != null) {
                try {
                    Map<String, Object> message = objectMapper.readValue(line, Map.class);

                    if (message.containsKey("event")) {
                        handleEvent(message);
                    } else if (message.containsKey("code")) {
                        // Это ответ на команду - кладем в очередь
                        if (expectingResponse.get()) {
                            commandResponseQueue.offer(message);
                        } else {
                            // Неожиданный ответ - логируем ошибку
                            System.err.println("Unexpected response from daemon: " + message);
                        }
                    } else {
                        System.err.println("Unknown message format: " + message);
                    }
                } catch (Exception e) {
                    System.err.println("Error parsing message from daemon: " + e.getMessage());
                }
            }
        } catch (IOException e) {
            if (running) {
                System.err.println("Connection lost: " + e.getMessage());
                handleConnectionLost();
            }
        }
    }

    private void handleConnectionLost() {
        // Оповещаем об обрыве соединения
        commandResponseQueue.offer(Map.of("code", 520, "answ", "connection closed"));
        running = false;
    }

    private void handleEvent(Map<String, Object> event) {
        System.out.println("[NetClient] Received event from daemon: " + event);

        // Обработка событий в отдельном потоке, чтобы не блокировать чтение
        CompletableFuture.runAsync(() -> {
            String eventId = (String) event.get("event");
            System.out.println("[NetClient] Processing event: " + eventId);

            switch (eventId) {
                case "FILE_CHANGED":
                case "SESSION_EXPIRED":
                    try {
                        webSocketEventService.sendEventToBrowser(event);
                        System.out.println("[NetClient] Event forwarded to WebSocketService: " + eventId);
                    } catch (Exception e) {
                        System.err.println("[NetClient] Failed to forward event: " + e.getMessage());
                    }
                    break;
                default:
                    System.err.println("[NetClient] Unknown event type: " + eventId);
            }
        }).exceptionally(e -> {
            System.err.println("[NetClient] Error in event handling: " + e.getMessage());
            return null;
        });
    }

    public User auth(String login, String token) {
        Map<String, Object> command = Map.of(
                "cmd", "AUTH",
                "login", login,
                "token", token
        );

        return executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null) {
                throw new RuntimeException("Invalid response from daemon");
            }

            if (code == 200) {
                String sessionId = (String) response.get("session_id");
                String fio = (String) response.get("fio");
                String post = (String) response.get("post");
                Integer role = (Integer) response.get("role");
                return new User(fio, post, role != null ? role : -1, sessionId);
            } else {
                String errorMsg = (String) response.get("answ");
                throw new RuntimeException(errorMsg != null ? errorMsg : "Authentication failed");
            }
        });
    }

    public void addFiles(String sessionId, List<FileToAdd> filesToAdd) {
        List<Map<String, Object>> filesJson = new ArrayList<>();
        for (FileToAdd file : filesToAdd) {
            filesJson.add(Map.of(
                    "path", file.getPath(),
                    "alg", file.getAlg(),
                    "hash", file.getHash(),
                    "for_users", file.getForUsers()
            ));
        }

        Map<String, Object> command = Map.of(
                "cmd", "ADD_FILES",
                "session_id", sessionId,
                "files", filesJson
        );

        executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null || code != 200) {
                String msg = (String) response.getOrDefault("answ", "Unknown error");
                throw new RuntimeException("ADD_FILES failed: " + msg);
            }
            return null;
        });
    }

    public void addUser(String sessionId, UserToAdd user) {
        Map<String, Object> command = Map.of(
                "cmd", "ADD_USER",
                "session_id", sessionId,
                "fio", user.getFio(),
                "post", user.getPost(),
                "level", user.getRole(),
                "login", user.getLogin(),
                "token", user.getPassword()
        );

        executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null || code != 200) {
                String msg = (String) response.getOrDefault("answ", "Unknown error");
                throw new RuntimeException("ADD_FILES failed: " + msg);
            }
            return null;
        });
    }

    public List<FileInfo> sync(String sessionId) {
        Map<String, Object> command = Map.of(
                "cmd", "SYNC",
                "session_id", sessionId
        );

        return executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null || code != 200) {
                String errorMsg = (String) response.getOrDefault("answ", "Unknown error");
                throw new RuntimeException("SYNC failed: " + errorMsg);
            }

            List<Map<String, Object>> fileList = (List<Map<String, Object>>) response.get("files");
            if (fileList == null) {
                return List.of();
            }

            return fileList.stream()
                    .map(fileObj -> {
                        int id = (Integer)fileObj.get("id");
                        String path = (String) fileObj.get("path");
                        String name = extractFileName(path);
                        return new FileInfo(id, name, path, false);
                    })
                    .toList();
        });
    }

    public List<UserInfo> listUsers(String sessionId) {
        Map<String, Object> command = Map.of(
                "cmd", "LIST_USERS",
                "session_id", sessionId
        );

        return executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null || code != 200) {
                String errorMsg = (String) response.getOrDefault("answ", "Unknown error");
                throw new RuntimeException("LIST_USERS failed: " + errorMsg);
            }

            List<Map<String, Object>> usersList = (List<Map<String, Object>>) response.get("users");
            if (usersList == null) {
                return List.of();
            }

            List<UserInfo> users = new ArrayList<>();
            for (Map<String, Object> u : usersList) {
                int id = (Integer) u.get("id");
                String fio = (String) u.get("fio");
                String post = (String) u.get("post");
                int role = (Integer) u.get("role");
                users.add(new UserInfo(id, fio, post, role));
            }
            return users;
        });
    }

    public void logout(String sessionId) {
        Map<String, Object> command = Map.of(
                "cmd", "LOGOUT",
                "session_id", sessionId
        );

        executeCommand(command, response -> {
            Integer code = (Integer) response.get("code");
            if (code == null || code != 200) {
                String errorMsg = (String) response.getOrDefault("answ", "Unknown error");
                throw new RuntimeException("LOGOUT failed: " + errorMsg);
            }
            return null;
        });
    }

    private <T> T executeCommand(Map<String, Object> command, ResponseHandler<T> handler) {
        if (sessionConnection == null || !running) {
            throw new RuntimeException("Connection to daemon is closed");
        }

        // Гарантируем, что только одна команда выполняется в момент времени
        synchronized (writeLock) {
            expectingResponse.set(true);
            commandResponseQueue.clear(); // Очищаем старые ответы

            try {
                // Отправляем команду
                String json = objectMapper.writeValueAsString(command);
                synchronized (sessionConnection.getWriter()) {
                    sessionConnection.getWriter().write(json);
                    sessionConnection.getWriter().newLine();
                    sessionConnection.getWriter().flush();
                }

                // Ждем ответа с таймаутом
                Map<String, Object> response = commandResponseQueue.poll(10, TimeUnit.SECONDS);

                if (response == null) {
                    throw new RuntimeException("Timeout waiting for response to command: " + command.get("cmd"));
                }

                // Проверяем, что это не сообщение об ошибке соединения
                Integer code = (Integer) response.get("code");
                if (code != null && code == 520) {
                    throw new RuntimeException("Connection to daemon lost");
                }

                return handler.handle(response);

            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                throw new RuntimeException("Command interrupted: " + command.get("cmd"), e);
            } catch (Exception e) {
                if (e instanceof RuntimeException) {
                    throw (RuntimeException) e;
                }
                throw new RuntimeException("Failed to execute command: " + command.get("cmd"), e);
            } finally {
                expectingResponse.set(false);
            }
        }
    }

    @FunctionalInterface
    private interface ResponseHandler<T> {
        T handle(Map<String, Object> response);
    }

    private String extractFileName(String path) {
        if (path == null || path.isEmpty()) return "unknown";
        int lastSlash = path.lastIndexOf('/');
        return lastSlash >= 0 ? path.substring(lastSlash + 1) : path;
    }

    @Override
    public synchronized void close() throws IOException {
        running = false;
        if (eventReaderThread != null) {
            eventReaderThread.interrupt();
        }
        if (sessionConnection != null) {
            sessionConnection.close();
        }
    }

    private static class SessionConnection implements Closeable {
        private final SSLSocket socket;
        private final BufferedWriter writer;
        private final BufferedReader reader;

        public SessionConnection(SSLSocket socket, BufferedWriter writer, BufferedReader reader) {
            this.socket = socket;
            this.writer = writer;
            this.reader = reader;
        }

        public BufferedWriter getWriter() { return writer; }
        public BufferedReader getReader() { return reader; }

        @Override
        public void close() throws IOException {
            try { writer.close(); } catch (IOException ignored) {}
            try { reader.close(); } catch (IOException ignored) {}
            try { socket.close(); } catch (IOException ignored) {}
        }
    }
}