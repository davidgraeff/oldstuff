package com.scenecontrol.ledcontrol.network;

import android.os.AsyncTask;
import android.os.Message;
import android.util.Log;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;

import org.apache.http.conn.ssl.SSLSocketFactory;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.security.KeyStore;

import javax.net.ssl.SSLSocket;

/**
 * Created by david on 13.06.13.
 */
public class TCPClientThread implements Runnable {
    // connection data
    private String m_server;
    private int m_port;
    private boolean m_useSSL;
    private JSONMessageArrivedInterface receiveObject = null;
    // propagate connect/disconnect messages to the tcpclient/gui thread
    private TCPClient m_tcpclient = null;
    // internal network variables
    private Socket client = null;
    private BufferedReader in;
    private PrintWriter out;
    private boolean inCleanup = false;

    private class SocketSendThread extends AsyncTask<String, Void, Void> {
        @Override
        protected Void doInBackground(String... datas) {
            for (String data : datas) {
                if (out == null || out.checkError()) {
                    closeConnection();
                    break;
                }
                out.write(data + "\n");
                out.flush();
            }
            if (out == null || out.checkError()) {
                closeConnection();
            }
            return null;
        }
    }

    private class SocketCloseThread extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... datas) {
            if (inCleanup)
                return null;
            inCleanup = true;
            try {
                if (client != null) {
                    client.close();
                }
            } catch (IOException e) {
            }
            try {
                if (in != null) {
                    in.close();
                }
            } catch (IOException e) {
            }
            if (out != null)
                out.close();
            client = null;
            in = null;
            out = null;
            inCleanup = false;

            Message msg = Message.obtain();
            msg.what = TCPClient.DISCONNECTEDCODE;
            m_tcpclient.sendMessage(msg);

            return null;
        }
    }

    public TCPClientThread(String server, int port, boolean useSSL, TCPClient tcpclient, JSONMessageArrivedInterface receiveObject) {
        m_server = server;
        m_port = port;
        m_useSSL = useSSL;
        m_tcpclient = tcpclient;
        this.receiveObject = receiveObject;
    }

    public void closeConnection() {
        new SocketCloseThread().execute();
    }

    public void write(String... datas) {
        if (isConnected())
            new SocketSendThread().execute(datas);
    }

    public boolean isConnected() {
        return client != null && client.isConnected();
    }

    @Override
    public void run() {
        // Establish the remote endpoint for the socket.
        InetAddress serverAddr = null;

        try {
            serverAddr = InetAddress.getByName(m_server);
        } catch (UnknownHostException e) {
            e.printStackTrace();
            return;
        }

        SocketAddress sockaddr = new InetSocketAddress(serverAddr, m_port);

        // Create a TCP/IP socket and connect to the server.
        try {
            if (m_useSSL) {
                KeyStore localTrustStore = KeyStore.getInstance("BKS");
                InputStream in = LedApplication.getAppContext().getResources().openRawResource(R.raw.truststore);
                localTrustStore.load(in, "".toCharArray());
                SSLSocketFactory sslSocketFactory = new SSLSocketFactory(localTrustStore);
                client = sslSocketFactory.createSocket();
                SSLSocket s = (SSLSocket) client;
                s.setUseClientMode(true);
                s.setEnableSessionCreation(true);
                s.connect(sockaddr, 2000);
                //s.startHandshake();
            } else {
                client = new Socket();
                client.connect(sockaddr, 2000);
            }

            out = new PrintWriter(client.getOutputStream(), true);
            in = new BufferedReader(new InputStreamReader(client.getInputStream()));

        } catch (SocketTimeoutException e) {
            closeConnection();
            return;
        } catch (Exception e) {
            closeConnection();
            return;
        }

        Message msg = Message.obtain();
        msg.obj = null;
        msg.what = TCPClient.CONNECTEDCODE;
        m_tcpclient.sendMessage(msg);

        String data;
        while (!out.checkError()) {
            try {
                data = in.readLine();
                if (data == null)
                    continue;
            } catch (IOException e) {
                break;
            }

            if (receiveObject == null)
                continue;

            JSONObject json;
            try {
                json = new JSONObject(data);
                receiveObject.jsonMessageArrived(json);
            } catch (JSONException e) {
                Log.e("SERVER", e.getMessage());
            }
        }
        closeConnection();
    }
}
