package ru.celkon.cli.dto;

public class LoginResponse {

    private String status;   // "ok" / "error"
    private String message;  // for error messages
    private String token;    // auth token
    private int role;        // 0=user, 1=admin

    public LoginResponse() {}

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public int getRole() {
        return role;
    }

    public void setRole(int role) {
        this.role = role;
    }
}
