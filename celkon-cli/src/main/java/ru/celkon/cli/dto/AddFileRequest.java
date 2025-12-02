package ru.celkon.cli.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

public class AddFileRequest {

    @JsonProperty("session_id")
    private String sessionId;
    private String path;
    private int algorithm;
    private String hash = "23";
    @JsonProperty("for_users")
    private String forUsers = "0";

    public AddFileRequest() {}

    public AddFileRequest(String sessionId, String path, int algorithm) {
        this.sessionId = sessionId;
        this.path = path;
        this.algorithm = algorithm;
    }

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public int getAlgorithm() {
        return algorithm;
    }

    public void setAlgorithm(int algorithm) {
        this.algorithm = algorithm;
    }

    public String getHash() {
        return hash;
    }

    public void setHash(String hash) {
        this.hash = hash;
    }

    public String getForUsers() {
        return forUsers;
    }

    public void setForUsers(String forUsers) {
        this.forUsers = forUsers;
    }
}
