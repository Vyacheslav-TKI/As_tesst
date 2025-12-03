package ru.rut.celcon.communication.dto;

import com.fasterxml.jackson.annotation.JsonProperty;

public class FileInfo {
    protected int id;
    protected String path;
    protected int alg;
    protected String hash;
    @JsonProperty("for_users")
    protected String forUsers;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public int getAlg() {
        return alg;
    }

    public void setAlg(int alg) {
        this.alg = alg;
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
