package com.scenecontrol.ledcontrol.fragments;

import android.app.Fragment;
import android.os.Bundle;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.ViewAdapter;
import com.viewpagerindicator.TitlePageIndicator;

public class TabsScenesFragment extends Fragment {
	@Override
	public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
		View v = inflater.inflate(R.layout.tabslayout, container, false);
		ViewPager pager = (ViewPager) v.findViewById(R.id.pager);
		ViewAdapter mViewAdapter = new ViewAdapter(getFragmentManager(), getActivity(),
				new Class<?>[] {FavouritesFragment.class,AlarmListFragment.class,SceneListFragment.class},
				new CharSequence[] {"Favouriten","Alarm","Alle Szenen"});
		pager.setAdapter(mViewAdapter);
		TitlePageIndicator titleIndicator = (TitlePageIndicator) v.findViewById(R.id.indicator);
		titleIndicator.setViewPager(pager);
		return v;
	}
}
