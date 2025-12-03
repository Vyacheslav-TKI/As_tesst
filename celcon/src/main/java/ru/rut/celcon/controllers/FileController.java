package ru.rut.celcon.controllers;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import ru.rut.celcon.FileInfo;

import java.util.List;

import jakarta.servlet.http.HttpSession;
import ru.rut.celcon.communication.client.NetClient;

import java.util.*;

@Controller
public class FileController {

    private final NetClient netClient;

    public FileController(NetClient netClient) {
        this.netClient = netClient;
    }

    @GetMapping("/files")
    public String showFiles(Model model, HttpSession session) {
        String sessionId = (String) session.getAttribute("daemonSessionId");
        model.addAttribute("userFio", session.getAttribute("userFio"));
        model.addAttribute("userRole", "Уровень" + String.valueOf(session.getAttribute("userRole")));
        model.addAttribute("userPost", session.getAttribute("userPost"));
        if (sessionId == null) {
            return "redirect:/auth";
        }

        try {
            List<FileInfo> files = netClient.sync(sessionId);
            boolean hasEditRights = session.getAttribute("userRole") != null
                    && (int) session.getAttribute("userRole") >= 1;

            model.addAttribute("files", files);
            model.addAttribute("hasEditRights", hasEditRights);
            return "files";
        } catch (Exception e) {
            model.addAttribute("error", "Ошибка загрузки файлов: " + e.getMessage());
            return "files";
        }
    }

    private String extractFileName(String path) {
        if (path == null || path.isEmpty()) return "unknown";
        int lastSlash = path.lastIndexOf('/');
        return lastSlash >= 0 ? path.substring(lastSlash + 1) : path;
    }
}
