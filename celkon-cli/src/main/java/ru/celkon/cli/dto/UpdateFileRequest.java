package ru.celkon.cli.dto;

public class UpdateFileRequest {

    private String token;
    private int file_id;
    private String new_path;
    private Integer new_algorithm;

    public UpdateFileRequest() {}

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

    public String getNew_path() {
        return new_path;
    }

    public void setNew_path(String new_path) {
        this.new_path = new_path;
    }

    public Integer getNew_algorithm() {
        return new_algorithm;
    }

    public void setNew_algorithm(Integer new_algorithm) {
        this.new_algorithm = new_algorithm;
    }
}
