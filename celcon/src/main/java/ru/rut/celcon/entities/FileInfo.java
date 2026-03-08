package ru.rut.celcon.entities;

public class FileInfo {
    private int id;
    private String name;
    private String path;
    private FileStatus status = FileStatus.PENDING;

    public FileInfo(int id, String name, String path) {
        this.id = id;
        this.name = name;
        this.path = path;
        //this.changed = changed;
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

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public FileStatus getStatus() {
        return status;
    }

    public void setStatus(FileStatus status) {
        this.status = status;
    }
}
