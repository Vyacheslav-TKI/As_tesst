package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.*;
import ru.celkon.cli.ui.InputReader;

public class DeleteFileCommand {

    private final CelkonClient client;
    private final InputReader input;

    public DeleteFileCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public DeleteFileResponse execute(String token) {

        int id = Integer.parseInt(input.readLine("File ID to delete: "));

        DeleteFileRequest req = new DeleteFileRequest(token, id);
        Envelope<DeleteFileRequest> env = new Envelope<>("delete_file", req);

        DeleteFileResponse resp = client.send(env, DeleteFileResponse.class);

        if (resp != null)
            System.out.println("Status: " + resp.getStatus());

        return resp;
    }
}
