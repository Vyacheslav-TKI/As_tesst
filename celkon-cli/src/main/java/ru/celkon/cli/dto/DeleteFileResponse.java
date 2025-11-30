package ru.celkon.cli.dto;

public class DeleteFileResponse {

    private String status;
    private String message;

    public DeleteFileResponse() {}

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
