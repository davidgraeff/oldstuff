package com.scenecontrol.ledcontrol.fragments;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.listadapter.SocketsListAdapter;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

import android.app.ListFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.Toast;

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
		ModelElement item = (ModelElement)adapter.getItem(position);
		Toast.makeText(getActivity(),"Change: " +item.getCacheName(), Toast.LENGTH_SHORT).show();
		String data = "{\"componentid_\":\"scenecontrol.switches\", \"instanceid_\":\"null\", \"type_\":\"execute\", \"method_\":\"toggleSwitch\", \"channel\":\""+item.entryid+"\"}";
		LedApplication.getServerconnection().getConnection().write(data);
	}
}
