package ru.celkon.cli.dto;

public class LoginRequest {

    private String login;
    private String token;

    public LoginRequest() {}

    public LoginRequest(String username, String password) {
        this.login = username;
        this.token = password;
    }

    public String getLogin() {
        return login;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }
}
