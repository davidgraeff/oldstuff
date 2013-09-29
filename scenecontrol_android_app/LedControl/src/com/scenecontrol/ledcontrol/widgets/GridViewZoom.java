package com.scenecontrol.ledcontrol.widgets;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.ViewGroup;
import android.widget.GridView;
import android.widget.ImageView;

public class GridViewZoom extends GridView {
	private ScaleGestureDetector mScaleDetector;
	private float mScaleFactor = 1.f;
	private int cW = 0;
	
	private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
	    @Override
	    public boolean onScale(ScaleGestureDetector detector) {
	    	if (cW==0)
	    		cW = GridViewZoom.this.getColumnWidth();
	        mScaleFactor *= detector.getScaleFactor();

	        // Don't let the object get too small or too large.
	        mScaleFactor = Math.max(0.1f, Math.min(mScaleFactor, 5.0f));
	        
	        GridViewZoom.this.setColumnWidth((int) (cW*mScaleFactor));
	        
//	        LayoutInflater mInflater = LayoutInflater.from(GridViewZoom.this.getContext());
//	        View v = mInflater.inflate(R.layout.rowfavorite, GridViewZoom.this, false);
//	        
	        for (int i=GridViewZoom.this.getChildCount()-1;i>=0;--i) {
	        	CheckableRelativeLayout lView = (CheckableRelativeLayout) GridViewZoom.this.getChildAt(i);
	        	for (int j=lView.getChildCount()-1;j>=0;--j) {
	        		View iView = lView.getChildAt(j);
		        	if (iView.getClass().equals(ImageView.class)) {
				        ViewGroup.LayoutParams params = iView.getLayoutParams();
				        params.width = params.height = (int) (cW * mScaleFactor);
				        iView.setLayoutParams(params);
				        iView.requestLayout();
		        	}
	        	}
	        }

	        //GridViewZoom.this.requestLayout();
	        invalidate();
	        return true;
	    }
	}
	
	public GridViewZoom(Context context) {
		super(context);
		mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
	}

	public GridViewZoom(Context context, AttributeSet attrs) {
		super(context, attrs);
		mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
	}

	public GridViewZoom(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
	}

	@Override
	public boolean onTouchEvent(MotionEvent ev) {
		super.onTouchEvent(ev);
	    // Let the ScaleGestureDetector inspect all events.
	    mScaleDetector.onTouchEvent(ev);
	    return true;
	}
}
