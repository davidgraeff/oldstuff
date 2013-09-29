package com.scenecontrol.ledcontrol.fragments;

import android.app.ListFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.listadapter.LedValueListener;
import com.scenecontrol.ledcontrol.listadapter.LedsListAdapter;

import org.json.JSONException;
import org.json.JSONObject;

public class LedsFragment extends ListFragment implements LedValueListener {
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        ListView listview = new ListView(getActivity());
        listview.setTextFilterEnabled(true);
        LedsListAdapter adapter = new LedsListAdapter(getActivity());
        setListAdapter(adapter);
        adapter.setPositionListener(this);
        return listview;
    }

    @Override
    public void onPositionChange(String channelid, int value) {
        //Toast.makeText(getActivity(),"Change: '" +channelid + "'",Toast.LENGTH_SHORT).show();
        JSONObject j = new JSONObject();
        try {
            j.put("componentid_", "scenecontrol.leds");
            j.put("instanceid_", "null");
            j.put("type_", "execute");
            j.put("method_", "setLed");
            j.put("fade", 0);
            j.put("channel", channelid);
            j.put("value", Integer.valueOf(value).toString());
            LedApplication.getServerconnection().write(j.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}