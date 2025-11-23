package ru.celkon.cli.client;

import ru.celkon.cli.proto.Protocol;
import ru.celkon.cli.proto.ResponseParser;

import javax.net.ssl.SSLSocket;
import java.io.*;

public class BackendClient {

    private final String host;
    private final int port;
    private final TLSConnectionFactory tlsFactory = new TLSConnectionFactory();

    public BackendClient(String host, int port) {
        this.host = host;
        this.port = port;
    }

    private SSLSocket connect() throws Exception {
        return tlsFactory.createSecureSocket(host, port);
    }

    public String send(String request) throws Exception {
        try (SSLSocket socket = connect()) {
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            out.println(request);
            return in.readLine();
        }
    }

    public boolean login(String username, String password) throws Exception {
        String cmd = Protocol.makeLogin(username, password);
        String resp = send(cmd);
        return ResponseParser.parseLogin(resp);
    }

    public String listFiles() throws Exception {
        return send(Protocol.LIST_FILES);
    }

    public String addFile(String path, int algorithm, String users) throws Exception {
        return send(Protocol.addFile(path, algorithm, users));
    }

    public String deleteFile(int id) throws Exception {
        return send(Protocol.deleteFile(id));
    }

    public String updateFile(int id, int alg) throws Exception {
        return send(Protocol.updateFile(id, alg));
    }

    public String getStats(int id) throws Exception {
        return send(Protocol.getStats(id));
    }
}
