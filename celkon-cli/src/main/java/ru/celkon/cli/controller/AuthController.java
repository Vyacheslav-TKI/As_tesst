package ru.celkon.cli.controller;

import ru.celkon.cli.client.BackendClient;

public class AuthController {

    private final BackendClient client;

    public AuthController(BackendClient client) {
        this.client = client;
    }

    public boolean login(String user, String pass) {
        try {
            return client.login(user, pass);
        } catch (Exception e) {
            System.out.println("Ошибка сети: " + e.getMessage());
            return false;
        }
    }
}
