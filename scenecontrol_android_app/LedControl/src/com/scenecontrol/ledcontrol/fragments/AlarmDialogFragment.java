package com.scenecontrol.ledcontrol.fragments;

import android.app.DatePickerDialog;
import android.app.DialogFragment;
import android.app.TimePickerDialog;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.DatePicker;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.RadioGroup.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.TimePicker;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.model.SceneModel;
import com.scenecontrol.ledcontrol.network.ModelAndNotificationManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Locale;

public class AlarmDialogFragment extends DialogFragment {
    public interface AlarmSetListener {
        void alarmSet(String sceneid, String scenename);
    }

    private String mSceneid;
    private String mScenename;
    private Button mbtnSave;
    private Button mbtnRemove;
    private TextView mdatetext;
    private TextView mtimetext;
    private RadioGroup mRadioGroup;
    private ArrayList<CheckBox> mCheckBoxes;
    JSONObject mTimeEvent = null;
    Calendar myCalendar = Calendar.getInstance();
    AlarmSetListener mAlarmSetListener = null;

    public void setAlarmSetListener(AlarmSetListener a) {
        mAlarmSetListener = a;
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.alarmdialog, container, false);
        mdatetext = (TextView) v.findViewById(R.id.EditDate);
        mdatetext.setText(new SimpleDateFormat("dd.MM.yyyy", Locale.GERMANY).format(myCalendar.getTime()));
        mdatetext.setOnClickListener(new onDateFieldListener());
        mtimetext = (TextView) v.findViewById(R.id.EditTime);
        mtimetext.setText(new SimpleDateFormat("HH:mm", Locale.GERMANY).format(myCalendar.getTime()));
        mtimetext.setOnClickListener(new onTimeFieldListener());
        mRadioGroup = (RadioGroup) v.findViewById(R.id.alarmTimes);
        mRadioGroup.setOnCheckedChangeListener(new onTimesChangedListener());
        mCheckBoxes = new ArrayList<CheckBox>();
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay1));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay2));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay3));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay4));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay5));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay6));
        mCheckBoxes.add((CheckBox) v.findViewById(R.id.chkAlarmDay7));
        mbtnSave = (Button) v.findViewById(R.id.ButtonAlarmSave);
        mbtnSave.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                saveRemoveButton(v);
            }
        });
        mbtnRemove = (Button) v.findViewById(R.id.ButtonAlarmRemove);
        mbtnRemove.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                saveRemoveButton(v);
            }
        });

        Bundle bundle = this.getArguments();
        mSceneid = bundle.getString("sceneid", null);
        mScenename = bundle.getString("scenename", "");
        getDialog().setTitle("Alarm - " + mScenename);

        mTimeEvent = null;

        if (mSceneid == null) {
            mRadioGroup.clearCheck();
            mRadioGroup.setVisibility(View.GONE);
        } else {
            mRadioGroup.setVisibility(View.VISIBLE);
            // find first time event for this scene id
            SceneModel model = LedApplication.getModelManager().getModel(ModelAndNotificationManager.MODEL_EVENTS);
            if (model != null) {
                int len = model.getCount();
                for (int i = 0; i < len; ++i) {
                    JSONObject data = model.getItem(i).getValues();
                    if (data.has("sceneid_") && data.optString("sceneid_").equals(mSceneid) &&
                            data.has("componentid_") && data.optString("componentid_").equals("scenecontrol.time") &&
                            data.has("method_") && data.has("time")) {
                        mTimeEvent = data;
                        break;
                    }
                }
            }
        }

        if (mTimeEvent != null && parseDatum(v)) {
            mbtnSave.setText(R.string.alarmsavebutton);
            mbtnRemove.setEnabled(true);
        } else {
            mbtnRemove.setEnabled(false);
            mRadioGroup.clearCheck();
            mbtnSave.setText(R.string.alarmcreatebutton);
        }
        updateVisiblity();

        return v;
    }

    void updateVisiblity() {
        if (mRadioGroup.getCheckedRadioButtonId() == R.id.radioAlarmOnce) {
            for (CheckBox b : mCheckBoxes) b.setVisibility(View.GONE);
            mdatetext.setVisibility(View.VISIBLE);
            mtimetext.setVisibility(View.VISIBLE);
            mbtnSave.setEnabled(true);
        } else if (mRadioGroup.getCheckedRadioButtonId() == R.id.radioAlarmRepeat) {
            for (CheckBox b : mCheckBoxes) b.setVisibility(View.VISIBLE);
            mdatetext.setVisibility(View.GONE);
            mtimetext.setVisibility(View.VISIBLE);
            mbtnSave.setEnabled(true);
        } else {
            mbtnSave.setEnabled(false);
            mdatetext.setVisibility(View.GONE);
            mtimetext.setVisibility(View.GONE);
            for (CheckBox b : mCheckBoxes) b.setVisibility(View.GONE);
        }
    }

    private boolean parseDatum(View v) {
        // fill in variables
        String method = mTimeEvent.optString("method_");
        java.util.Date timeDate;
        try {
            SimpleDateFormat t1 = new SimpleDateFormat("HH:mm", Locale.GERMANY);
            timeDate = t1.parse(mTimeEvent.optString("time"));
            mtimetext.setText(t1.format(timeDate));
            myCalendar.setTime(timeDate);
        } catch (ParseException e) {
            return false;
        }
        if (method.equals("eventDateTime")) {
            String date = mTimeEvent.optString("date");
            if (date == null)
                return false;
            try {
                SimpleDateFormat t1 = new SimpleDateFormat("yyyy-MM-dd", Locale.GERMANY);
                java.util.Date d = t1.parse(date);
                t1.applyPattern("dd.MM.yyyy");
                mdatetext.setText(t1.format(d));
                Calendar calendar = GregorianCalendar.getInstance();
                calendar.setTime(d);
                myCalendar.set(Calendar.YEAR, calendar.get(Calendar.YEAR));
                myCalendar.set(Calendar.MONTH, calendar.get(Calendar.MONTH));
                myCalendar.set(Calendar.DAY_OF_MONTH, calendar.get(Calendar.DAY_OF_MONTH));
                //Log.w("DATE", d.toString()+" "+myCalendar.toString());
            } catch (ParseException e) {
                return false;
            }
            RadioButton rb1 = (RadioButton) v.findViewById(R.id.radioAlarmOnce);
            rb1.setChecked(true);
            return true;
        } else if (method.equals("eventPeriodic")) {
            //Log.w("ALARM", "Found event:"+mTimeEvent.toString());
            JSONArray days = mTimeEvent.optJSONArray("days");
            if (days == null || days.length() < mCheckBoxes.size())
                return false;
            for (int i = 0; i < mCheckBoxes.size(); ++i) {
                boolean checked = days.optBoolean(i);
                mCheckBoxes.get(i).setChecked(checked);
            }
            RadioButton rb1 = (RadioButton) v.findViewById(R.id.radioAlarmRepeat);
            rb1.setChecked(true);
            return true;
        }
        return false;
    }

    public Button getMbtnSave() {
        return mbtnSave;
    }

    public void setMbtnSave(Button mbtnSave) {
        this.mbtnSave = mbtnSave;
    }

    private class onTimeFieldListener implements OnClickListener {
        @Override
        public void onClick(View v) {
            new TimePickerDialog(getActivity(), t, myCalendar
                    .get(Calendar.HOUR_OF_DAY), myCalendar
                    .get(Calendar.MINUTE), true).show();
        }
    }

    private class onDateFieldListener implements OnClickListener {
        @Override
        public void onClick(View v) {
            new DatePickerDialog(getActivity(), d, myCalendar
                    .get(Calendar.YEAR), myCalendar.get(Calendar.MONTH),
                    myCalendar.get(Calendar.DAY_OF_MONTH)).show();
        }
    }

    public void saveRemoveButton(View view) {
        JSONObject o = new JSONObject();
        try {
            o.put("componentid_", "server");
            o.put("type_", "execute");

            if (view.getId() == R.id.ButtonAlarmSave) {
                JSONObject doc = mTimeEvent;
                if (doc == null) {
                    doc = new JSONObject();
                    doc.put("instanceid_", "null"); // ONLY WORK IF PLUGIN with instanceid == 0
                    doc.put("componentid_", "scenecontrol.time");
                    doc.put("type_", "event");
                    doc.put("id_", "GENERATEGUID");
                    doc.put("sceneid_", mSceneid);
                }
                doc.put("time", new SimpleDateFormat("HH:mm", Locale.GERMANY).format(myCalendar.getTime()));
                if (mRadioGroup.getCheckedRadioButtonId() == R.id.radioAlarmOnce) {
                    doc.put("method_", "eventDateTime");
                    // Date format: [RFC 3339] and [ISO 8601]
                    doc.put("date", new SimpleDateFormat("yyyy-MM-dd", Locale.GERMANY).format(myCalendar.getTime()));
                    doc.remove("days");
                } else if (mRadioGroup.getCheckedRadioButtonId() == R.id.radioAlarmRepeat) {
                    JSONArray days = new JSONArray();
                    for (int i = 0; i < mCheckBoxes.size(); ++i) {
                        days.put(mCheckBoxes.get(i).isChecked());
                    }
                    doc.put("method_", "eventPeriodic");
                    doc.put("days", days);
                    doc.remove("date");
                } else {
                    return;
                }
                o.put("doc", doc);
                o.put("method_", "changeDocument");
            } else if (mTimeEvent != null && view.getId() == R.id.ButtonAlarmRemove) {
                o.put("doc", mTimeEvent);
                o.put("method_", "removeDocument");
            } else
                return;
        } catch (JSONException e) {
            Log.e("ALARM", e.toString());
            return;
        }

        //Log.w("SENDEN", o.toString());
        LedApplication.getServerconnection().write(o.toString());

        if (mAlarmSetListener != null)
            mAlarmSetListener.alarmSet(mSceneid, mScenename);
        dismiss();
    }

    private class onTimesChangedListener implements OnCheckedChangeListener {
        @Override
        public void onCheckedChanged(RadioGroup arg0, int arg1) {
            updateVisiblity();
        }
    }

    DatePickerDialog.OnDateSetListener d = new DatePickerDialog.OnDateSetListener() {
        @Override
        public void onDateSet(DatePicker view, int year, int monthOfYear,
                              int dayOfMonth) {
            myCalendar.set(Calendar.YEAR, year);
            myCalendar.set(Calendar.MONTH, monthOfYear);
            myCalendar.set(Calendar.DAY_OF_MONTH, dayOfMonth);
            mdatetext.setText(new SimpleDateFormat("dd.MM.yyyy", Locale.GERMANY).format(myCalendar.getTime()));
        }
    };

    TimePickerDialog.OnTimeSetListener t = new TimePickerDialog.OnTimeSetListener() {
        public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
            myCalendar.set(Calendar.HOUR_OF_DAY, hourOfDay);
            myCalendar.set(Calendar.MINUTE, minute);
            mtimetext.setText(new SimpleDateFormat("HH:mm", Locale.GERMANY).format(myCalendar.getTime()));
        }
    };
}
