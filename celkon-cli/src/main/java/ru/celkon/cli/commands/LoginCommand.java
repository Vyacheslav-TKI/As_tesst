package ru.celkon.cli.commands;

import ru.celkon.cli.client.CelkonClient;
import ru.celkon.cli.dto.Envelope;
import ru.celkon.cli.dto.LoginRequest;
import ru.celkon.cli.dto.LoginResponse;
import ru.celkon.cli.ui.InputReader;

public class LoginCommand {

    private final CelkonClient client;
    private final InputReader input;

    public LoginCommand(CelkonClient client, InputReader input) {
        this.client = client;
        this.input = input;
    }

    public LoginResponse execute() {
        String username = input.readLine("Username: ");
        String password = input.readLine("Password: ");

        LoginRequest req = new LoginRequest(username, password);
        Envelope<LoginRequest> env = new Envelope<>("AUTH", req);

        LoginResponse resp = client.send(env, LoginResponse.class);

        if (resp != null)
            System.out.println("Response: " + resp.getMessage());

        return resp;
    }
}
