package com.scenecontrol.ledcontrol;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentTransaction;
import android.content.Intent;
import android.os.Bundle;
import android.speech.RecognizerIntent;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.ViewConfiguration;

import com.scenecontrol.ledcontrol.fragments.TabsLightControlFragment;
import com.scenecontrol.ledcontrol.fragments.TabsScenesFragment;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

public class MainActivity extends Activity {
    private Fragment mBefore = null;
    private String mRestoreViewAfterConnect = null;

    @Override
    protected void onUserLeaveHint() {
        super.onUserLeaveHint();
        LedApplication.getConnectionObserver().stop();
        LedApplication.getServerconnection().CloseConnection();
    }

    @Override
    protected void onResume() {
        super.onResume();
        LedApplication.getConnectionObserver().start();
        LedApplication.tryConnect();

        if (mRestoreViewAfterConnect != null)
            setFragment(mRestoreViewAfterConnect);
        else
            replaceByScenesView();
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Thread.setDefaultUncaughtExceptionHandler(new ExceptionsToMail(this));
        setContentView(R.layout.activity_main);

        // Hack to always show the overflow menu
        try {
            ViewConfiguration config = ViewConfiguration.get(this);
            java.lang.reflect.Field menuKeyField = ViewConfiguration.class.getDeclaredField("sHasPermanentMenuKey");
            if (menuKeyField != null) {
                menuKeyField.setAccessible(true);
                menuKeyField.setBoolean(config, false);
            }
        } catch (Exception ex) {
            // Ignore
        }

        if (savedInstanceState != null) {
            mRestoreViewAfterConnect = savedInstanceState.getString("lastview", null);
        }
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        if (mBefore != null) {
            outState.putString("lastview", mBefore.getClass().getName());
        }
    }

    private void setFragment(String m) {
        FragmentTransaction ft = getFragmentManager().beginTransaction();
        //ft.setCustomAnimations(R.anim.slide_in_left, R.anim.slide_out_right);
        if (mBefore != null) {
            // Do nothing if this fragment is already visible
            if (mBefore.getClass().getName().equals(m))
                return;
        }
        mBefore = Fragment.instantiate(this, m);
        ft.replace(R.id.fragmentcontainer, mBefore, m).commitAllowingStateLoss();
    }

    private void replaceByLightControlView() {
        getActionBar().setDisplayHomeAsUpEnabled(true);
        setFragment(TabsLightControlFragment.class.getName());
    }

    private void replaceByScenesView() {
        getActionBar().setDisplayHomeAsUpEnabled(false);
        setFragment(TabsScenesFragment.class.getName());
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.activity_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                replaceByScenesView();
                return true;
            case R.id.menu_lightcontrol:
                replaceByLightControlView();
                return true;
            case R.id.menu_disconnect:
                LedApplication.getServerconnection().CloseConnection();
                return true;
            case R.id.menu_voicecmd:
                Intent intent =
                        new Intent(RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
                intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                        RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
                intent.putExtra(RecognizerIntent.EXTRA_PROMPT, R.string.voicecmd);
                startActivityForResult(intent, 'r');
                return true;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 'r' && resultCode == RESULT_OK) {
            java.util.List<String> l = data.getExtras().getStringArrayList(RecognizerIntent.EXTRA_RESULTS);
            List<String> cmds = new ArrayList<String>();
            List<String> profiles = new ArrayList<String>();
            for (int i = 0; i < l.size(); ++i) {
                String line = l.get(i).toLowerCase(Locale.GERMAN);
                int profilWordIndex = line.indexOf("profil");
                if (profilWordIndex == -1)
                    continue;

                cmds.add(line.substring(0, profilWordIndex).trim());
                profiles.add(line.substring(profilWordIndex + 1 + "profil".length()));
            }
            if (cmds.contains("starte")) {
                Log.w("SPEECH", "starte: " + profiles.get(0));
            } else {
                Log.w("SPEECH", "unknown. Cmds: " + cmds.toString());
            }

        }
    }

}
