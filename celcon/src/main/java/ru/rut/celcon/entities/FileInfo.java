package ru.rut.celcon.entities;

public class FileInfo {
    private int id;
    private String name;
    private String path;
    private boolean changed;

    public FileInfo(String name, String path, boolean changed) {
        this.name = name;
        this.path = path;
        this.changed = changed;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public boolean isChanged() {
        return changed;
    }

    public void setChanged(boolean changed) {
        this.changed = changed;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }
}
