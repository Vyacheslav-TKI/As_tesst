package ru.celkon.cli.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

public class LoginResponse {

    private int code;
    @JsonProperty("session_id")
    private String sessionId;
    @JsonProperty("answ")
    private String message;  // for error messages

    public LoginResponse() {}

    public String getMessage() {
        return message;
    }

    public String getToken() {
        return sessionId;
    }

    public int getCode() {
        return code;
    }

    public void setCode(int code) {
        this.code = code;
    }

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }
}
