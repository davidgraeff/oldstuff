package com.scenecontrol.ledcontrol.fragments;

import android.app.ListFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.Toast;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.listadapter.SocketsListAdapter;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

import org.json.JSONException;
import org.json.JSONObject;

public class SocketFragment extends ListFragment {

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ListView listview = new ListView(getActivity());
        listview.setTextFilterEnabled(true);
        setListAdapter(new SocketsListAdapter(getActivity()));
        return listview;
    }

    @Override
    public void onListItemClick(ListView parent, View v, int position, long id) {
        SocketsListAdapter adapter = (SocketsListAdapter) parent.getAdapter();
        ModelElement item = (ModelElement) adapter.getItem(position);
        Toast.makeText(getActivity(), "Change: " + item.getCacheName(), Toast.LENGTH_SHORT).show();
        JSONObject j = new JSONObject();
        try {
            j.put("componentid_", "scenecontrol.switches");
            j.put("instanceid_", "null");
            j.put("type_", "execute");
            j.put("method_", "toggleSwitch");
            j.put("channel", item.entryid);
            LedApplication.getServerconnection().write(j.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}
