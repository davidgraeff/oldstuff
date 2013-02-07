package com.scenecontrol.ledcontrol.fragments;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.listadapter.LedsListAdapter;
import com.scenecontrol.ledcontrol.listadapter.LedValueListener;

import android.app.ListFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;

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
		String data = "{\"componentid_\":\"scenecontrol.leds\", \"instanceid_\":\"null\", \"type_\":\"execute\", \"method_\":\"setLed\", \"fade\":0, \"channel\":\""+channelid+"\", \"value\":"+Integer.valueOf(value).toString()+"}";
		LedApplication.getServerconnection().getConnection().write(data);
	}
}