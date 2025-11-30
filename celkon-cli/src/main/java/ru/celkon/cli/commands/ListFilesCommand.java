package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.*;
import ru.celkon.cli.ui.InputReader;

public class ListFilesCommand {

    private final CelkonClient client;
    private final InputReader input;

    public ListFilesCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public ListFilesResponse execute(String token) {

        ListFilesRequest req = new ListFilesRequest(token);
        Envelope<ListFilesRequest> env = new Envelope<>("list_files", req);

        ListFilesResponse resp = client.send(env, ListFilesResponse.class);

        if (resp != null && resp.getFiles() != null) {
            for (FileInfo f : resp.getFiles()) {
                System.out.println(f.getFile_id() + " | " + f.getPath() + " | alg=" + f.getAlgorithm());
            }
        }

        return resp;
    }
}
