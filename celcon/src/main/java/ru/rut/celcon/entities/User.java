package ru.rut.celcon.entities;

public class User {
    public String fio;
    public String post;
    public int role = -1;
    public String sessionId;
    public User(String fio, String post, int role, String sessionId) {
        this.fio = fio;
        this.post = post;
        this.role = role;
        this.sessionId = sessionId;
    }

    public String getFio() {
        return fio;
    }

    public String getPost() {
        return post;
    }

    public int getRole() {
        return role;
    }

    public String getSessionId() {
        return sessionId;
    }
}
