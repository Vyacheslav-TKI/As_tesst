package ru.celkon.cli.dto;

public class AddFileRequest {

    private String token;
    private String path;
    private int algorithm;

    public AddFileRequest() {}

    public AddFileRequest(String token, String path, int algorithm) {
        this.token = token;
        this.path = path;
        this.algorithm = algorithm;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
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
}
