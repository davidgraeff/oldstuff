package com.scenecontrol.ledcontrol.network;

import android.os.Handler;
import android.os.Message;

import java.util.ArrayList;

public class TCPClient extends Handler implements ConnectionObserver.ConnectionObserverInterface {
    private TCPClientThread m_TCPClientThread = null;
    private ArrayList<ConnectDisconnectInterface> mConnectionListener = new ArrayList<ConnectDisconnectInterface>();
    public static final int DISCONNECTEDCODE = 2;
    public static final int CONNECTEDCODE = 3;

    private String m_server;
    private int m_port;
    private boolean m_useSSL;
    private JSONMessageArrivedInterface m_receiveObject = null;

    @Override
    public void handleMessage(Message msg) {
        switch (msg.what) {
            case TCPClient.DISCONNECTEDCODE:
                for (ConnectDisconnectInterface o : mConnectionListener) {
                    o.executionServerDisconnected();
                }
                break;
            case TCPClient.CONNECTEDCODE: {
                for (ConnectDisconnectInterface o : mConnectionListener) {
                    o.executionServerConnected();
                }
            }
            break;
        }
    }

    public void addConnectionListener(ConnectDisconnectInterface o) {
        if (!mConnectionListener.contains(o))
            mConnectionListener.add(o);
    }

    public void removeConnectionListener(ConnectDisconnectInterface o) {
        mConnectionListener.remove(o);
    }

    public void setConnectionData(String server, int port, boolean useSSL, JSONMessageArrivedInterface receiveObject) {
        m_server = server;
        m_port = port;
        m_useSSL = useSSL;
        m_receiveObject = receiveObject;
    }

    public void Connect() {
        if (m_TCPClientThread == null && m_server != null && m_receiveObject != null) {
            m_TCPClientThread = new TCPClientThread(m_server, m_port, m_useSSL, this, m_receiveObject);

            Thread thread = new Thread(m_TCPClientThread);
            thread.start();
        }
    }

    public void CloseConnection() {
        if (m_TCPClientThread != null) {
            m_TCPClientThread.closeConnection();
            while (m_TCPClientThread.isConnected()) {

            }
            m_TCPClientThread = null;
        }
    }

    public void write(String... datas) {
        if (m_TCPClientThread != null) {
            m_TCPClientThread.write(datas);
        }
    }

    public boolean isConnected() {
        if (m_TCPClientThread == null)
            return false;
        return m_TCPClientThread.isConnected();
    }

    @Override
    public void connectityChanged(int state) {
        switch (state) {
            case ConnectionObserver.STATE_CONNECTED:
                Connect();
                break;
            case ConnectionObserver.STATE_NOCONNECTION:
                CloseConnection();
        }
    }
}