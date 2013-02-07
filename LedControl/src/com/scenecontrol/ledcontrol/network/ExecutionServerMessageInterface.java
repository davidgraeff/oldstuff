package com.scenecontrol.ledcontrol.network;

import org.json.JSONObject;


public interface ExecutionServerMessageInterface {
	void executionServerMessage(TCPClient client, JSONObject json);
	void executionServerDisconnected(TCPClient client, String reason);
	void executionServerConnected(TCPClient client);
}
