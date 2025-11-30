package ru.celkon.cli.dto;

public class StatusRequest {

    private String token;
    private int file_id;

    public StatusRequest() {}

    public StatusRequest(String token, int file_id) {
        this.token = token;
        this.file_id = file_id;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public int getFile_id() {
        return file_id;
    }

    public void setFile_id(int file_id) {
        this.file_id = file_id;
    }
}
