package ru.rut.celcon.entities;

public class FileToAdd {
    private String path;
    private int alg;        // 0 = MD5, 1 = SHA-1, 2 = SHA-256 и т.д.
    private String hash;    // эталонный хеш
    private String forUsers; // "0,1,2"

    // конструктор, геттеры, сеттеры
    public FileToAdd(String path, int alg, String hash, String forUsers) {
        this.path = path;
        this.alg = alg;
        this.hash = hash;
        this.forUsers = forUsers;
    }

    // геттеры
    public String getPath() { return path; }
    public int getAlg() { return alg; }
    public String getHash() { return hash; }
    public String getForUsers() { return forUsers; }
}
