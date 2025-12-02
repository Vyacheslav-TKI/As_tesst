package ru.celkon.cli.dto;

import java.util.List;

public class ListFilesResponse {

    private String status; // "ok" / "error"
    private List<FileInfo> files;
    private String message; // error message if any

    public ListFilesResponse() {}

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public List<FileInfo> getFiles() {
        return files;
    }

    public void setFiles(List<FileInfo> files) {
        this.files = files;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }
}
