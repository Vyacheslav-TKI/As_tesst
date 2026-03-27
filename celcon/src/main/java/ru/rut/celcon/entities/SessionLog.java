package ru.rut.celcon.entities;

public class SessionLog {
    private int id;
    private String fio;
    private long timestamp;

    public SessionLog(int id, String fio, long timestamp) {
        this.id = id;
        this.fio = fio;
        this.timestamp = timestamp;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getFio() {
        return fio;
    }

    public void setFio(String fio) {
        this.fio = fio;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }
}
