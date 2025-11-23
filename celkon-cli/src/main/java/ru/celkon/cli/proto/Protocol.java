package ru.celkon.cli.proto;

public class Protocol {

    public static final String LIST_FILES = "LIST_FILES";

    public static String makeLogin(String user, String pass) {
        return "LOGIN " + user + " " + pass;
    }

    public static String addFile(String path, int algorithm, String forUsers) {
        return "ADD_FILE " + path + " " + algorithm + " " + forUsers;
    }

    public static String deleteFile(int id) {
        return "DELETE_FILE " + id;
    }

    public static String updateFile(int id, int alg) {
        return "UPDATE_FILE " + id + " " + alg;
    }

    public static String getStats(int id) {
        return "GET_STATS " + id;
    }
}
