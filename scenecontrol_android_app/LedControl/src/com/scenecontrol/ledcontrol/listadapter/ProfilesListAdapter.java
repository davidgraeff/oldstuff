package com.scenecontrol.ledcontrol.listadapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListAdapter;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.ModelAvailableInterface;
import com.scenecontrol.ledcontrol.model.ModelChangeListener;
import com.scenecontrol.ledcontrol.model.SceneModel;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;
import com.scenecontrol.ledcontrol.network.ModelAndNotificationManager;

@SuppressWarnings("unused")
public class ProfilesListAdapter extends BaseAdapter implements ModelAvailableInterface, ModelChangeListener, ListAdapter {
    private LayoutInflater mInflater;
    private SceneModel m_model;

    private ProfilesValueListener mProfilesValueListener = null;

    public void setPositionListener(final ProfilesValueListener listener) {
        mProfilesValueListener = listener;
    }

    class IconListener implements OnClickListener {

        @Override
        public void onClick(View v) {
            if (mProfilesValueListener != null) {
                ModelElement data = (ModelElement) v.getTag();
                mProfilesValueListener.onIconClick(data.entryid, data.cacheName);
            }
        }
    }

    private IconListener mIconListener = new IconListener();

    public ProfilesListAdapter(Context context) {
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
        if (m_model == null)
            return convertView;

        ModelElement data = m_model.getItem(position);
        if (convertView == null) {
            ViewHolder vh = new ViewHolder();
            if (data.entryid.isEmpty()) {
                convertView = mInflater.inflate(R.layout.rowcategory, null);
                vh.title = (TextView) convertView.findViewById(R.id.list_item_section_text);
                vh.isHeader = true;
            } else {
                convertView = mInflater.inflate(R.layout.rowprofile, null);
                vh.title = (TextView) convertView.findViewById(R.id.profile_title);
                //vh.favicon = (ImageView) convertView.findViewById(R.id.profile_fav);
                //vh.favicon.setOnClickListener(mIconListener);
                vh.isHeader = false;
            }
            convertView.setTag(vh);
        }

        ViewHolder vh = (ViewHolder) convertView.getTag();
        vh.data = data;
        vh.title.setText(data.getCacheName());
        vh.title.setTag(data);
//		if (!vh.isHeader) {
//			vh.favicon.setImageResource(android.R.drawable.gallery_thumb);
//			vh.favicon.setTag(data);
//		}

        return convertView;
    }

    public static class ViewHolder {
        ImageView favicon;
        TextView title;
        public ModelElement data;
        boolean isHeader;
    }

    @Override
    public void onModelAvailable(SceneModel model) {
        if (!model.getID().equals(ModelAndNotificationManager.MODEL_PROFILES))
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
    public int getViewTypeCount() {
        return 2;
    }

    @Override
    /**
     * 0: normal entry, 1: section header
     */
    public int getItemViewType(int position) {
        if (m_model != null)
            return m_model.getItem(position).entryid.isEmpty() ? 1 : 0;
        return 0;
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
        if (m_model != null)
            return !m_model.getItem(position).entryid.isEmpty();
        return false;
    }

}