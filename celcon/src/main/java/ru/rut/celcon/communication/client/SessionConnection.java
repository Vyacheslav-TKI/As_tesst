package ru.rut.celcon.communication.client;

import javax.net.ssl.SSLSocket;
import java.io.BufferedReader;
import java.io.BufferedWriter;

// Внутри NetClient.java или отдельно
public class SessionConnection {
    private final SSLSocket socket;
    private final BufferedWriter writer;
    private final BufferedReader reader;

    public SessionConnection(SSLSocket socket, BufferedWriter writer, BufferedReader reader) {
        this.socket = socket;
        this.writer = writer;
        this.reader = reader;
    }

    public SSLSocket getSocket() { return socket; }
    public BufferedWriter getWriter() { return writer; }
    public BufferedReader getReader() { return reader; }
}