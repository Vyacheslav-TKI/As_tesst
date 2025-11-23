package ru.celkon.cli.ui;

import ru.celkon.cli.client.BackendClient;
import ru.celkon.cli.controller.AuthController;
import ru.celkon.cli.controller.FileController;

import java.util.Scanner;

public class CliMenu {

    private BackendClient client =
            new BackendClient("127.0.0.1", 9090);

    private AuthController auth = new AuthController(client);
    private FileController files = new FileController(client);

    public void start() {
        Scanner sc = new Scanner(System.in);

        System.out.println("ЦЕЛКОН — CLI клиент");

        // ---- LOGIN ----
        boolean ok = false;
        while (!ok) {
            System.out.print("Логин: ");
            String u = sc.nextLine();
            System.out.print("Пароль: ");
            String p = sc.nextLine();

            ok = auth.login(u, p);
            if (!ok) System.out.println("Неверный логин или пароль!");
        }

        // ---- MAIN LOOP ----
        while (true) {
            System.out.println("\nДоступные команды:");
            System.out.println("1. list");
            System.out.println("2. add");
            System.out.println("3. delete");
            System.out.println("4. update");
            System.out.println("5. stats");
            System.out.println("0. exit");

            System.out.print("> ");
            String cmd = sc.nextLine();

            switch (cmd) {
                case "list" -> files.list();

                case "add" -> {
                    System.out.print("Путь: ");
                    String path = sc.nextLine();
                    System.out.print("Алгоритм: ");
                    int alg = Integer.parseInt(sc.nextLine());
                    System.out.print("Доступ (напр. 0 или 1,2): ");
                    String users = sc.nextLine();
                    files.add(path, alg, users);
                }

                case "delete" -> {
                    System.out.print("ID: ");
                    int id = Integer.parseInt(sc.nextLine());
                    files.delete(id);
                }

                case "update" -> {
                    System.out.print("ID: ");
                    int id = Integer.parseInt(sc.nextLine());
                    System.out.print("Новый алгоритм: ");
                    int alg = Integer.parseInt(sc.nextLine());
                    files.update(id, alg);
                }

                case "stats" -> {
                    System.out.print("FileID: ");
                    int id = Integer.parseInt(sc.nextLine());
                    try {
                        System.out.println(client.getStats(id));
                    } catch (Exception e) {
                        System.out.println("Ошибка: " + e.getMessage());
                    }
                }

                case "exit" -> System.exit(0);
            }
        }
    }
}
