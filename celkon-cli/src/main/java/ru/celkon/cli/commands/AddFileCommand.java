package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.*;
import ru.celkon.cli.ui.InputReader;

public class AddFileCommand {

    private final CelkonClient client;
    private final InputReader input;

    public AddFileCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public AddFileResponse execute(String token) {

        String path = input.readLine("File path: ");
        int alg = Integer.parseInt(input.readLine("Algorithm (0..4): "));

        AddFileRequest req = new AddFileRequest(token, path, alg);
        Envelope<AddFileRequest> env = new Envelope<>("ADD_FILES", req);

        AddFileResponse resp = client.send(env, AddFileResponse.class);

        if (resp != null)
            System.out.println("Status: " + resp.getStatus());

        return resp;
    }
}
