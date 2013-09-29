package com.scenecontrol.ledcontrol.network;

import com.scenecontrol.ledcontrol.model.ModelAvailableInterface;
import com.scenecontrol.ledcontrol.model.SceneModel;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

public class ModelAndNotificationManager implements JSONMessageArrivedInterface, ConnectDisconnectInterface {
    private HashMap<String, SceneModel> m_models = new HashMap<String, SceneModel>();
    private ArrayList<NotificationListenerInterface> mNotificationListener = new ArrayList<NotificationListenerInterface>();
    private ArrayList<ModelAvailableInterface> mModelAvailableListens = new ArrayList<ModelAvailableInterface>();
	public static final String MODEL_EVENTS = "event";
    public static final String MODEL_PROFILES = "scene";

    private void putModel(SceneModel model) {
		m_models.put(model.getID(), model);
		// inform listeners
		Iterator<ModelAvailableInterface> i = mModelAvailableListens.iterator();
		while (i.hasNext()) {
			ModelAvailableInterface o = i.next();
			o.onModelAvailable(model);
		}
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

    public void addNotificationListener(NotificationListenerInterface o) {
        if (!mNotificationListener.contains(o))
            mNotificationListener.add(o);
    }

    public void removeNotificationListener(NotificationListenerInterface o) {
        mNotificationListener.remove(o);
    }

    @Override
    public void jsonMessageArrived(JSONObject json) {
        if (!json.has("type_") || !json.has("id_"))
            return;
        String type = json.optString("type_");
        String id = json.optString("id_");
        SceneModel model = m_models.get(id);
        if (model != null && type.equals("model.change")) {
            model.change(json);
        } else if (model != null && type.equals("model.remove")) {
            model.remove(json);
        } else if (type.equals("model.reset")) {
            if (model == null) {
                String key = json.optString("key_");
                putModel(new SceneModel(id, key, null));
            } else {
                model.clear();
            }
        } else if (type.equals("notification")) {
            if (id.equals("alldocuments")) {
                JSONArray documents = json.optJSONArray("documents");
                for (int i = 0; i < documents.length(); i++) {
                    JSONObject doc = documents.optJSONObject(i);
                    model = m_models.get(doc.optString("type_"));
                    if (model != null)
                        model.change(doc);
                }
            } else if (id.equals("documentChanged")) {
                JSONObject doc = json.optJSONObject("document");
                model = m_models.get(doc.optString("type_"));
                if (model != null)
                    model.change(doc);

            } else if (id.equals("documentRemoved")) {
                JSONObject doc = json.optJSONObject("document");
                model = m_models.get(doc.optString("type_"));
                if (model != null)
                    model.remove(doc);

            } else {
                // inform listeners
                for (NotificationListenerInterface o : mNotificationListener) {
                    o.notification(json);
                }
            }
        }
    }

    @Override
    public void executionServerDisconnected() {
        Iterator<SceneModel> models = m_models.values().iterator();
        while (models.hasNext()) {
            SceneModel o = models.next();
            models.remove();
            o.clear();
        }
    }

    @Override
    public void executionServerConnected() {
        putModel(new SceneModel(MODEL_PROFILES, "id_", "categories"));
        putModel(new SceneModel(MODEL_EVENTS, "id_", null));
    }
}
