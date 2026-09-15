package ru.rut.celcon.services;

import org.springframework.stereotype.Service;
import ru.rut.celcon.communication.client.NetClient;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.*;

@Service
public class StatisticsService {

    private final NetClient netClient;

    public StatisticsService(NetClient netClient) {
        this.netClient = netClient;
    }

    public Map<String, Object> getStatistics(String sessionId, String from, String to) {
        // Парсим даты или используем умолчания
        LocalDate startDate = parseDate(from, LocalDate.now().minusDays(7));
        LocalDate endDate = parseDate(to, LocalDate.now());

        // Здесь логика получения статистики через netClient
        // Это пример - адаптируйте под ваш API

        Map<String, Object> stats = new HashMap<>();
        stats.put("totalFiles", 150);
        stats.put("changedFiles", 23);
        stats.put("unchangedFiles", 127);
        stats.put("changePercentage", 15.3);
        stats.put("mostChangedCategory", "Конфигурационные файлы");
        stats.put("startDate", startDate.toString());
        stats.put("endDate", endDate.toString());

        return stats;
    }

    public Map<String, Object> getChartData(String sessionId, String from, String to) {
        // Данные для Chart.js
        Map<String, Object> chartData = new HashMap<>();

        // Пример данных для графика
        chartData.put("labels", Arrays.asList("Пн", "Вт", "Ср", "Чт", "Пт", "Сб", "Вс"));
        chartData.put("datasets", Arrays.asList(
                Map.of(
                        "label", "Изменённые файлы",
                        "data", Arrays.asList(12, 19, 3, 5, 2, 3, 7),
                        "backgroundColor", "rgba(255, 99, 132, 0.2)",
                        "borderColor", "rgba(255, 99, 132, 1)"
                ),
                Map.of(
                        "label", "Неизменённые файлы",
                        "data", Arrays.asList(20, 25, 30, 28, 32, 27, 25),
                        "backgroundColor", "rgba(54, 162, 235, 0.2)",
                        "borderColor", "rgba(54, 162, 235, 1)"
                )
        ));

        return chartData;
    }

    private LocalDate parseDate(String dateStr, LocalDate defaultValue) {
        if (dateStr == null || dateStr.isEmpty()) {
            return defaultValue;
        }
        try {
            return LocalDate.parse(dateStr);
        } catch (Exception e) {
            return defaultValue;
        }
    }
}
