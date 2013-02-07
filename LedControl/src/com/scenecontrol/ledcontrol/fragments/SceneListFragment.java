package com.scenecontrol.ledcontrol.fragments;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.listadapter.ProfilesListAdapter;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

import android.app.ListFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ListView;
import android.widget.Toast;

public class SceneListFragment extends ListFragment {
	private ProfilesListAdapter mProfilesListAdapter;
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container,
			Bundle savedInstanceState) {
		ListView v = new ListView(getActivity());
		v.setTextFilterEnabled(true);
		mProfilesListAdapter = new ProfilesListAdapter(getActivity());
		v.setAdapter(mProfilesListAdapter);
		return v;
	}
	
	@Override
	public void onListItemClick(ListView parent, View v, int position, long id) {
		ProfilesListAdapter.ViewHolder vh = (ProfilesListAdapter.ViewHolder)v.getTag();
		ModelElement data = vh.data;
		Toast.makeText(getActivity(),"Execute: " +data.cacheName, Toast.LENGTH_SHORT).show();
		String rdata = "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"runcollection\", \"sceneid_\":\""+data.entryid+"\"}";
		LedApplication.getServerconnection().getConnection().write(rdata);
	}
}
