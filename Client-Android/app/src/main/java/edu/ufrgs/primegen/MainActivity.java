package edu.ufrgs.primegen;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;

import org.json.JSONException;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    private Switch startSwitch;
    private TextView consoleText;
    private EditText ipAddress;
    private EditText port;

    private Runner runner;
    private static final Integer lock = 0;

    class Runner implements Runnable {

        boolean running = false;
        String ip = "";
        int port = 0;

        @Override
        public void run() {
            boolean shouldWait = true;
            while(shouldWait) {
                boolean localRunning;
                synchronized (lock) {
                    localRunning = running;
                }
                if (localRunning) {
                    try {
                        Manager manager;
                        synchronized (lock) {
                            manager = new Manager(ip, port);
                        }
                        manager.setConsoleHandler(new ConsoleHandler() {
                            @Override
                            public void PrintConsole(final String s) {
                                consoleText.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        consoleText.append(s + "\n");
                                    }
                                });
                            }
                        });
                        manager.runMultiThread();
                    } catch (IOException | InterruptedException e) {
                        running = false;
                        consoleText.post(new Runnable() {
                            @Override
                            public void run() {
                                consoleText.append(e.getMessage() + "\n");
                            }
                        });
                        startSwitch.post(new Runnable() {
                            @Override
                            public void run() {
                                startSwitch.setEnabled(true);
                                startSwitch.setChecked(false);
                            }
                        });
                    }
                } else {
                    try {
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        shouldWait = false;
                    }
                }
            }
        }

        public void start(String ip, int port) {
            synchronized (lock) {
                if (!running) {
                    this.ip = ip;
                    this.port = port;
                    running = true;
                }
            }
        }

        public void stop() {
            synchronized (lock) {
                running = false;
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        startSwitch = (Switch) findViewById(R.id.startSwitch);
        consoleText = (TextView) findViewById(R.id.console);
        consoleText.setMovementMethod(new ScrollingMovementMethod());
        ipAddress = (EditText) findViewById(R.id.ipAddress);
        port = (EditText) findViewById(R.id.port);

        runner = new Runner();
        new Thread(runner).start();

        startSwitch.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (startSwitch.isChecked()) {
                    int tryPortValue = 0;
                    try {
                        tryPortValue = Integer.parseInt(port.getText().toString());
                    } catch (NumberFormatException e) {
                        consoleText.append("INVALID PORT" + "\n");
                        startSwitch.post(new Runnable() {
                            @Override
                            public void run() {
                                startSwitch.setEnabled(true);
                                startSwitch.setChecked(false);
                            }
                        });
                        return;
                    }
                    final String ip = ipAddress.getText().toString();
                    final int portValue = tryPortValue;
                    consoleText.append("USING IP/PORT: " + ip + ":" + String.valueOf(portValue) + "\n");
                    int numCores = Runtime.getRuntime().availableProcessors();
                    consoleText.append("RUNNING WITH " + String.valueOf(numCores) + " THREADS/CORES" + "\n");
                    runner.start(ip, portValue);
                } else {
                    runner.stop();
                }
            }
        });
    }
}
