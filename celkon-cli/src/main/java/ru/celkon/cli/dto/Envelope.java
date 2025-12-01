package ru.celkon.cli.dto;

import com.fasterxml.jackson.annotation.JsonUnwrapped;

public class Envelope<T> {

    private String cmd;
    @JsonUnwrapped
    private T data;

    public Envelope() {}

    public Envelope(String action, T data) {
        this.cmd = action;
        this.data = data;
    }

    public String getCmd() {
        return cmd;
    }

    public void setCmd(String cmd) {
        this.cmd = cmd;
    }

    public T getData() {
        return data;
    }

    public void setData(T data) {
        this.data = data;
    }
}
