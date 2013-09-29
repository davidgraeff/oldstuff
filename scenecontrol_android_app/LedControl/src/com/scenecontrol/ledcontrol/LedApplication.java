package com.scenecontrol.ledcontrol;

import android.app.Application;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Build;

import com.scenecontrol.ledcontrol.network.ConnectionObserver;
import com.scenecontrol.ledcontrol.network.ModelAndNotificationManager;
import com.scenecontrol.ledcontrol.network.SceneProtocol;
import com.scenecontrol.ledcontrol.network.TCPClient;

public class LedApplication extends Application {
    private static Context context;
    private static ModelAndNotificationManager modelmanager;
    private static SceneProtocol sceneprotocol;
    private static TCPClient serverconnection;
    private static ConnectionObserver connectionObserver;
    public static final String PREFS_NAME = "ConfigFile";

    public LedApplication() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        LedApplication.context = getApplicationContext();
        modelmanager = new ModelAndNotificationManager();
        serverconnection = new TCPClient();
        serverconnection.setConnectionData(getHost(), getPort(), getUSeSSL(), modelmanager);
        sceneprotocol = new SceneProtocol();
        serverconnection.addConnectionListener(modelmanager); // for init/resetting the models on connect/disconnect
        serverconnection.addConnectionListener(sceneprotocol); // for doing the handshake on connect
        connectionObserver = new ConnectionObserver();
        connectionObserver.addListener(serverconnection);
        if (Build.FINGERPRINT.startsWith("generic") || Build.FINGERPRINT.equals("unknown")) {
            tryConnect();
        }
    }

    public static void tryConnect() {
        serverconnection.connectityChanged(ConnectionObserver.STATE_CONNECTED);
    }

    public static Context getAppContext() {
        return LedApplication.context;
    }

    public static TCPClient getServerconnection() {
        return serverconnection;
    }

    public static SceneProtocol getSceneProtocol() {
        return sceneprotocol;
    }

    public static ModelAndNotificationManager getModelManager() {
        return modelmanager;
    }

    public static ConnectionObserver getConnectionObserver() {
        return connectionObserver;
    }

    public static boolean getUSeSSL() {
        SharedPreferences settings = context.getSharedPreferences(PREFS_NAME, 0);
        return settings.getBoolean("usessl", true);
    }

    public static String getHost() {
        SharedPreferences settings = context.getSharedPreferences(PREFS_NAME, 0);
        return settings.getString("host", "192.168.1.2");
    }

    public static void setHost(String url) {
        SharedPreferences settings = context.getSharedPreferences(PREFS_NAME, 0);
        Editor editor = settings.edit();
        editor.putString("host", url);
        editor.apply();
    }

    public static int getPort() {
        SharedPreferences settings = context.getSharedPreferences(PREFS_NAME, 0);
        return settings.getInt("port", 3101);
    }

    public static void setPort(int port) {
        SharedPreferences settings = context.getSharedPreferences(PREFS_NAME, 0);
        Editor editor = settings.edit();
        editor.putInt("port", port);
        editor.apply();
    }
}
