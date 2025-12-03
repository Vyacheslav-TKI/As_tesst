package ru.rut.celcon.communication.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.List;


public class AddFileRequest {
    @JsonProperty("session_id")
    private String sessionId;
    private List<FileInfo> files;

    public String getSessionId() {
        return sessionId;
    }

    public void setSessionId(String sessionId) {
        this.sessionId = sessionId;
    }

    public List<FileInfo> getFiles() {
        return files;
    }

    public void setFiles(List<FileInfo> files) {
        this.files = files;
    }
}
