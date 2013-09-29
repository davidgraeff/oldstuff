package com.scenecontrol.ledcontrol.listadapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.RectF;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.ModelChangeListener;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;
import com.scenecontrol.ledcontrol.model.SceneModelAlarmProxy.AlarmInfo;
import com.scenecontrol.ledcontrol.model.SceneModelAlarmProxy;

public class ScenesForAlarmAdapter extends BaseAdapter implements ModelChangeListener, ListAdapter {
	private LayoutInflater mInflater;
	private SceneModelAlarmProxy m_proxy;
	private Bitmap mIconCal, mIconTime, mNo;
	
	public ScenesForAlarmAdapter(Context context) {
		super();
		mInflater = LayoutInflater.from(context);
		Bitmap compareB = BitmapFactory.decodeResource(context.getResources(), R.drawable.alarmcal);
		Bitmap bmp = Bitmap.createBitmap(compareB.getWidth(), compareB.getHeight(), Bitmap.Config.ARGB_8888) ;
		{
			mIconCal = bmp.copy(bmp.getConfig(), true);
			Canvas c = new Canvas( mIconCal );
			Paint bgPaint = new Paint();
			bgPaint.setARGB(128, 255, 255, 255);
			c.drawRoundRect(new RectF(0,0,bmp.getWidth(),bmp.getHeight()), 15.0f, 15.0f, bgPaint);
			c.drawBitmap( BitmapFactory.decodeResource(context.getResources(), R.drawable.alarmcal), 0,0, null );
		}
		{
			mIconTime = bmp.copy(bmp.getConfig(), true);
			Canvas c = new Canvas( mIconTime );
			Paint bgPaint = new Paint();
			bgPaint.setARGB(128, 255, 255, 255);
			c.drawRoundRect(new RectF(0,0,bmp.getWidth(),bmp.getHeight()), 15.0f, 15.0f, bgPaint);
			c.drawBitmap( BitmapFactory.decodeResource(context.getResources(), R.drawable.alarmtime), 0,0, null );
		}
		{
			mNo = bmp.copy(bmp.getConfig(), true);
			Canvas c = new Canvas( mNo );
			Paint bgPaint = new Paint();
			bgPaint.setARGB(128, 255, 255, 255);
			c.drawRoundRect(new RectF(0,0,bmp.getWidth(),bmp.getHeight()), 15.0f, 15.0f, bgPaint);
			c.drawBitmap( BitmapFactory.decodeResource(context.getResources(), R.drawable.alarmno), 0,0, null );
		}
		m_proxy = new SceneModelAlarmProxy();
		m_proxy.addChangeListener(this);
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		if (m_proxy == null)
			return convertView;
		
		ModelElement data = m_proxy.getItem(position);
		if (convertView == null) {
			ViewHolder vh = new ViewHolder();
			if (data.entryid.isEmpty()) {
				convertView = mInflater.inflate(R.layout.rowcategory, null);
				vh.title = (TextView) convertView.findViewById(R.id.list_item_section_text);
				vh.isHeader = true;
			} else {
				convertView = mInflater.inflate(R.layout.rowalarmprofile, null);
				vh.title = (TextView)convertView.findViewById(R.id.profile_title);
				vh.subtitle = (TextView)convertView.findViewById(R.id.profile_subtitle);
				vh.favicon = (ImageView) convertView.findViewById(R.id.profile_fav);
				vh.isHeader = false;
			}
			convertView.setTag(vh);
		}

		ViewHolder vh = (ViewHolder)convertView.getTag();
		vh.data = data;
		vh.title.setText(data.getCacheName());
		if (!vh.isHeader) {
			AlarmInfo i = m_proxy.getAlarmInfo(position);
			vh.subtitle.setText(i.alarmtime);
			switch (i.alarmtype) {
			case NoAlarm:
				vh.favicon.setImageBitmap(mNo);
				break;
			case OneTimeAlarm:
				vh.favicon.setImageBitmap(mIconCal);
				break;
			case PeriodicAlarm:
				vh.favicon.setImageBitmap(mIconTime);
				break;
			default:
				vh.favicon.setImageBitmap(mNo);
				break;
			}
		}

		return convertView;
	}

	public static class ViewHolder {
	    ImageView favicon;
	    TextView title;
	    TextView subtitle;
	    public ModelElement data;
	    boolean isHeader;
	}

	@Override
	public void onModelDataChange() {
		notifyDataSetChanged();
	}

	@Override
	public void onClear() {
		notifyDataSetChanged();
	}

	@Override
	public int getCount() {
		return m_proxy.getCount();
	}

	@Override
	public Object getItem(int index) {
		return m_proxy.getItem(index);
	}

	@Override
	public int getViewTypeCount() {
	    return 2;
	}
	
	@Override
	/**
	 * 0: normal entry, 1: section header
	 */
	public int getItemViewType(int position) {
		return m_proxy.getItem(position).entryid.isEmpty()?1:0;
	}
	
	@Override
	public long getItemId(int arg0) {
		return arg0;
	}
	
	@Override
	public boolean areAllItemsEnabled() {
		return false;
	}

	@Override
	public boolean isEnabled(int position) {
		return !m_proxy.getItem(position).entryid.isEmpty();
	}

}