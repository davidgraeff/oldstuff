package com.scenecontrol.ledcontrol.model;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;
import com.scenecontrol.ledcontrol.network.ModelAndNotificationManager;

import org.json.JSONArray;
import org.json.JSONObject;

import java.text.DateFormatSymbols;
import java.util.ArrayList;


public class SceneModelAlarmProxy implements ModelChangeListener, ModelAvailableInterface {
    public enum AlarmTypeEnum {
        NoAlarm,
        PeriodicAlarm,
        OneTimeAlarm
    }

    public class AlarmInfo {
        public String alarmtime;

        public String getAlarmtime() {
            return alarmtime;
        }

        public AlarmTypeEnum alarmtype;

        public AlarmTypeEnum getAlarmtype() {
            return alarmtype;
        }

        public AlarmInfo(String alarmtime, AlarmTypeEnum alarmtype) {
            this.alarmtime = alarmtime;
            this.alarmtype = alarmtype;
        }
    }

    ;

    final ArrayList<AlarmInfo> mAlarmInfo = new ArrayList<AlarmInfo>();
    SceneModel mModel;
    SceneModel eventModel = null;
    boolean mIsModified;

    private ArrayList<ModelChangeListener> mChangeListener = new ArrayList<ModelChangeListener>();

    public void addChangeListener(ModelChangeListener listener) {
        if (mChangeListener.contains(listener))
            return;
        mChangeListener.add(listener);
        if (mModel != null && mModel.getCount() > 0)
            listener.onModelDataChange();
    }

    public void removeChangeListener(ModelChangeListener listener) {
        mChangeListener.remove(listener);
    }

    public SceneModelAlarmProxy() {
        LedApplication.getModelManager().addModelAvailableListener(this);
        mIsModified = true;
    }

    @Override
    public void finalize() {
        LedApplication.getModelManager().removeModelAvailableListener(this);
    }

    public void clear() {
        if (mModel == null)
            return;
        mAlarmInfo.clear();
        eventModel.removeChangeListener(this);
        eventModel = null;
        mModel.removeChangeListener(this);
        mModel = null;
        for (ModelChangeListener o : mChangeListener) {
            o.onClear();
        }
    }

    public int getCount() {
        if (mModel == null)
            return 0;
        return mModel.getCount();
    }

    private JSONObject getTimeEventForSceneID(String sceneid) {
        if (eventModel == null)
            return null;
        int c = eventModel.getCount();
        for (int i = 0; i < c; ++i) {
            JSONObject data = eventModel.getItem(i).getValues();
            if (data == null)
                continue;
            if (data.has("sceneid_") && data.optString("sceneid_").equals(sceneid) &&
                    data.has("componentid_") && data.optString("componentid_").equals("scenecontrol.time") &&
                    data.has("method_") && data.has("time")) {
                return data;
            }
        }
        return null;
    }

    private void rebuildAlarmInfo() {
        mAlarmInfo.clear();
        int c = mModel.getCount();

        for (int i = 0; i < c; ++i) {
            JSONObject te = getTimeEventForSceneID(mModel.getItem(i).entryid);
            AlarmTypeEnum type = AlarmTypeEnum.NoAlarm;
            String infoString = "";
            if (te != null) {
                String method = te.optString("method_");
                infoString += te.optString("time", "");
                if (method.equals("eventDateTime")) {
                    type = AlarmTypeEnum.OneTimeAlarm;
                    infoString += " (" + te.optString("date", "") + ")";
                } else if (method.equals("eventPeriodic")) {
                    type = AlarmTypeEnum.PeriodicAlarm;
                    JSONArray days = te.optJSONArray("days");
                    DateFormatSymbols dfs = new DateFormatSymbols();
                    String[] weekdays = dfs.getShortWeekdays();
                    if (days.length() > 0)
                        infoString += " (";
                    for (int d = 0; d < days.length(); ++d) {
                        boolean checked = days.optBoolean(d);
                        if (checked)
                            infoString += weekdays[(d + 1) % 7 + 1] + " ";
                    }
                    if (days.length() > 0) {
                        infoString = infoString.substring(0, infoString.length() - 2) + ")";
                    }
                }
            }
            mAlarmInfo.add(new AlarmInfo(infoString, type));
        }
    }

    public AlarmInfo getAlarmInfo(int index) {
        if (mIsModified) {
            mIsModified = false;
            rebuildAlarmInfo();
        }
        return mAlarmInfo.get(index);
    }

    public ModelElement getItem(int index) {
        return mModel.getItem(index);
    }

    @Override
    public void onModelDataChange() {
        mIsModified = true;
        for (ModelChangeListener o : mChangeListener) {
            o.onModelDataChange();
        }
    }

    @Override
    public void onClear() {
        clear();
    }

    @Override
    public void onModelAvailable(SceneModel model) {
        if (model.getID().equals(ModelAndNotificationManager.MODEL_EVENTS)) {
            eventModel = model;
            eventModel.addChangeListener(this);
        } else if (model.getID().equals(ModelAndNotificationManager.MODEL_PROFILES)) {
            mModel = model;
            onModelDataChange();
        }
    }
}
