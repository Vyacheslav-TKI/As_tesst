package ru.celkon.cli.client;

import ru.celkon.cli.dto.Envelope;

public class CelkonClient {

    public CelkonClient() {
        // TODO: load config if needed
    }

    public <T> T sendRequest(Envelope<?> request, Class<T> responseClass) {
        // TODO: open TLS socket, send JSON, receive JSON
        return null;
    }
}
