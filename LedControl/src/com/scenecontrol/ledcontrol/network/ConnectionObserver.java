package com.scenecontrol.ledcontrol.network;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.ArrayList;

import com.scenecontrol.ledcontrol.LedApplication;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class ConnectionObserver extends BroadcastReceiver {
	public static final int STATE_NOCONNECTION = 0;
	public static final int STATE_WLANON = 1;
	public static final int STATE_HOSTLOOK = 2;
	public static final int STATE_CONNECTED = 3;
	private static Handler m_updateViewHandler = null;
	private WaitForConnectionThread mWaitForConnectionThread = null;

	public static Handler getUpdateViewHandler() {
		return m_updateViewHandler;
	}

	public ConnectionObserver() {
		m_updateViewHandler = new UpdateViewHandler(this);
	}

	public interface ConnectionObserverInterface {
		void connectityChanged(int state);
	}

	private ArrayList<ConnectionObserverInterface> mListens = new ArrayList<ConnectionObserverInterface>();

	public void addListener(ConnectionObserverInterface o) {
		if (!mListens.contains(o))
			mListens.add(o);
	}

	public void removeListener(ConnectionObserverInterface o) {
		mListens.remove(o);
	}

	public void sendViaMessageHandler(int state) {
		for (ConnectionObserverInterface listener : mListens) {
			listener.connectityChanged(state);
		}
	}

	public boolean isWLanConnected(Context context) {
		ConnectivityManager conMan = (ConnectivityManager) context.getSystemService(
				Context.CONNECTIVITY_SERVICE);
		NetworkInfo wifi = conMan.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
		return (wifi != null && wifi.getState() == NetworkInfo.State.CONNECTED);
	}

	static class UpdateViewHandler extends Handler {
		private final WeakReference<ConnectionObserver> mConnectionObserver;

		UpdateViewHandler(ConnectionObserver fragment) {
			mConnectionObserver = new WeakReference<ConnectionObserver>(fragment);
		}

		public void handleMessage(Message msg) {
			// If the NoConnectionFragment is already gone -> do nothing
			if (mConnectionObserver.get() == null)
				return;

			mConnectionObserver.get().sendViaMessageHandler(msg.what);
		}
	};

	public class WaitForConnectionThread extends AsyncTask<Void, Void, Void> {
		@Override
		protected Void doInBackground(Void... datas) {
			int counter = 0;
			boolean reachable = true;
			Handler h = getUpdateViewHandler();

			// only check for wlan, if we are not in the emulator
			Message.obtain(h, STATE_WLANON).sendToTarget();
					
			ConnectivityManager conMan = (ConnectivityManager) LedApplication.getAppContext().getSystemService(
					Context.CONNECTIVITY_SERVICE);
			NetworkInfo wifi = conMan.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
			while (wifi == null || wifi.getState() != NetworkInfo.State.CONNECTED) {
				if (++counter > 20)
					return null;
				wifi = conMan.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
				try {
					Thread.sleep(250);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			Message.obtain(h, STATE_HOSTLOOK).sendToTarget();
			counter = 0;
			while (true) {
				if (++counter > 20) {
					reachable = false;
					break;
				}
				try {
					if (InetAddress.getByName(LedApplication.getHost()).isReachable(30)) {
						break;
					}
				} catch (UnknownHostException e1) {
					Log.w("WIFI", "unknown host");
					reachable = false;
					break;
				} catch (IOException e1) {
					reachable = false;
					break;
				}

				try {
					Thread.sleep(250);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			Message.obtain(h, reachable ? STATE_CONNECTED : STATE_NOCONNECTION).sendToTarget();
			return null;
		}
		@Override
		protected void onPostExecute(Void result) {
			mWaitForConnectionThread = null;
		}
	}

	@Override
	public void onReceive(Context context, Intent intent) {
		NetworkInfo wifi = intent.getParcelableExtra(WifiManager.EXTRA_NETWORK_INFO);

		if (mWaitForConnectionThread == null && wifi.getState() == NetworkInfo.State.CONNECTED) {
			mWaitForConnectionThread = new WaitForConnectionThread();
			mWaitForConnectionThread.execute();
		} else if (wifi.getState() == NetworkInfo.State.DISCONNECTED) {
			sendViaMessageHandler(STATE_NOCONNECTION);
		}
	}

	public void start() {
		LedApplication.getAppContext().registerReceiver(this, new IntentFilter(WifiManager.NETWORK_STATE_CHANGED_ACTION));
	}

	public void stop() {
	    LedApplication.getAppContext().unregisterReceiver(this);
	}
}
