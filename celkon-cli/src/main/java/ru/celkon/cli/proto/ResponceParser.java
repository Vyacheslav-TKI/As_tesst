package ru.celkon.cli.proto;

public class ResponseParser {

    public static boolean parseLogin(String resp) {
        return resp.startsWith("OK");
    }
}
