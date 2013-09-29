package com.scenecontrol.ledcontrol.model;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Set;
import java.util.TreeSet;

import com.scenecontrol.ledcontrol.R;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.CompressFormat;

/**
 * 
 * Singleton Model of Favourite scenes
 */
public class FavModel {
	static FavModel mInstance = null;
	private Context mContext;
	private ArrayList<FavModelItem> mData = new ArrayList<FavModelItem>();
	private Set<String> mFavSet;
	private File cacheDir = null;
	private ModelChangeListener mChangeListener = null;
	public static final String PREFS_NAME = "FavFile";

	public class FavModelItem {
		public String sceneid;
		public String scenename;
		public Bitmap scenepicture;

		public FavModelItem(String sceneid, String scenename, Bitmap scenepicture) {
			this.sceneid = sceneid;
			this.scenename = scenename;
			this.scenepicture = scenepicture;
		}
	}

	public static final FavModel getInstance(Context c) {
		if (mInstance == null)
			mInstance = new FavModel(c);
		return mInstance;
	}

	private FavModel(Context context) {
		mContext = context;
		String sdState = android.os.Environment.getExternalStorageState();
		if (sdState.equals(android.os.Environment.MEDIA_MOUNTED)) {
			File f = context.getExternalCacheDir();
			if (f == null)
				return;
			cacheDir = new File(f.getAbsoluteFile(), "favimages");
			if (!cacheDir.exists())
				if (!cacheDir.mkdirs()) {
					cacheDir = null;
				}
		}
		Bitmap plusB = BitmapFactory.decodeResource(context.getApplicationContext().getResources(), R.drawable.plus);
		mData.add(new FavModelItem(null, "", plusB));
		loadFromConfig();
	}

	@Override
	protected void finalize() throws Throwable {
		super.finalize();
		SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME,
				Context.MODE_MULTI_PROCESS);
		Editor editor = settings.edit();
		editor.putStringSet("favs", mFavSet);
		editor.apply();
	}

	private void loadFromConfig() {
		SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME,
				Context.MODE_MULTI_PROCESS);
		mFavSet = settings.getStringSet("favs", new TreeSet<String>());
		Iterator<String> i = mFavSet.iterator();
		while (i.hasNext()) {
			String sceneid = i.next();
			File f = new File(cacheDir, sceneid + ".jpg");
			Bitmap b = null;
			try {
				b = BitmapFactory.decodeStream(new FileInputStream(f));
			} catch (FileNotFoundException e) {
			}
			mData.add(new FavModelItem(sceneid, settings.getString(sceneid,
					sceneid), b));
		}
	}

	public void setChangeListener(ModelChangeListener listener) {
		mChangeListener = listener;
		if (mChangeListener != null && mData.size() > 0)
			mChangeListener.onModelDataChange();
	}

	public boolean isFavourite(String sceneid) {
		if (sceneid==null)
			return true;
		
		for (int i = 0; i < mData.size(); ++i) {
			if (sceneid.equals(mData.get(i).sceneid)) {
				return true;
			}
		}
		return false;
	}

	public boolean setFavourite(String sceneid, String scenename) {
		if (isFavourite(sceneid))
			return false;

		SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME,
				Context.MODE_MULTI_PROCESS);
		mFavSet.add(sceneid);
		Editor editor = settings.edit();
		editor.putStringSet("favs", mFavSet);
		editor.putString(sceneid, scenename);
		editor.apply();

		mData.add(new FavModelItem(sceneid, scenename, null));
		if (mChangeListener != null) {
			mChangeListener.onModelDataChange();
		}
		return true;
	}

	public int getCount() {
		return mData.size();
	}

	public FavModelItem getItem(int index) {
		return mData.get(index);
	}

	public void removeFavourite(String sceneid) {
		if (sceneid==null)
			return;
		
		int pos = -1;
		for (int i = 0; i < mData.size(); ++i) {
			if (sceneid.equals(mData.get(i).sceneid)) {
				pos = i;
				break;
			}
		}
		if (pos == -1)
			return;

		SharedPreferences settings = mContext.getSharedPreferences(PREFS_NAME,
				Context.MODE_MULTI_PROCESS);
		mFavSet.remove(sceneid);
		mData.remove(pos);
		Editor editor = settings.edit();
		editor.putStringSet("favs", mFavSet);
		editor.remove(sceneid);
		editor.apply();
		
		File f = new File(cacheDir, sceneid + ".jpg");
		if (f.exists()) {
			f.delete();
		}

		if (mChangeListener != null)
			mChangeListener.onModelDataChange();
	}

	public void putBitmap(String sceneid, Bitmap b) {
		if (cacheDir == null || sceneid == null)
			return;
		File f = new File(cacheDir, sceneid + ".jpg");
		try {
			// Log.w("FavModelImages", "write: "+f.getAbsolutePath());
			FileOutputStream fos;
			fos = new FileOutputStream(f);
			b.compress(CompressFormat.JPEG, 90, fos);
			fos.close();
			
			for (int i = 0; i < mData.size(); ++i) {
				if (sceneid.equals(mData.get(i).sceneid)) {
					mData.get(i).scenepicture = b;
					break;
				}
			}
			
			if (mChangeListener != null)
				mChangeListener.onModelDataChange();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
