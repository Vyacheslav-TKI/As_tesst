package ru.celkon.cli.dto;

public class StatusResponse {

    private String status;
    private boolean changed;
    private long timestamp;
    private String new_hash;

    public StatusResponse() {}

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public boolean isChanged() {
        return changed;
    }

    public void setChanged(boolean changed) {
        this.changed = changed;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public String getNew_hash() {
        return new_hash;
    }

    public void setNew_hash(String new_hash) {
        this.new_hash = new_hash;
    }
}
