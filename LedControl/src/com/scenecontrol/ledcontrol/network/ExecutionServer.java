package com.scenecontrol.ledcontrol.network;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.model.ModelAvailableInterface;
import com.scenecontrol.ledcontrol.model.SceneModel;
import com.scenecontrol.ledcontrol.network.ConnectionObserver.ConnectionObserverInterface;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class ExecutionServer extends Handler implements ConnectionObserverInterface {
	private HashMap<String, SceneModel> m_models = new HashMap<String, SceneModel>();
	private TCPClient m_connection;
	private ArrayList<ExecutionServerMessageInterface> mListens = new ArrayList<ExecutionServerMessageInterface>();
	private ArrayList<ModelAvailableInterface> mModelAvailableListens = new ArrayList<ModelAvailableInterface>();

	private void putModel(SceneModel model) {
		m_models.put(model.getID(), model);
		// inform listeners
		Iterator<ModelAvailableInterface> i = mModelAvailableListens.iterator();
		while (i.hasNext()) {
			ModelAvailableInterface o = i.next();
			o.onModelAvailable(model);
		}
	}
	
	@Override
	public void handleMessage(Message msg) {
		switch (msg.what) {
		case TCPClient.DISCONNECTEDCODE:

			Iterator<SceneModel> models = m_models.values().iterator();
			while (models.hasNext()) {
				SceneModel o = models.next();
				models.remove();
				o.clear();
			}
			for (ExecutionServerMessageInterface o : mListens) {
				o.executionServerDisconnected(m_connection, (String) msg.obj);
			}
			break;
		case TCPClient.CONNECTEDCODE:
			{
				for (ExecutionServerMessageInterface o : mListens) {
					o.executionServerConnected(m_connection);
				}
				putModel(new SceneModel("profiles", "id_","categories"));
				putModel(new SceneModel("event", "id_", null));
				
				m_connection.write("{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"version\"}",
						"{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"requestAllProperties\"}",
						"{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"fetchAllDocuments\"}",
						"{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"registerNotifier\"}");
			}
			break;
		case TCPClient.READCODE:
			String msgstr = (String) msg.obj;
			// check if model data is received
			JSONObject json;
			try {
				json = new JSONObject(msgstr);
				if (!json.has("type_") || !json.has("id_"))
					break;
				String type = json.getString("type_");
				String id = json.getString("id_");
				SceneModel model = m_models.get(id);
				if (model != null && type.equals("model.change")) {
					model.change(json);
				} else if (model != null && type.equals("model.remove")) {
					model.remove(json);
				} else if (type.equals("model.reset")) {
					if (model == null) {
						String key = json.getString("key_");
						putModel(new SceneModel(id, key, null));
					}
				} else if (type.equals("notification")) {
					if (id.equals("alldocuments")) {
						JSONArray documents = json.getJSONArray("documents");
						for (int i = 0; i < documents.length(); i++) {
							JSONObject doc = documents.getJSONObject(i);
							if (doc.getString("type_").equals("scene"))
								m_models.get("profiles").change(doc);
							else if (doc.getString("type_").equals("event"))
								m_models.get("event").change(doc);
							//that.documentChanged(doc.documents[i], false);
						}

//						$(that).trigger("onloadcomplete");
					} else if (id.equals("documentChanged")) {
						JSONObject doc = json.getJSONObject("document");
						if (doc.getString("type_").equals("scene"))
							m_models.get("profiles").change(doc);
						else if (doc.getString("type_").equals("event"))
							m_models.get("events").change(doc);

//						that.documentChanged(doc.document, false);
//						that.notifyDocumentChange(doc.document, false);
//						if (doc.document.type_=="action"||doc.document.type_=="event"||doc.document.type_=="condition") {
//							var scenedoc = that.scenes[that.unqiueSceneID(doc.document.sceneid_)]
//							if (scenedoc)
//								that.notifyDocumentChange(scenedoc, false);
//						}
					} else if (id.equals("documentRemoved"))  {
						JSONObject doc = json.getJSONObject("document");
						if (doc.getString("type_").equals("scene"))
							m_models.get("profiles").remove(doc);
						else if (doc.getString("type_").equals("event"))
							m_models.get("event").remove(doc);

//						that.documentChanged(doc.document, true);
//						that.notifyDocumentChange(doc.document, true);
						// update scene
//						if (doc.document.type_=="action"||doc.document.type_=="event"||doc.document.type_=="condition")
//							that.notifyDocumentChange(doc.document.sceneid_, false);
					} else if (id.equals("registerNotifier"))  {
//						console.log("Document notifier registered:", doc.notifierstate);
					} else if (id.equals("plugins") && json.getString("componentid_").equals("PluginController"))  {
//						that.plugins = doc.plugins;
//						$(that).trigger("onplugins");
					} else {
						// inform listeners
						for (ExecutionServerMessageInterface o : mListens) {
							o.executionServerMessage(m_connection, json);
						}
//						console.log("Notification", doc);
					}
				}
			} catch (JSONException e) {
				Log.e("SERVER", e.getMessage());
			}
			break;
		}
	}

	public TCPClient getConnection() {
		return m_connection;
	}

	public SceneModel getModel(String modelid) {
		return m_models.get(modelid);
	}
	
	public void addModelAvailableListener(ModelAvailableInterface o) {
		if (!mModelAvailableListens.contains(o))
			mModelAvailableListens.add(o);
		Iterator<SceneModel> models = m_models.values().iterator();
		while (models.hasNext()) {
			SceneModel model = models.next();
			o.onModelAvailable(model);
		}
	}

	public void removeModelAvailableListener(ModelAvailableInterface o) {
		mModelAvailableListens.remove(o);
	}

	public void addMessageHandler(ExecutionServerMessageInterface o) {
		if (!mListens.contains(o))
			mListens.add(o);
	}

	public void removeMessageHandler(ExecutionServerMessageInterface o) {
		mListens.remove(o);
	}

	public void executeScene(String id, String name) {
		String data = "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"runcollection\", \"sceneid_\":\""+id+"\"}";
		m_connection.write(data);
	}

	public ExecutionServer() {
		super();
		m_connection = new TCPClient(this);
	}
	
	public boolean isConnected() {
		return m_connection != null && m_connection.isConnected();
	}

	@Override
	public void connectityChanged(int state) {
		switch (state) {
		case ConnectionObserver.STATE_CONNECTED:
			m_connection.Connect(LedApplication.getHost(), LedApplication.getPort(), true);
			break;
		case ConnectionObserver.STATE_NOCONNECTION:
			m_connection.CloseConnection();
		}
	}
}
