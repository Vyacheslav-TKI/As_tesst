package ru.celkon.cli.ui;

import java.util.Scanner;

public class InputReader {

    private final Scanner scanner = new Scanner(System.in);

    public String readLine(String label) {
        System.out.print(label);
        return scanner.nextLine().trim();
    }

    public int readInt(String label) {
        while (true) {
            System.out.print(label);
            String text = scanner.nextLine().trim();
            try {
                return Integer.parseInt(text);
            } catch (NumberFormatException e) {
                System.out.println("Invalid number, try again.");
            }
        }
    }
}
