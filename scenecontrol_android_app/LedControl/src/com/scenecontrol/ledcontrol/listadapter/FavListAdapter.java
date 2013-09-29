package com.scenecontrol.ledcontrol.listadapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.FavModel;
import com.scenecontrol.ledcontrol.model.FavModel.FavModelItem;
import com.scenecontrol.ledcontrol.model.ModelChangeListener;

public class FavListAdapter  extends BaseAdapter implements ModelChangeListener {
	private LayoutInflater mInflater;
	private FavModel m_model;
	private Bitmap placeholder = null;
	private boolean mSelectionMode = false;
	
	public FavListAdapter(Context context) {
		super();
		mInflater = LayoutInflater.from(context);
		m_model = FavModel.getInstance(context);
		m_model.setChangeListener(this);
		placeholder = BitmapFactory.decodeResource(context.getResources(),
				android.R.drawable.star_big_on);
	}
	
	public void setSelectionMode(boolean b) {
		mSelectionMode = b;
		notifyDataSetChanged();
	}
	
	@Override
	public void finalize() {
		if (m_model != null)
			m_model.setChangeListener(null);
	}
	
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		FavModelItem data = (FavModelItem)getItem(position);
		
		if (convertView == null) {
			convertView = mInflater.inflate(R.layout.rowfavorite, null);
			ViewHolder vh = new ViewHolder();
			vh.socket_title = (TextView) convertView.findViewById(R.id.favtitle);
			vh.socket_icon = (ImageView) convertView.findViewById(R.id.favimage);
			convertView.setTag(vh);
		}

		ViewHolder vh = (ViewHolder)convertView.getTag();
		vh.data = data;
		vh.socket_title.setText(data.scenename);
		vh.socket_title.setVisibility((data.sceneid==null)?TextView.INVISIBLE:TextView.VISIBLE);
		vh.socket_icon.setImageBitmap((data.scenepicture==null)?placeholder:data.scenepicture);

		return convertView;
	}
	
	static public class ViewHolder {
	    TextView socket_title;
	    ImageView socket_icon;
	    public FavModelItem data;
	}

	@Override
	public void onModelDataChange() {
		notifyDataSetChanged();
	}

	@Override
	public void onClear() {
		m_model = null;
		notifyDataSetChanged();
	}

	@Override
	public int getCount() {
		if (m_model != null)
			return m_model.getCount();
		return 0;
	}

	@Override
	public Object getItem(int index) {
		if (m_model != null)
			return m_model.getItem(index);
		return null;
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
		if (mSelectionMode && position==0)
			return false;
		return true;
	}
}