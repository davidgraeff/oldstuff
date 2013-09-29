package com.scenecontrol.ledcontrol.network;

import org.json.JSONObject;

/**
 * Created by david on 13.06.13.
 */
public interface NotificationListenerInterface {
    void notification(JSONObject data);
}
