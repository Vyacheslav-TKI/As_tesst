package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.*;
import ru.celkon.cli.ui.InputReader;

public class UpdateFileCommand {

    private final CelkonClient client;
    private final InputReader input;

    public UpdateFileCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public UpdateFileResponse execute(String token) {

        int id = Integer.parseInt(input.readLine("File ID to modify: "));
        String newPath = input.readLine("New file path (leave empty to skip): ");
        String newAlgStr = input.readLine("New algorithm (leave empty to skip): ");

        UpdateFileRequest req = new UpdateFileRequest();
        req.setToken(token);
        req.setFile_id(id);

        if (!newPath.isEmpty())
            req.setNew_path(newPath);

        if (!newAlgStr.isEmpty())
            req.setNew_algorithm(Integer.parseInt(newAlgStr));

        Envelope<UpdateFileRequest> env = new Envelope<>("update_file", req);
        return client.send(env, UpdateFileResponse.class);
    }
}
