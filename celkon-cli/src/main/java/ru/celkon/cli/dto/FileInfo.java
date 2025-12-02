package ru.celkon.cli.dto;

public class FileInfo {

    private int file_id;
    private String path;
    private int algorithm;
    private String baseline_hash;

    public FileInfo() {}

    public int getFile_id() {
        return file_id;
    }

    public void setFile_id(int file_id) {
        this.file_id = file_id;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public int getAlgorithm() {
        return algorithm;
    }

    public void setAlgorithm(int algorithm) {
        this.algorithm = algorithm;
    }

    public String getBaseline_hash() {
        return baseline_hash;
    }

    public void setBaseline_hash(String baseline_hash) {
        this.baseline_hash = baseline_hash;
    }
}
