package ru.rut.celcon.controllers;

import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.SessionLog;

import java.util.List;

@Controller
public class SessionsLogController {
    private final NetClient netClient;
    public SessionsLogController(NetClient netClient) {
        this.netClient = netClient;
    }

    @GetMapping("/sessions_log")
    public String listSessionsLog(Model model, HttpSession session) {
        String sessionId = (String) session.getAttribute("daemonSessionId");

        if (sessionId == null) {
            return "redirect:/auth";
        }

        model.addAttribute("userFio", session.getAttribute("userFio"));
        int userLevel = Integer.parseInt(String.valueOf(session.getAttribute("userLevel")));
        if (userLevel == 2) {
            model.addAttribute("userLevel", "Администратор");
        } else if (userLevel == 1) {
            model.addAttribute("userLevel", "Пользователь уровня 1");
        } else if (userLevel == 0) {
            model.addAttribute("userLevel", "Пользователь уровня 2");
        } else {
            model.addAttribute("userLevel", "");
        }
        model.addAttribute("userPost", session.getAttribute("userPost"));
        model.addAttribute("isAdmin",session.getAttribute("isAdmin"));

        try {
            List<SessionLog> sessionsLog = netClient.getSessionsLog(sessionId);

            model.addAttribute("sessionsLog", sessionsLog);
            return "sessions_log";
        } catch (Exception e) {
            model.addAttribute("error", "Ошибка загрузки истории входов: " + e.getMessage());
            return "sessions_log";
        }
    }
}
