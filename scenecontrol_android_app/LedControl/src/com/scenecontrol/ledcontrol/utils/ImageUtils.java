package com.scenecontrol.ledcontrol.utils;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

import com.scenecontrol.ledcontrol.R;

import android.app.AlertDialog;
import android.app.Fragment;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ResolveInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.provider.MediaStore;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

public class ImageUtils {
	private static Uri mImageCaptureUri;
	private static boolean mCameraTempFile;
	public static final int PICK_FROM_CAMERA = 1;
	public static final int CROP_FROM_CAMERA = 2;
	public static final int PICK_FROM_FILE = 3;

	public static Bitmap decodeFile(InputStream stream, InputStream stream2,
			int imageSize) {
		Bitmap b = null;
		try {
			// Decode image size
			BitmapFactory.Options o = new BitmapFactory.Options();
			o.inJustDecodeBounds = true;

			BitmapFactory.decodeStream(stream, null, o);
			stream.close();

			int scale = 1;
			if (o.outHeight > imageSize || o.outWidth > imageSize) {
				scale = (int) Math.pow(
						2,
						(int) Math.round(Math.log(imageSize
								/ (double) Math.max(o.outHeight, o.outWidth))
								/ Math.log(0.5)));
			}

			// Decode with inSampleSize
			BitmapFactory.Options o2 = new BitmapFactory.Options();
			o2.inSampleSize = scale;
			b = BitmapFactory.decodeStream(stream2, null, o2);
			stream2.close();
		} catch (IOException e) {
		}
		return b;
	}

	public static void setImageURI(Uri uri) {
		mImageCaptureUri = uri;
	}
	
	public static boolean isImageURISet() {
		return mImageCaptureUri != null;
	}
	
	public static void doSelectPicture(final Fragment fragment, Context context) {
		final String[] items = new String[] { "Take from camera",
				"Select from Gallery" };
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(context,
				android.R.layout.select_dialog_item, items);
		AlertDialog.Builder builder = new AlertDialog.Builder(context);

		builder.setTitle("Select Image");
		builder.setAdapter(adapter, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface dialog, int item) { // pick from camera
				if (item == 0) {
					Intent intent = new Intent(MediaStore.ACTION_IMAGE_CAPTURE);
					mCameraTempFile = true;
					try {
						mImageCaptureUri = Uri.fromFile(File.createTempFile("my_app", ".jpg"));
					} catch (IOException e1) {
						e1.printStackTrace();
						return;
					}

					intent.putExtra(android.provider.MediaStore.EXTRA_OUTPUT, mImageCaptureUri);

					try {
						intent.putExtra("return-data", true);

						fragment.startActivityForResult(intent,
								PICK_FROM_CAMERA);
					} catch (ActivityNotFoundException e) {
						e.printStackTrace();
						return;
					}
				} else { // pick from file
					Intent intent = new Intent();
					mCameraTempFile = false;
					intent.setType("image/*");
					intent.setAction(Intent.ACTION_GET_CONTENT);

					fragment.startActivityForResult(Intent.createChooser(
							intent, "Complete action using"), PICK_FROM_FILE);
				}
			}
		});

		AlertDialog dialog = builder.create();
		dialog.show();
	}
	
	public static void doCrop(final Fragment fragment, Context context, int outputX, int outputY) {
		class CropOption {
			public CharSequence title;
			public Drawable icon;
			public Intent appIntent;
		}
		class CropOptionAdapter extends ArrayAdapter<CropOption> {
			private ArrayList<CropOption> mOptions;
			private LayoutInflater mInflater;

			public CropOptionAdapter(Context context, ArrayList<CropOption> options) {
				super(context, R.layout.crop_selector, options);

				mOptions 	= options;

				mInflater	= LayoutInflater.from(context);
			}

			@Override
			public View getView(int position, View convertView, ViewGroup group) {
				if (convertView == null)
					convertView = mInflater.inflate(R.layout.crop_selector, null);

				CropOption item = mOptions.get(position);

				if (item != null) {
					((ImageView) convertView.findViewById(R.id.iv_icon)).setImageDrawable(item.icon);
					((TextView) convertView.findViewById(R.id.tv_name)).setText(item.title);

					return convertView;
				}

				return null;
			}
		}
		
        final ArrayList<CropOption> cropOptions = new ArrayList<CropOption>();
 
        Intent intent = new Intent("com.android.camera.action.CROP");
        intent.setType("image/*");
 
        List<ResolveInfo> list = fragment.getActivity().getPackageManager().queryIntentActivities( intent, 0 );
 
        int size = list.size();
 
        if (size == 0) {
            //Toast.makeText(this, "Can not find image crop app", Toast.LENGTH_SHORT).show();
        	removeTempFile(fragment);
            return;
        } else {
            intent.setData(mImageCaptureUri);
 
            intent.putExtra("outputX", outputX);
            intent.putExtra("outputY", outputY);
            intent.putExtra("aspectX", 1);
            intent.putExtra("aspectY", 1);
            intent.putExtra("scale", true);
            intent.putExtra("return-data", true);
 
            if (size == 1) {
                Intent i        = new Intent(intent);
                ResolveInfo res = list.get(0);
 
                i.setComponent( new ComponentName(res.activityInfo.packageName, res.activityInfo.name));
 
                fragment.startActivityForResult(i, CROP_FROM_CAMERA);
            } else {
                for (ResolveInfo res : list) {
                    final CropOption co = new CropOption();
 
                    co.title    = fragment.getActivity().getPackageManager().getApplicationLabel(res.activityInfo.applicationInfo);
                    co.icon     = fragment.getActivity().getPackageManager().getApplicationIcon(res.activityInfo.applicationInfo);
                    co.appIntent= new Intent(intent);
 
                    co.appIntent.setComponent( new ComponentName(res.activityInfo.packageName, res.activityInfo.name));
 
                    cropOptions.add(co);
                }
 
                CropOptionAdapter adapter = new CropOptionAdapter(context, cropOptions);
 
                AlertDialog.Builder builder = new AlertDialog.Builder(context);
                builder.setTitle("Choose Crop App");
                builder.setAdapter( adapter, new DialogInterface.OnClickListener() {
                    public void onClick( DialogInterface dialog, int item ) {
                    	fragment.startActivityForResult( cropOptions.get(item).appIntent, CROP_FROM_CAMERA);
                    }
                });
 
                builder.setOnCancelListener( new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel( DialogInterface dialog ) {
                		removeTempFile(fragment);
                    }
                } );
 
                AlertDialog alert = builder.create();
 
                alert.show();
            }
        }
    }
	
	public static void removeTempFile(Fragment fragment) {
        if (mCameraTempFile && mImageCaptureUri != null ) {
        	fragment.getActivity().getContentResolver().delete(mImageCaptureUri, null, null );
        }
        mImageCaptureUri = null;
	}
}
