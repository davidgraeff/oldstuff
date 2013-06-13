package com.scenecontrol.ledcontrol.fragments;

import android.app.Fragment;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.fragments.AlarmDialogFragment.AlarmSetListener;
import com.scenecontrol.ledcontrol.listadapter.ScenesForAlarmAdapter;
import com.scenecontrol.ledcontrol.model.SceneModel.ModelElement;
import com.scenecontrol.ledcontrol.network.NotificationListenerInterface;

import org.json.JSONException;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Locale;

public class AlarmListFragment extends Fragment implements AlarmSetListener, OnItemClickListener, NotificationListenerInterface {
    private ScenesForAlarmAdapter mProfilesListAdapter;
    private TextView mNextAlarmText;

    private class NextAlarmTextUpdater implements OnClickListener {

        @Override
        public void onClick(View arg0) {
            updateNextAlarmText();
        }
    }

    class NextAlarmTextUpdaterTimer implements Runnable {
        private Handler mNextAlarmTextUpdaterHandler = new Handler();

        public void setupTimer(int seconds) {
            remainingSeconds = seconds;
            mNextAlarmTextUpdaterHandler.removeCallbacks(this);
            if (seconds > 60)
                mNextAlarmTextUpdaterHandler.postDelayed(this, 1000 * (seconds - 60));
            else
                mNextAlarmTextUpdaterHandler.postDelayed(this, 1000);
        }

        public void disable() {
            mNextAlarmTextUpdaterHandler.removeCallbacks(this);
        }

        public int remainingSeconds = 0;

        public void run() {
            --remainingSeconds;
            mNextAlarmText.setText("Alarm in " + Integer.valueOf(remainingSeconds).toString() + " Sekunden");
            mNextAlarmTextUpdaterHandler.removeCallbacks(this);
            if (remainingSeconds > 0)
                mNextAlarmTextUpdaterHandler.postDelayed(this, 1000);
            else if (remainingSeconds == 0)
                updateNextAlarmText();
        }
    }

    ;

    NextAlarmTextUpdaterTimer mNextAlarmTextUpdaterTimer = new NextAlarmTextUpdaterTimer();

    private void updateNextAlarmText() {
        mNextAlarmText.setText("Update...");
        LedApplication.getSceneProtocol().requestProperty("nextalarm", "scenecontrol.time", "null");
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.alarm_fragment, container, false);
        ListView listview = (ListView) v.findViewById(R.id.alarmlistview);
        mNextAlarmText = (TextView) v.findViewById(R.id.nextalarmtext);
        mNextAlarmText.setOnClickListener(new NextAlarmTextUpdater());
        listview.setTextFilterEnabled(true);
        mProfilesListAdapter = new ScenesForAlarmAdapter(getActivity());
        listview.setAdapter(mProfilesListAdapter);
        listview.setOnItemClickListener(this);
        LedApplication.getModelManager().addNotificationListener(this);
        updateNextAlarmText();
        return v;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
        ScenesForAlarmAdapter.ViewHolder vh = (ScenesForAlarmAdapter.ViewHolder) v.getTag();
        ModelElement modeldata = vh.data;
        // Toast.makeText(getActivity(),"Execute: " +modeldata.cacheName,
        // Toast.LENGTH_SHORT).show();
        // String data =
        // "{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":\"runcollection\", \"sceneid_\":\""+modeldata.entryid+"\"}\n";
        // LedApplication.getServerconnection().getConnection().Send(data);
        AlarmDialogFragment alarmDialogFragment = new AlarmDialogFragment();
        alarmDialogFragment.setAlarmSetListener(this);
        Bundle bundle = new Bundle();
        bundle.putString("sceneid", modeldata.entryid);
        bundle.putString("scenename", modeldata.cacheName);
        alarmDialogFragment.setArguments(bundle);
        alarmDialogFragment.show(getFragmentManager(), "alarmDialogFragment");
    }

    @Override
    public void alarmSet(String sceneid, String scenename) {
        Toast.makeText(getActivity(), "Speichere " + scenename, Toast.LENGTH_SHORT).show();
    }

    @Override
    public void notification(JSONObject json) {
        try {
            if (!json.has("componentid_") || !json.has("id_"))
                return;
            String id = json.getString("id_");
            if (id.equals("nextalarm") && json.getString("componentid_").equals("scenecontrol.time")) {
                mNextAlarmTextUpdaterTimer.disable();
                if (json.has("seconds")) {
                    int seconds = json.getInt("seconds");
                    Calendar now = Calendar.getInstance();
                    now.add(Calendar.SECOND, seconds);
                    if (seconds >= 3600 * 24) {
                        mNextAlarmText.setText("Alarm: "
                                + new SimpleDateFormat("dd.MM.yyyy HH:mm", Locale.GERMANY).format(now.getTime())
                                + " Uhr");
                    } else if (seconds <= 0) {
                        mNextAlarmText.setText("TimePlugin fehlerhaft!");
                    } else {
                        mNextAlarmText.setText("N�chster Alarm: "
                                + new SimpleDateFormat("HH:mm", Locale.GERMANY).format(now.getTime()) + " Uhr");
                        mNextAlarmTextUpdaterTimer.setupTimer(seconds);
                    }
                } else {
                    mNextAlarmText.setText("Kein Alarm");
                }

            }
        } catch (JSONException e) {
            Log.e("SERVER", e.getMessage());
        }
    }
}
