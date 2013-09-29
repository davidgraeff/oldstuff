package com.scenecontrol.ledcontrol.fragments;

import com.scenecontrol.ledcontrol.listadapter.ProfilesListAdapter;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

import android.app.DialogFragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;

public class SceneSelectDialogFragment extends DialogFragment implements OnItemClickListener   {
	public interface SceneSelectedListener {
		void sceneSelected(String sceneid, String scenename);
	}

	SceneSelectedListener mSceneSelectedListener = null;
	
	private ProfilesListAdapter mProfilesListAdapter;
	
	public void setSceneSelectedListener(SceneSelectedListener a) {
		mSceneSelectedListener = a;
	}
	
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
    	ListView listview = new ListView(getActivity());
		listview.setTextFilterEnabled(true);
		mProfilesListAdapter = new ProfilesListAdapter(getActivity());
		listview.setAdapter(mProfilesListAdapter);
		listview.setOnItemClickListener(this);
		getDialog().setTitle("Wähle ein Profil");
		return listview;
	}

	@Override
	public void onItemClick(AdapterView<?> arg0, View v, int position, long id) {
		ProfilesListAdapter.ViewHolder vh = (ProfilesListAdapter.ViewHolder)v.getTag();
		ModelElement data = vh.data;
		if (mSceneSelectedListener != null)
			mSceneSelectedListener.sceneSelected(data.entryid, data.cacheName);
		dismiss();
	}
}
