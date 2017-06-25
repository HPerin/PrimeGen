package edu.ufrgs.primegen;

import java.io.BufferedReader;
import java.io.DataOutput;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.List;

/**
 * Created by lucas on 6/24/17.
 */

public class Connection {

    private Socket socket;
    private DataOutputStream outputStream;
    private BufferedReader bufferedReader;

    public Connection(String address, int port) throws IOException {
        socket = new Socket(address, port);
        outputStream = new DataOutputStream(socket.getOutputStream());
        bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
    }

    public synchronized String requestBlock() throws IOException, InterruptedException {
        String outMessage = "{\"type\":\"BLOCK_REQUEST\"}$";
        outputStream.writeBytes(outMessage);
        return readUntil('$');
    }

    public synchronized String blockResult(String id, String blockStart, String blockEnd, List<String> data) throws IOException, InterruptedException {
        String outMessage = "{\"type\":\"BLOCK_RESULT\",\"id\":\"" + id + "\",\"block_start\":\"" + blockStart + "\",\"block_end\":\"" + blockEnd + "\",\"data\":[";
        for (int i = 0; i < data.size(); i++) {
            if (i != 0) outMessage += ",";
            outMessage += "\"" + data.get(i) + "\"";
        }
        outMessage += "]}$";
        outputStream.writeBytes(outMessage);
        return readUntil('$');
    }

    private String readUntil(char separator) throws IOException, InterruptedException {
        StringBuilder stringBuilder = new StringBuilder();
        int ch;
        while(true) {
            ch = bufferedReader.read();
            if (ch == separator) {
                return stringBuilder.toString();
            } else if (ch == -1) {
                Thread.sleep(1);
            } else {
                stringBuilder.append((char) ch);
            }
        }
    }
}
