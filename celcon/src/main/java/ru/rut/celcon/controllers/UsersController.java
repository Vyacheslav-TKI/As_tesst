package ru.rut.celcon.controllers;

import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.ResponseBody;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.UserInfo;

import java.util.List;
import java.util.Map;

@Controller
public class UsersController {
    private final NetClient netClient;

    public UsersController(NetClient netClient) {
        this.netClient = netClient;
    }
    @GetMapping("/users")
    public String getUsersData(Model model, HttpSession session) {
        String sessionId = (String)session.getAttribute("daemonSessionId");

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

        List<UserInfo> users = netClient.listUsers(sessionId);;
        model.addAttribute("users", users);
        return "users";
    }
    @PostMapping("/api/users/add")
    @ResponseBody
    public Map<String, Object> addUser(
            @RequestBody Map<String, Object> request,
            HttpSession session) {
        String sessionId = (String) session.getAttribute("daemonSessionId");
        if (sessionId == null) {
            return Map.of("code", 401, "answ", "Not authenticated");
        }

        String login = (String) request.get("login");
        String password = (String) request.get("password");
        String fio = (String) request.get("fio");
        String post = (String) request.get("post");
        int role = (Integer) request.get("role");
        return null;
    }
}
