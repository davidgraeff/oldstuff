package com.scenecontrol.ledcontrol.listadapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.ModelAvailableInterface;
import com.scenecontrol.ledcontrol.model.ModelChangeListener;
import com.scenecontrol.ledcontrol.model.SceneModel;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;

public class LedsListAdapter extends BaseAdapter implements ModelAvailableInterface, ModelChangeListener {
    private LayoutInflater mInflater;
    private SceneModel m_model;

    private LedValueListener mLedValueListener = null;

    public void setPositionListener(final LedValueListener listener) {
        mLedValueListener = listener;
    }

    private OnSeekBarChangeListener sliderListener = new OnSeekBarChangeListener() {
        @Override
        public void onProgressChanged(SeekBar seekBar, int progress,
                                      boolean fromUser) {
            if (fromUser && mLedValueListener != null) {
                String channelid = (String) seekBar.getTag();
                mLedValueListener.onPositionChange(channelid, progress);
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            String channelid = (String) seekBar.getTag();
            m_model.suppressChanges(channelid, true);
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            String channelid = (String) seekBar.getTag();
            m_model.suppressChanges(channelid, false);
        }

    };

    public LedsListAdapter(Context context) {
        super();
        mInflater = LayoutInflater.from(context);
        LedApplication.getModelManager().addModelAvailableListener(this);
    }

    @Override
    public void finalize() {
        LedApplication.getModelManager().removeModelAvailableListener(this);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = mInflater.inflate(R.layout.rowled, null);
            SeekBar slider = (SeekBar) convertView.findViewById(R.id.led_slider);
            slider.setOnSeekBarChangeListener(sliderListener);
        }

        ModelElement data = (ModelElement) getItem(position);
        String name = data.getCacheName();
        String channelid = data.entryid;
        int value = data.getValues().optInt("value");

        TextView item = (TextView) convertView.findViewById(R.id.led_title);
        SeekBar description = (SeekBar) convertView.findViewById(R.id.led_slider);
        description.setTag(channelid);
        item.setText(name);
        description.setProgress(value);

        return convertView;
    }

    @Override
    public void onModelAvailable(SceneModel model) {
        if (!model.getID().equals("leds"))
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
