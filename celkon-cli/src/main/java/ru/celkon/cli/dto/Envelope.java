package ru.celkon.cli.dto;

public class Envelope<T> {
    private String action;
    private T payload;

    public Envelope() {}

    public Envelope(String action, T payload) {
        this.action = action;
        this.payload = payload;
    }

    // getters/setters
}
