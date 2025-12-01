package ru.celkon.cli.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

public class AddFileResponse {

    private String status;
    @JsonProperty("answ")
    private String message;
    private int file_id;

    public AddFileResponse() {}

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

    public int getFile_id() {
        return file_id;
    }

    public void setFile_id(int file_id) {
        this.file_id = file_id;
    }
}
