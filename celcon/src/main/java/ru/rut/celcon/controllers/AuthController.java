package ru.rut.celcon.controllers;

import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.User;

import javax.net.ssl.SSLSocket;
import java.util.Map;

@Controller
public class AuthController {
    private final NetClient netClient;

    public AuthController(NetClient netClient) {
        this.netClient = netClient;
    }

    @GetMapping("/auth")
    public String showAuthPage() {
        return "auth"; // Thymeleaf: templates/auth.html
    }

    @PostMapping("/api/auth")
    @ResponseBody
    public Map<String, Object>  authenticate(
            @RequestBody Map<String, String> request,
            HttpSession session) {

        String login = request.get("login");
        String token = request.get("token");

        try {

            // Отправляем AUTH

            User user = netClient.auth(login, token);
            session.setAttribute("daemonSessionId", user.sessionId);
            session.setAttribute("userLevel", user.role);
            session.setAttribute("userFio", user.fio);
            session.setAttribute("userPost", user.post);
            session.setAttribute("isAdmin",user.role == 2);

            // Возвращаем JSON, как от демона — или свой формат с code=200
            return Map.of(
                    "code", 200,
                    "answ", "ok",
                    "redirectUrl", "/files" // ← опционально, если фронт не знает, куда идти
            );

        } catch (Exception e) {
            // Возвращаем ошибку в формате JSON
            return Map.of(
                    "code", 500,
                    "answ", "Auth failed: " + e.getMessage()
            );
        }
    }
}
