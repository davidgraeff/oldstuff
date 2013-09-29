package com.scenecontrol.ledcontrol.listadapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.ModelAvailableInterface;
import com.scenecontrol.ledcontrol.model.ModelChangeListener;
import com.scenecontrol.ledcontrol.model.SceneModel;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

public class SocketsListAdapter extends BaseAdapter implements ModelAvailableInterface, ModelChangeListener {
    private LayoutInflater mInflater;
    private SceneModel m_model;
    private Bitmap m_bmpOn = Bitmap.createBitmap(50, 50, Bitmap.Config.ARGB_8888);
    private Bitmap m_bmpOff = Bitmap.createBitmap(50, 50, Bitmap.Config.ARGB_8888);

    public SocketsListAdapter(Context context) {
        super();
        mInflater = LayoutInflater.from(context);
        {
            Paint p = new Paint();
            p.setARGB(255, 46, 139, 87);
            Canvas c = new Canvas(m_bmpOn);
            c.drawCircle(25, 25, 25, p);
        }
        {
            Paint p = new Paint();
            p.setARGB(255, 250, 128, 114);
            Canvas c = new Canvas(m_bmpOff);
            c.drawCircle(25, 25, 25, p);
        }
        LedApplication.getModelManager().addModelAvailableListener(this);
    }

    @Override
    public void finalize() {
        LedApplication.getModelManager().removeModelAvailableListener(this);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.rowsocket, null);
            ViewHolder vh = new ViewHolder();
            vh.socket_title = (TextView) convertView.findViewById(R.id.socket_title);
            vh.socket_icon = (ImageView) convertView.findViewById(R.id.socket_icon);
            convertView.setTag(vh);
        }

        ModelElement data = (ModelElement) getItem(position);
        String name = data.getCacheName();
        @SuppressWarnings("unused")
        String channelid = data.entryid;
        boolean value = data.getValues().optBoolean("value");
        ViewHolder vh = (ViewHolder) convertView.getTag();
        vh.socket_title.setText(name);
        if (value)
            vh.socket_icon.setImageBitmap(m_bmpOn);
        else
            vh.socket_icon.setImageBitmap(m_bmpOff);

        return convertView;
    }

    static class ViewHolder {
        TextView socket_title;
        ImageView socket_icon;
    }

    @Override
    public void onModelAvailable(SceneModel model) {
        if (!model.getID().equals("switches"))
            return;
        m_model = model;
        model.addChangeListener(this);
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
}
