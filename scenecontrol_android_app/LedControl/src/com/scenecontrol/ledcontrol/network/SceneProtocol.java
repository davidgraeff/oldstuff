package com.scenecontrol.ledcontrol.network;

import com.scenecontrol.ledcontrol.LedApplication;

import org.json.JSONException;
import org.json.JSONObject;

/**
 * Created by david on 13.06.13.
 */
public class SceneProtocol implements ConnectDisconnectInterface {
    public void requestProperty(String id, String componentid, String componentinstance) {
        JSONObject j = new JSONObject();
        try {
            j.put("componentid_", "server");
            j.put("type_", "execute");
            j.put("type_", "execute");
            j.put("method_", "requestProperty");
            j.put("type_", "execute");
        } catch (JSONException e) {

        }
        LedApplication.getServerconnection().write(j.toString());
    }

    public void executeScene(String id) {
        String data = "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"runcollection\", \"sceneid_\":\"" + id + "\"}";
        LedApplication.getServerconnection().write(data);
    }

    @Override
    public void executionServerDisconnected() {

    }

    @Override
    public void executionServerConnected() {
        LedApplication.getServerconnection().write("{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"version\"}",
                "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"requestAllProperties\"}",
                "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"fetchAllDocuments\"}",
                "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"registerNotifier\"}");
    }
}
