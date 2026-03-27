package ru.rut.celcon.entities;

public class FileChange {
    private int changeId;
    private int fileId;
    private String path;
    private int hashAlgorithm;
    private String baselineHash;
    private String currentHash;
    private long timestamp;

    public FileChange(int changeId, int fileId, String path, int hashAlgorithm, String baselineHash, String currentHash, long timestamp) {
        this.changeId = changeId;
        this.fileId = fileId;
        this.path = path;
        this.hashAlgorithm = hashAlgorithm;
        this.baselineHash = baselineHash;
        this.currentHash = currentHash;
        this.timestamp = timestamp;
    }

    public int getChangeId() {
        return changeId;
    }

    public void setChangeId(int changeId) {
        this.changeId = changeId;
    }

    public String getPath() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public String getBaselineHash() {
        return baselineHash;
    }

    public void setBaselineHash(String baselineHash) {
        this.baselineHash = baselineHash;
    }

    public String getCurrentHash() {
        return currentHash;
    }

    public void setCurrentHash(String currentHash) {
        this.currentHash = currentHash;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public int getHashAlgorithm() {
        return hashAlgorithm;
    }

    public void setHashAlgorithm(int hashAlgorithm) {
        this.hashAlgorithm = hashAlgorithm;
    }

    public int getFileId() {
        return fileId;
    }

    public void setFileId(int fileId) {
        this.fileId = fileId;
    }
    public String toString() {
        return "path: " + path + " timestamp: " + timestamp + "\n";
    }
}
