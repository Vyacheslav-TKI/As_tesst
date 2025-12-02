package ru.celkon.cli.client;

import javax.net.ssl.*;
import java.io.FileInputStream;
import java.security.KeyStore;

public class TLSConnectionFactory {

    private SSLSocketFactory factory;

    public TLSConnectionFactory() {
        try {
            // Загружаем truststore
            KeyStore ts = KeyStore.getInstance("JKS");
            ts.load(new FileInputStream("truststore.jks"), "changeit".toCharArray());

            // Инициализируем TrustManager
            TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            tmf.init(ts);

            SSLContext ctx = SSLContext.getInstance("TLS");
            ctx.init(null, tmf.getTrustManagers(), null);

            factory = ctx.getSocketFactory();

        } catch (Exception e) {
            throw new RuntimeException("Failed to initialize TLS: " + e.getMessage(), e);
        }
    }

    public SSLSocket createSocket(String host, int port) throws Exception {
        SSLSocket socket = (SSLSocket) factory.createSocket(host, port);

        socket.startHandshake();
        return socket;
    }
}
