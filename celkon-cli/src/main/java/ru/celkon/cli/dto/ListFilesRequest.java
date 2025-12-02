package ru.celkon.cli.dto;

public class ListFilesRequest {

    private String token;

    public ListFilesRequest() {}

    public ListFilesRequest(String token) {
        this.token = token;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }
}
