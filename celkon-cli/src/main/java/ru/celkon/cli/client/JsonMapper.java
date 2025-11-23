package ru.celkon.cli.client;

import com.fasterxml.jackson.databind.ObjectMapper;

public class JsonMapper {

    private static final ObjectMapper mapper = new ObjectMapper();

    public static ObjectMapper get() {
        return mapper;
    }
}
