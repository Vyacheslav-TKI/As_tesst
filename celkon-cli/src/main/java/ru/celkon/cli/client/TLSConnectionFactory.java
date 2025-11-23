package ru.celkon.cli.client;

import javax.net.ssl.*;
import java.io.FileInputStream;
import java.security.KeyStore;

public class TLSConnectionFactory {

    public SSLSocket createSecureSocket(String host, int port) throws Exception {

        SSLContext context = SSLContext.getInstance("TLSv1.3");

        KeyStore trustStore = KeyStore.getInstance("JKS");
        trustStore.load(new FileInputStream("truststore.jks"), "password".toCharArray());

        TrustManagerFactory tmf = TrustManagerFactory.getInstance(
                TrustManagerFactory.getDefaultAlgorithm()
        );
        tmf.init(trustStore);

        context.init(null, tmf.getTrustManagers(), null);

        SSLSocketFactory factory = context.getSocketFactory();
        return (SSLSocket) factory.createSocket(host, port);
    }
}
