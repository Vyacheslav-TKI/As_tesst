package ru.rut.celcon.controllers;

import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.*;
import ru.rut.celcon.FileInfo;

import java.util.List;

import jakarta.servlet.http.HttpSession;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.FileToAdd;

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

        try {
            List<FileInfo> files = netClient.sync(sessionId);
            boolean hasEditRights = userLevel >= 1;

            model.addAttribute("files", files);
            model.addAttribute("hasEditRights", hasEditRights);
            return "files";
        } catch (Exception e) {
            model.addAttribute("error", "Ошибка загрузки файлов: " + e.getMessage());
            return "files";
        }
    }

    @PostMapping("/api/files/add")
    @ResponseBody
    public Map<String, Object> addFiles(
            @RequestBody Map<String, Object> request,
            HttpSession session) {

        String sessionId = (String) session.getAttribute("daemonSessionId");
        if (sessionId == null) {
            return Map.of("code", 401, "answ", "Not authenticated");
        }

        try {
            // Ожидаем:
            // { "files": [ { "path": "...", "alg": 0, "hash": "...", "for_users": "0,1" } ] }
            List<Map<String, Object>> filesRaw = (List<Map<String, Object>>) request.get("files");
            if (filesRaw == null || filesRaw.isEmpty()) {
                return Map.of("code", 400, "answ", "No files provided");
            }

            List<FileToAdd> files = filesRaw.stream()
                    .map(f -> {
                        String path = (String) f.get("path");
                        Integer alg = (Integer) f.get("alg");
                        String hash = (String) f.get("hash");
                        String forUsers = (String) f.get("for_users");

                        if (path == null || alg == null || hash == null || forUsers == null) {
                            throw new IllegalArgumentException("Missing required field in file");
                        }
                        return new FileToAdd(path, alg, hash, forUsers);
                    })
                    .toList();

            netClient.addFiles(sessionId, files);
            return Map.of("code", 200, "answ", "ok");
        } catch (Exception e) {
            return Map.of("code", 500, "answ", "Failed to add files: " + e.getMessage());
        }
    }

    private String extractFileName(String path) {
        if (path == null || path.isEmpty()) return "unknown";
        int lastSlash = path.lastIndexOf('/');
        return lastSlash >= 0 ? path.substring(lastSlash + 1) : path;
    }
}
