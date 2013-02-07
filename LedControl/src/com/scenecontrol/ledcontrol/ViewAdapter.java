package com.scenecontrol.ledcontrol;

import android.app.Fragment;
import android.app.FragmentManager;
import android.content.Context;
import android.support.v13.app.FragmentStatePagerAdapter;

public class ViewAdapter extends FragmentStatePagerAdapter   {
	private final Class<?>[] mContent;
	private final CharSequence[] mContentTitles;
	private Fragment[] mCacheFragments;
	private Context mContext;
	
	@Override
	protected void finalize() throws Throwable {
		super.finalize();
	}
	
	public ViewAdapter(FragmentManager fm, Context c, Class<?>[] classes, CharSequence[] titles) {
		super(fm);
		mContent = classes;
		mContentTitles = titles;
		mContext = c;
		mCacheFragments = new Fragment[classes.length];
	}
	
	@Override
	public Fragment getItem(int position) {
		if (mCacheFragments[position]==null) {
			Fragment m = Fragment.instantiate(mContext, mContent[position].getName());
			mCacheFragments[position] = m;
		}
		return mCacheFragments[position];
	}

    @Override
    public CharSequence getPageTitle(int position) {
        return mContentTitles[position];
    }
    
    public int getItemPosition(Object object) {
        return POSITION_NONE;
    }
    
	@Override
	public int getCount() {
		return mContent.length;
	}
}
