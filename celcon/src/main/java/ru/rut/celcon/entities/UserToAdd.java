package ru.rut.celcon.entities;

public class UserToAdd {
    private String login;
    private String password;
    private String fio;
    private String post;
    private int role;
    public UserToAdd(String fio, String post, int role, String login, String password) {
        this.login = login;
        this.password = password;
        this.fio = fio;
        this.post = post;
        this.role = role;
    }

    public String getLogin() {
        return login;
    }

    public void setLogin(String login) {
        this.login = login;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getFio() {
        return fio;
    }

    public void setFio(String fio) {
        this.fio = fio;
    }

    public String getPost() {
        return post;
    }

    public void setPost(String post) {
        this.post = post;
    }

    public int getRole() {
        return role;
    }

    public void setRole(int role) {
        this.role = role;
    }
}
