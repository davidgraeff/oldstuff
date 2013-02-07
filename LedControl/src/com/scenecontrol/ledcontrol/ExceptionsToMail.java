package com.scenecontrol.ledcontrol;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class ExceptionsToMail implements Thread.UncaughtExceptionHandler {

	private Thread.UncaughtExceptionHandler defaultUEH;

	private Context mContext = null;

	public ExceptionsToMail(Context context) {
		this.defaultUEH = Thread.getDefaultUncaughtExceptionHandler();
		this.mContext = context;
	}

	public void uncaughtException(Thread t, Throwable e) {

		StackTraceElement[] arr = e.getStackTrace();
		String Raghav = t.toString();
		String report = e.toString() + "\n\n";
		report += "--------- Stack trace ---------\n\n" + Raghav;
		for (int i = 0; i < arr.length; i++) {
			report += "    " + arr[i].toString() + "\n";
		}
		report += "-------------------------------\n\n";

		// If the exception was thrown in a background thread inside
		// AsyncTask, then the actual exception can be found with getCause
		report += "--------- Cause ---------\n\n";
		Throwable cause = e.getCause();
		if (cause != null) {
			report += cause.toString() + "\n\n";
			arr = cause.getStackTrace();
			for (int i = 0; i < arr.length; i++) {
				report += "    " + arr[i].toString() + "\n";
			}
		}
		report += "-------------------------------\n\n";

		Intent emailIntent = new Intent(android.content.Intent.ACTION_SEND);
		emailIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK|Intent.FLAG_ACTIVITY_CLEAR_TASK|Intent.FLAG_ACTIVITY_MULTIPLE_TASK);
		emailIntent.setType("message/rfc822");
		emailIntent.putExtra(Intent.EXTRA_EMAIL  , new String[]{"david.graeff@web.de"});
		emailIntent.putExtra(android.content.Intent.EXTRA_SUBJECT, "Crash");
		emailIntent.putExtra(android.content.Intent.EXTRA_TEXT, report);
		//app.startActivity(Intent.createChooser(emailIntent, "Email to Friend"));

		AlarmManager mngr = (AlarmManager)mContext.getSystemService(Context.ALARM_SERVICE);
		PendingIntent intent;
		intent = PendingIntent.getActivity(mContext.getApplicationContext(), 0, emailIntent, Intent.FLAG_ACTIVITY_NEW_TASK | Intent.FLAG_ACTIVITY_CLEAR_TASK);
		            if (mngr == null) {
		                Log.e("CrashHandler", "Couldn't retrieve alarm manager!");
		                return;
		            }
		            mngr.set(AlarmManager.RTC, System.currentTimeMillis(), intent);
		
		defaultUEH.uncaughtException(t, e);
	}

}