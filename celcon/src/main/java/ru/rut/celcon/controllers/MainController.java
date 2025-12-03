package ru.rut.celcon.controllers;

import jakarta.servlet.http.HttpSession;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
public class MainController {
    @GetMapping("/")
    public String mainPage(HttpSession session, Model model) {
        if (session.getAttribute("daemonSessionId") == null) {
            return "redirect:/auth"; // Не авторизован — редирект на /auth
        }

        // Загрузка списка файлов через демон (используя session_id из сессии)
        // model.addAttribute("files", ...);
        return "redirect:/files";
    }
}
