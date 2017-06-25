package edu.ufrgs.primegen;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by lucas on 6/24/17.
 */

public class Manager {

    private Connection connection;
    private ConsoleHandler consoleHandler = null;

    public Manager(String ipAddress, int port) throws IOException {
        connection = new Connection(ipAddress, port);
    }

    public void setConsoleHandler(ConsoleHandler consoleHandler) {
        this.consoleHandler = consoleHandler;
    }

    public void runSingleThread() throws IOException, InterruptedException, JSONException {
        String blockString = connection.requestBlock();
        JSONObject blockJson = new JSONObject(blockString);
        consoleHandler.PrintConsole("PROCESS STARTED [BLOCK " + blockJson.getString("block_start") + " - " + blockJson.getString("block_end") + "]");
        Block block = new Block(blockJson.getString("block_start"), blockJson.getString("block_end"));
        List<String> result = block.generateResult();
        consoleHandler.PrintConsole("PROCESS FINISHED [BLOCK " + blockJson.getString("block_start") + " - " + blockJson.getString("block_end") + "]");
        connection.blockResult(blockJson.getString("id"), blockJson.getString("block_start"), blockJson.getString("block_end"), result);
    }

    public void runMultiThread() throws InterruptedException {
        int numCores = Runtime.getRuntime().availableProcessors();
        List<Thread> threads = new ArrayList<>(numCores);
        for (int i = 0; i < numCores; i++) {
            threads.add(new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        runSingleThread();
                    } catch (IOException | InterruptedException | JSONException e) {
                        consoleHandler.PrintConsole(e.getMessage());
                    }
                }
            }));
            threads.get(i).start();
        }
        for (Thread t : threads) {
            t.join();
        }
    }
}
