public class CelkonClient {

    private final TLSConnectionFactory tlsFactory = new TLSConnectionFactory();

    public <T> T send(Envelope<?> env, Class<T> responseClass) {
        try {
            SSLSocket socket = tlsFactory.createSocket("127.0.0.1", 8888);

            // Serialize JSON
            String json = JsonMapper.get().writeValueAsString(env);

            // Send
            socket.getOutputStream().write((json + "\n").getBytes());

            // Receive
            BufferedReader br = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String resp = br.readLine();

            return JsonMapper.get().readValue(resp, responseClass);

        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
