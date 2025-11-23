package ru.celkon.cli.controller;

import ru.celkon.cli.client.BackendClient;

public class FileController {
    private final BackendClient client;

    public FileController(BackendClient client) {
        this.client = client;
    }

    public void list() {
        try {
            System.out.println(client.listFiles());
        } catch (Exception e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
    }

    public void add(String path, int alg, String users) {
        try {
            System.out.println(client.addFile(path, alg, users));
        } catch (Exception e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
    }

    public void delete(int id) {
        try {
            System.out.println(client.deleteFile(id));
        } catch (Exception e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
    }

    public void update(int id, int alg) {
        try {
            System.out.println(client.updateFile(id, alg));
        } catch (Exception e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
    }
}
