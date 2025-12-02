package ru.celkon.cli.util;

import ru.celkon.cli.dto.FileInfo;
import ru.celkon.cli.dto.StatusResponse;

import java.util.List;

public class PrettyPrint {

    // Печать списка файлов в виде таблицы
    public static void fileList(List<FileInfo> files) {
        if (files == null || files.isEmpty()) {
            System.out.println("No files.");
            return;
        }

        System.out.printf("%-6s | %-40s | %-4s | %s\n",
                "ID", "Path", "Alg", "Baseline");

        System.out.println("----------------------------------------------------------------------");

        for (FileInfo f : files) {
            System.out.printf("%-6d | %-40s | %-4d | %s\n",
                    f.getFile_id(),
                    f.getPath(),
                    f.getAlgorithm(),
                    f.getBaseline_hash());
        }
    }

    // Печать результата status
    public static void fileStatus(StatusResponse resp) {
        if (resp == null) {
            System.out.println("Invalid response.");
            return;
        }

        System.out.println("File status:");
        System.out.println("Changed   : " + resp.isChanged());
        System.out.println("New Hash  : " + resp.getNew_hash());
        System.out.println("Timestamp : " + resp.getTimestamp());
    }
}
