package ru.celkon.cli.ui;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.commands.*;
import ru.celkon.cli.dto.*;

public class CliMenu {

    private final CelkonClient client = new CelkonClient();
    private final InputReader input = new InputReader();

    private String token = null;
    private int role = -1; // 0=user, 1=admin

    public void start() {
        System.out.println("=== CELKON CLI ===");

        while (true) {
            if (token == null)
                loginMenu();
            else
                mainMenu();
        }
    }

    // ---------------- LOGIN MENU ----------------

    private void loginMenu() {
        System.out.println("\n1) Login");
        System.out.println("0) Exit");

        int choice = input.readInt("Select: ");

        switch (choice) {
            case 1 -> performLogin();
            case 0 -> exit();
            default -> System.out.println("Invalid option.");
        }
    }

    private void performLogin() {
        LoginCommand login = new LoginCommand(client, input);
        LoginResponse resp = login.execute();

        if (resp == null || !"ok".equals(resp.getMessage())) {
            System.out.println("Login failed: " +
                    (resp != null ? resp.getMessage() : "no response"));
            return;
        }

        token = resp.getToken();

        System.out.println("Logged in successfully.");
        mainMenu();
    }

    // ---------------- MAIN MENU ----------------

    private void mainMenu() {
        System.out.println("\n=== MAIN MENU ===");
        System.out.println("1) List files");
        System.out.println("2) Check file status");
        System.out.println("3) Add file");
        System.out.println("4) Update file");
        System.out.println("5) Delete file");
        System.out.println("9) Logout");
        System.out.println("0) Exit");

        int choice = input.readInt("Select: ");

        switch (choice) {

            case 1 -> listFiles();
            case 2 -> statusFile();

            case 3 -> {
                addFile();
            }

            case 4 -> {
                updateFile();
            }

            case 5 -> {
                deleteFile();
            }

            case 9 -> logout();
            case 0 -> exit();
            default -> System.out.println("Invalid option.");
        }
    }

    // ---------------- COMMAND HANDLERS ----------------

    private void listFiles() {
        ListFilesCommand cmd = new ListFilesCommand(client, input);

        ListFilesResponse resp = cmd.execute(token);
        if (resp == null || !"ok".equals(resp.getStatus())) {
            System.out.println("Error: " + (resp != null ? resp.getMessage() : "no response"));
        }
    }

    private void statusFile() {
        StatusCommand cmd = new StatusCommand(client, input);
        StatusResponse resp = cmd.execute(token);

        if (resp == null || !"ok".equals(resp.getStatus())) {
            System.out.println("Error checking file.");
        }
    }

    private void addFile() {
        AddFileCommand cmd = new AddFileCommand(client, input);
        AddFileResponse resp = cmd.execute(token);

        if (resp == null || !"ok".equals(resp.getStatus())) {
            System.out.println("Error: " + (resp != null ? resp.getMessage() : "no response"));
        }
    }

    private void updateFile() {
        UpdateFileCommand cmd = new UpdateFileCommand(client, input);
        UpdateFileResponse resp = cmd.execute(token);

        if (resp == null || !"ok".equals(resp.getStatus())) {
            System.out.println("Error updating: " + (resp != null ? resp.getMessage() : "no response"));
        }
    }

    private void deleteFile() {
        DeleteFileCommand cmd = new DeleteFileCommand(client, input);
        DeleteFileResponse resp = cmd.execute(token);

        if (resp == null || !"ok".equals(resp.getStatus())) {
            System.out.println("Error deleting: " + (resp != null ? resp.getMessage() : "no response"));
        }
    }

    // ---------------- SYSTEM ----------------

    private void logout() {
        token = null;
        role = -1;
        System.out.println("Logged out.");
    }

    private void exit() {
        System.out.println("Exiting...");
        System.exit(0);
    }
}
