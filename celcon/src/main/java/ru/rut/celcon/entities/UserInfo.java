package ru.rut.celcon.entities;

public class UserInfo {
    protected int id;
    protected String fio;
    protected String post;
    protected int role;
    public UserInfo(int id, String fio, String post, int role) {
        this.id = id;
        this.fio = fio;
        this.post = post;
        this.role = role;
    }

    public int getId() {
        return id;
    }

    public int getRole() {
        return role;
    }

    public String getFio() {
        return fio;
    }

    public String getPost() {
        return post;
    }

    public void setId(int id) {
        this.id = id;
    }

    public void setFio(String fio) {
        this.fio = fio;
    }

    public void setRole(int role) {
        this.role = role;
    }

    public void setPost(String post) {
        this.post = post;
    }
}
