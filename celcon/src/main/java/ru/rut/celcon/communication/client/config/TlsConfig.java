package ru.rut.celcon.communication.client.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManagerFactory;
import java.io.InputStream;
import java.security.KeyStore;

@Configuration
public class TlsConfig {

    @Bean
    public SSLContext daemonSslContext() throws Exception {
        // Указываем тип "JKS" вместо "PKCS12"
        KeyStore trustStore = KeyStore.getInstance("JKS");

        try (InputStream in = getClass().getResourceAsStream("/truststore.jks")) {
            if (in == null) {
                throw new RuntimeException("Файл truststore.jks не найден в classpath (src/main/resources)");
            }
            // Замените "changeit" на реальный пароль от вашего truststore
            trustStore.load(in, "changeit".toCharArray());
        }

        TrustManagerFactory tmf = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
        tmf.init(trustStore);

        SSLContext sslContext = SSLContext.getInstance("TLS");
        sslContext.init(null, tmf.getTrustManagers(), null); // без mTLS → keyManagers = null
        return sslContext;
    }
}