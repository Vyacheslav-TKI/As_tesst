package ru.celkon.cli;

import ru.celkon.cli.ui.CliMenu;

public class CelkonCliApplication {
    public static void main(String[] args) {
        System.out.println("Celkon CLI starting...");
        // TODO: initialize CLI menu
        CliMenu menu = new CliMenu();
        menu.start();
    }
}