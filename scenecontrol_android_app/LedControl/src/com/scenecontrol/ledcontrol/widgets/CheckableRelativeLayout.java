package com.scenecontrol.ledcontrol.widgets;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.Checkable;
import android.widget.RelativeLayout;

public class CheckableRelativeLayout extends RelativeLayout implements
		Checkable {
	private boolean mChecked = false;
	private final List<Checkable> mCheckableViews = new ArrayList<Checkable>();
//
//	@Override
//	protected void onFinishInflate() {
//	    super.onFinishInflate();
//	    findCheckableChildren(this);
//	}
//
//	private void findCheckableChildren(View v) {
//	    if (v instanceof Checkable && v instanceof ViewGroup) {
//	        mCheckableViews.add((Checkable) v);
//	    }
//	    if (v instanceof ViewGroup) {
//	        final ViewGroup vg = (ViewGroup) v;
//	        final int childCount = vg.getChildCount();
//	        for (int i = 0; i < childCount; ++i) {
//	            findCheckableChildren(vg.getChildAt(i));
//	        }
//	    }
//	}
//	
	public CheckableRelativeLayout(Context context) {
		super(context, null);
	}

	public CheckableRelativeLayout(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	private static final int[] CheckedStateSet = { android.R.attr.state_checked };

	@Override
	public void setChecked(boolean checked) {
		mChecked = checked;
		for (Checkable c : mCheckableViews) {
			c.setChecked(checked);
		}
		refreshDrawableState();
	}

	@Override
	public boolean isChecked() {
		return mChecked;
	}

	@Override
	public void toggle() {
		setChecked(!mChecked);
	}

	@Override
	protected int[] onCreateDrawableState(int extraSpace) {
		final int[] drawableState = super.onCreateDrawableState(extraSpace + 1);
		if (isChecked()) {
			mergeDrawableStates(drawableState, CheckedStateSet);
		}
		return drawableState;
	}

}
