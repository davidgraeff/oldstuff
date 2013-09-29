package com.scenecontrol.ledcontrol.model;

import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.TreeSet;

public class SceneModel {
    public class ModelElement {
        private JSONObject values;

        public synchronized JSONObject getValues() {
            return values;
        }

        public String entryid; // for removing an object
        public String sortid; // for determining the correct place in the array
        public String cacheName = null;
        public TreeSet<String> categories = new TreeSet<String>();

        public String getCacheName() {
            return cacheName;
        }

        public ModelElement(JSONObject values, String entryid, String sortid, String cacheName) {
            this.entryid = entryid;
            this.sortid = sortid;
            this.values = values;
            this.cacheName = cacheName;
        }

        // Section header
        public ModelElement(String entryid, String sortid) {
            this.values = null;
            this.entryid = entryid;
            this.sortid = sortid;
        }
    }

    ;

    final ArrayList<ModelElement> m_entries = new ArrayList<ModelElement>();
    final String m_modelid;
    final String m_key;
    final String m_category_element_id;
    final String m_name_element_id = "name";
    String m_suppressChangesID = null;

    private ArrayList<ModelChangeListener> mChangeListener = new ArrayList<ModelChangeListener>();

    public void addChangeListener(ModelChangeListener listener) {
        if (mChangeListener.contains(listener))
            return;
        mChangeListener.add(listener);
        if (m_entries.size() > 0)
            listener.onModelDataChange();
    }

    public void removeChangeListener(ModelChangeListener listener) {
        mChangeListener.remove(listener);
    }

    public String getID() {
        return m_modelid;
    }

    public SceneModel(String id, String key, String category_element_id) {
        m_modelid = id;
        m_key = key;
        m_category_element_id = category_element_id;
    }

    private void changeWithoutCat(JSONObject json) {
        String entryid = json.optString(m_key);
        String name = json.optString(m_name_element_id, entryid);

        insertPositionResult r = getInsertPosition(name);
        if (r.found) {
            ModelElement m = m_entries.get(r.pos);
            m.values = json;
            m.cacheName = name;
        } else {
            ModelElement m = new ModelElement(json, entryid, name, name);
            m_entries.add(r.pos, m);
        }

        if (!entryid.equals(m_suppressChangesID)) {
            for (ModelChangeListener o : mChangeListener) {
                o.onModelDataChange();
            }
        }
    }

    private class insertPositionResult {
        public int pos;
        public boolean found;

        public insertPositionResult(int pos, boolean found) {
            this.pos = pos;
            this.found = found;
        }
    }

    private insertPositionResult getInsertPosition(String cmpname) {
        for (int entryPosition = 0; entryPosition < m_entries.size(); ++entryPosition) {
            ModelElement element = m_entries.get(entryPosition);
            int cmp = element.sortid.compareTo(cmpname);
            if (cmp == 0) {
                return new insertPositionResult(entryPosition, true);
            } else if (cmp > 0) {
                return new insertPositionResult(entryPosition, false);
            }
        }
        return new insertPositionResult(m_entries.size(), false);
    }

    private void insertCategory(String catname, String catid, int position) {
        ModelElement e = new ModelElement("", catid);
        e.cacheName = catname;
        m_entries.add(position, e);
    }

    public synchronized void change(JSONObject json) {
        if (!json.has(m_key))
            return;
        JSONArray categoriesJSON = json.optJSONArray(m_category_element_id);
        if (categoriesJSON == null) {
            changeWithoutCat(json);
            return;
        }
        String entryid = json.optString(m_key);
        String name = json.optString(m_name_element_id, entryid);

        // convert categories to an array
        TreeSet<String> categories = new TreeSet<String>();
        for (int i = 0; i < categoriesJSON.length(); ++i) {
            String catid = categoriesJSON.optString(i);
            categories.add(catid);
            insertPositionResult r = getInsertPosition(catid);
            if (!r.found)
                insertCategory(catid, catid, r.pos);
        }
        if (categories.size() == 0) {
            final String catname = "Unkategorisiert";
            insertPositionResult r = getInsertPosition(catname);
            if (!r.found)
                insertCategory(catname, ".", 0);
            categories.add(".");
        }

        // Remove previous entries with this id
        remove(entryid);

        // for every category -> find correct position and insert
        Iterator<String> catIterator = categories.iterator();
        String sortid;
        while (catIterator.hasNext()) {
            sortid = catIterator.next() + name;
            insertPositionResult r = getInsertPosition(sortid);
            if (r.found) {
                Log.e("MODEL", "Found but should be deleted");
            }
            ModelElement m = new ModelElement(json, entryid, sortid, name);
            m_entries.add(r.pos, m);
        }

        if (!entryid.equals(m_suppressChangesID)) {
            for (ModelChangeListener o : mChangeListener) {
                o.onModelDataChange();
            }
        }
    }

    public synchronized void remove(JSONObject json) {
        if (!json.has(m_key))
            return;

        if (remove(json.optString(m_key))) {
            for (ModelChangeListener o : mChangeListener) {
                o.onModelDataChange();
            }
        }
    }

    private boolean remove(String entryid) {
        int countOps = 0;
        Iterator<ModelElement> i = m_entries.iterator();
        while (i.hasNext()) {
            ModelElement element = i.next();
            if (element.entryid.equals(entryid)) {
                i.remove();
                ++countOps;
            }
        }
        return countOps > 0;
    }

    public synchronized void clear() {
        m_entries.clear();
        ArrayList<ModelChangeListener> copy = new ArrayList<ModelChangeListener>(mChangeListener);
        for (ModelChangeListener o : copy) {
            o.onClear();
        }
    }

    public int getCount() {
        return m_entries.size();
    }

    public ModelElement getItem(int index) {
        return m_entries.get(index);
    }

    public void suppressChanges(String channelid, boolean suppress) {
        m_suppressChangesID = suppress ? channelid : null;
    }
}
