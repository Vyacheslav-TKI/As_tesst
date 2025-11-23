package ru.celkon.cli.client;

import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLContext;

public class TLSConnectionFactory {

    public SSLSocket createSocket(String host, int port) throws Exception {
        // TODO: implement TLS socket creation
        SSLContext ctx = SSLContext.getDefault();
        return (SSLSocket) ctx.getSocketFactory().createSocket(host, port);
    }
}
