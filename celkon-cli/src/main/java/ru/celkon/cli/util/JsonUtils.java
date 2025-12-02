package ru.celkon.cli.util;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.SerializationFeature;

public class JsonUtils {

    private static final ObjectMapper prettyMapper = new ObjectMapper()
            .enable(SerializationFeature.INDENT_OUTPUT);

    public static String pretty(Object obj) {
        try {
            return prettyMapper.writeValueAsString(obj);
        } catch (Exception e) {
            return "<invalid json: " + e.getMessage() + ">";
        }
    }

    public static void printPretty(Object obj) {
        System.out.println(pretty(obj));
    }
}
