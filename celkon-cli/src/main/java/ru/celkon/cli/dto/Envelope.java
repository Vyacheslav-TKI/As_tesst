package ru.celkon.cli.dto;

public class Envelope<T> {

    private String action;
    private T data;

    public Envelope() {}

    public Envelope(String action, T data) {
        this.action = action;
        this.data = data;
    }

    public String getAction() {
        return action;
    }

    public void setAction(String action) {
        this.action = action;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }
}
