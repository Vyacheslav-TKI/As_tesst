package ru.rut.celcon.controllers;

import jakarta.servlet.http.HttpSession;
import org.springframework.format.annotation.DateTimeFormat;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import ru.rut.celcon.communication.client.NetClient;
import ru.rut.celcon.entities.FileChange;
import tools.jackson.databind.ObjectMapper;

import java.time.Instant;
import java.time.LocalDate;
import java.time.ZoneId;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Controller
public class HistoryController {
    private final NetClient netClient;

    private List<FileChange> changes;

    public HistoryController(NetClient netClient) {
        this.netClient = netClient;
    }

    @GetMapping("/history")
    public String historyMainPage() {
        return "redirect:/history/summary";
    }

    @GetMapping("/history/summary")
    public String historySummary(HttpSession session,
                                 @RequestParam(required = false) @DateTimeFormat(pattern = "yyyy-MM-dd") LocalDate dateFrom,
                                 @RequestParam(required = false) @DateTimeFormat(pattern = "yyyy-MM-dd") LocalDate dateTo,
                                 Model model) {
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
            LocalDate endDate = dateTo != null ? dateTo : LocalDate.now();
            LocalDate startDate = dateFrom != null ? dateFrom : endDate.minusDays(30);
            long dateBegin = startDate.atStartOfDay(ZoneId.systemDefault()).toEpochSecond();
            long dateEnd = endDate.atTime(23, 59, 59).atZone(ZoneId.systemDefault()).toEpochSecond();
            changes = netClient.getFileChanges(sessionId, dateBegin, dateEnd);
            Map<String, Long> changesByDate = changes.stream().collect(Collectors.groupingBy(
                    change -> Instant.ofEpochSecond(change.getTimestamp()).atZone(ZoneId.systemDefault()).toLocalDate().toString(), Collectors.counting()
            ));
            ObjectMapper objectMapper = new ObjectMapper();
            String changesByDateJson = objectMapper.writeValueAsString(changesByDate);
            System.out.println(changesByDateJson);
            System.out.println(changes);
            model.addAttribute("changes", changes);
            model.addAttribute("changesByDate", changesByDateJson);
            model.addAttribute("dateFrom", startDate);
            model.addAttribute("dateTo", endDate);
            model.addAttribute("totalChanges", changes.size());
        } catch (Exception e) {
            model.addAttribute("error", "Ошибка получения данных " + e.getMessage());
            model.addAttribute("changes", List.of());
            model.addAttribute("changesByDate", Map.of());
            e.printStackTrace();
        }

        return "history/summary";
    }
}
