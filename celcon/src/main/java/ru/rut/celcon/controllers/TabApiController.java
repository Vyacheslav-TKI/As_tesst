package ru.rut.celcon.controllers;

import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.FileInfo;
import ru.rut.celcon.entities.UserInfo;
import ru.rut.celcon.services.StatisticsService;
import jakarta.servlet.http.HttpSession;

import java.util.List;
import java.util.Map;

@RestController
@RequestMapping("/api/tabs")
public class TabApiController {

    private final NetClient netClient;
    private final StatisticsService statisticsService; // Нужно будет создать

    public TabApiController(NetClient netClient, StatisticsService statisticsService) {
        this.netClient = netClient;
        this.statisticsService = statisticsService;
    }

    // API для вкладки "Файлы"
    @GetMapping("/files")
    public Map<String, Object> getFilesData(HttpSession session) {
        String sessionId = (String) session.getAttribute("daemonSessionId");

        if (sessionId == null) {
            return Map.of("error", "Not authenticated");
        }

        try {
            List<FileInfo> files = netClient.sync(sessionId);
            int userLevel = Integer.parseInt(String.valueOf(
                    session.getAttribute("userLevel")));
            boolean hasEditRights = userLevel >= 1;

            return Map.of(
                    "files", files,
                    "hasEditRights", hasEditRights,
                    "userLevel", userLevel
            );
        } catch (Exception e) {
            return Map.of("error", "Ошибка загрузки файлов: " + e.getMessage());
        }
    }

    // API для вкладки "Статистика"
    @GetMapping("/statistics")
    public Map<String, Object> getStatisticsData(
            @RequestParam(required = false) String from,
            @RequestParam(required = false) String to,
            HttpSession session) {

        String sessionId = (String) session.getAttribute("daemonSessionId");

        if (sessionId == null) {
            return Map.of("error", "Not authenticated");
        }

        try {
            // Используем сервис статистики
            Map<String, Object> stats = statisticsService.getStatistics(
                    sessionId, from, to);

            return Map.of(
                    "success", true,
                    "data", stats,
                    "startDate", from,
                    "endDate", to
            );
        } catch (Exception e) {
            return Map.of("error", "Ошибка загрузки статистики: " + e.getMessage());
        }
    }

    // API для графика статистики
    @GetMapping("/statistics/chart")
    public Map<String, Object> getChartData(
            @RequestParam(required = false) String from,
            @RequestParam(required = false) String to,
            HttpSession session) {

        String sessionId = (String) session.getAttribute("daemonSessionId");

        if (sessionId == null) {
            return Map.of("error", "Not authenticated");
        }

        try {
            // Данные для Chart.js
            Map<String, Object> chartData = statisticsService.getChartData(
                    sessionId, from, to);

            return Map.of(
                    "success", true,
                    "chartData", chartData
            );
        } catch (Exception e) {
            return Map.of("error", "Ошибка загрузки графика: " + e.getMessage());
        }
    }

}
