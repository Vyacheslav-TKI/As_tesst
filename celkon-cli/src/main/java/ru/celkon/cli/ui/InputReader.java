package ru.celkon.cli.ui;

import java.util.Scanner;

public class InputReader {

    private final Scanner scanner = new Scanner(System.in);

    public String readLine(String label) {
        System.out.print(label);
        return scanner.nextLine();
    }
}
