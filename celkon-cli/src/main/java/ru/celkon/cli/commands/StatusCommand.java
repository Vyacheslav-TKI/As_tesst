package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.*;
import ru.celkon.cli.ui.InputReader;

public class StatusCommand {

    private final CelkonClient client;
    private final InputReader input;

    public StatusCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public StatusResponse execute(String token) {

        int id = Integer.parseInt(input.readLine("File ID to check: "));

        StatusRequest req = new StatusRequest(token, id);
        Envelope<StatusRequest> env = new Envelope<>("status", req);

        StatusResponse resp = client.send(env, StatusResponse.class);

        if (resp != null) {
            System.out.println("Changed: " + resp.isChanged());
            System.out.println("Hash: " + resp.getNew_hash());
            System.out.println("Timestamp: " + resp.getTimestamp());
        }

        return resp;
    }
}
