package com.scenecontrol.ledcontrol.fragments;

import android.app.Activity;
import android.app.Fragment;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.util.SparseBooleanArray;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView.MultiChoiceModeListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.Toast;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.fragments.SceneSelectDialogFragment.SceneSelectedListener;
import com.scenecontrol.ledcontrol.listadapter.FavListAdapter;
import com.scenecontrol.ledcontrol.listadapter.FavListAdapter.ViewHolder;
import com.scenecontrol.ledcontrol.model.FavModel;
import com.scenecontrol.ledcontrol.model.FavModel.FavModelItem;
import com.scenecontrol.ledcontrol.utils.ImageUtils;

import java.util.ArrayList;

public class FavouritesFragment extends Fragment implements OnItemClickListener, SceneSelectedListener {
    private FavListAdapter mFavListAdapter;
    private GridView gridview;
    private ActionMode mContextActionMode;
    private MultiChoiceModeListener m = new MultiChoiceModeListener() {

        @Override
        public void onItemCheckedStateChanged(ActionMode mode, int position,
                                              long id, boolean checked) {
            if (position == 0) {
                mode.finish();
                return;
            }
            // Here you can do something when items are selected/de-selected,
            // such as update the title in the CAB
            mode.setTitle("Selected: " + Integer.valueOf(gridview.getCheckedItemCount()).toString());
        }

        @Override
        public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
            // Respond to clicks on the actions in the CAB
            switch (item.getItemId()) {
                case R.id.favmenu_remove:
                    ArrayList<FavModelItem> favitems = getSelectedFavItems();
                    for (FavModelItem favitem : favitems) {
                        Toast.makeText(getActivity(), "Remove: " + favitem.scenename, Toast.LENGTH_SHORT).show();
                        FavModel.getInstance(getActivity()).removeFavourite(favitem.sceneid);
                    }
                    mode.finish(); // Action picked, so close the CAB
                    return true;
                case R.id.favmenu_setpicture:
                    ImageUtils.doSelectPicture(FavouritesFragment.this, getActivity());
                    return true;
                default:
                    return false;
            }
        }

        @Override
        public boolean onCreateActionMode(ActionMode mode, Menu menu) {
            // Inflate the menu for the CAB
            MenuInflater inflater = mode.getMenuInflater();
            inflater.inflate(R.menu.fav_context_menu, menu);
            mFavListAdapter.setSelectionMode(true);
            mContextActionMode = mode;
            return true;
        }

        @Override
        public void onDestroyActionMode(ActionMode mode) {
            mContextActionMode = null;
            mFavListAdapter.setSelectionMode(false);
            // Here you can make any necessary updates to the activity when
            // the CAB is removed. By default, selected items are deselected/unchecked.
        }

        @Override
        public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
            // Here you can perform updates to the CAB due to
            // an invalidate() request
            return false;
        }
    };

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fav_fragment, container, false);
        gridview = (GridView) v.findViewById(R.id.favgridview);
        gridview.setTextFilterEnabled(true);
        mFavListAdapter = new FavListAdapter(gridview.getContext());
        gridview.setAdapter(mFavListAdapter);
        gridview.setOnItemClickListener(this);
        gridview.setChoiceMode(GridView.CHOICE_MODE_MULTIPLE_MODAL);
        gridview.setMultiChoiceModeListener(m);
        return v;
    }

    public FavListAdapter getModel() {
        return mFavListAdapter;
    }

    private ArrayList<FavModelItem> getSelectedFavItems() {
        // Get selected items
        SparseBooleanArray selectedIds = gridview.getCheckedItemPositions();
        ArrayList<FavModelItem> favitems = new ArrayList<FavModelItem>();
        for (int i = 0; i < gridview.getCount(); ++i) {
            if (selectedIds.get(i)) {
                FavModelItem favitem = (FavModelItem) gridview.getItemAtPosition(i);
                favitems.add(favitem);
            }
        }
        return favitems;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
        ViewHolder vh = (ViewHolder) v.getTag();
        FavModelItem data = vh.data;
        if (data.sceneid == null) {
            SceneSelectDialogFragment sceneSelectDialogFragment = new SceneSelectDialogFragment();
            sceneSelectDialogFragment.setSceneSelectedListener(this);
            sceneSelectDialogFragment.show(getFragmentManager(), "sceneSelectDialogFragment");
            return;
        }
        LedApplication.getSceneProtocol().executeScene(data.sceneid);
        Toast.makeText(getActivity(), "Execute: " + data.scenename, Toast.LENGTH_SHORT).show();

    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (resultCode != Activity.RESULT_OK || mContextActionMode == null)
            return;
        if (requestCode == ImageUtils.CROP_FROM_CAMERA) {
            Bundle extras = data.getExtras();
            if (extras == null)
                return;
            Bitmap bitmap = extras.getParcelable("data");
            if (bitmap == null) {
                Log.w("FAV", "Invalid bitmap!");
                return;
            }
            FavModel favmodel = FavModel.getInstance(getActivity());
            ArrayList<FavModelItem> favitems = getSelectedFavItems();
            for (FavModelItem favitem : favitems) {
                favmodel.putBitmap(favitem.sceneid, bitmap);
            }
            ImageUtils.removeTempFile(this);
            if (mContextActionMode != null)
                mContextActionMode.finish();
        } else if (requestCode == ImageUtils.PICK_FROM_CAMERA) {
            ImageUtils.doCrop(this, getActivity(), 120, 120);
        } else if (requestCode == ImageUtils.PICK_FROM_FILE) {
            ImageUtils.setImageURI(data.getData());
            ImageUtils.doCrop(this, getActivity(), 120, 120);
        }
    }

    @Override
    public void sceneSelected(String sceneid, String scenename) {
        if (FavModel.getInstance(getActivity()).setFavourite(sceneid, scenename)) {
            Toast.makeText(getActivity(), "Add Favorite: " + scenename, Toast.LENGTH_SHORT).show();
        }
    }
}