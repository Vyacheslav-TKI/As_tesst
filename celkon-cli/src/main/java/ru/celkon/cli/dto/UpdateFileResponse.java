package ru.celkon.cli.dto;

public class UpdateFileResponse {

    private String status;
    private String message;

    public UpdateFileResponse() {}

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
}
