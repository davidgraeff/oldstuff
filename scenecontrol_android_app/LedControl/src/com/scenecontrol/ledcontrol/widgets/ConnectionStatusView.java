package com.scenecontrol.ledcontrol.widgets;

import android.content.Context;
import android.util.AttributeSet;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;
import com.scenecontrol.ledcontrol.network.ConnectDisconnectInterface;
import com.scenecontrol.ledcontrol.network.ConnectionObserver;
import com.scenecontrol.ledcontrol.network.ConnectionObserver.ConnectionObserverInterface;

public class ConnectionStatusView extends LinearLayout implements ConnectionObserverInterface, ConnectDisconnectInterface, OnClickListener {
    private EditText mUrl;
    private EditText mPort;
    private View view;

    public ConnectionStatusView(Context context) {
        super(context);
        doAdditionalConstructorWork();
    }

    public ConnectionStatusView(Context context, AttributeSet attrs) {
        super(context, attrs);
        LayoutInflater layoutInflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (layoutInflater != null) {
            view = layoutInflater.inflate(R.layout.connectionstatus, this, true);
        }
        doAdditionalConstructorWork();
    }

    public ConnectionStatusView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        doAdditionalConstructorWork();
    }

    private void doAdditionalConstructorWork() {
        mUrl = (EditText) view.findViewById(R.id.EditConnectionURL);
        mUrl.setText(LedApplication.getHost());
        mPort = (EditText) view.findViewById(R.id.EditConnectionPort);
        mPort.setText(Integer.valueOf(LedApplication.getPort()).toString());
        Button btnConnect = (Button) view.findViewById(R.id.ButtonConnect);
        btnConnect.setOnClickListener(this);
        LedApplication.getConnectionObserver().addListener(this);
        LedApplication.getServerconnection().addConnectionListener(this);
        if (LedApplication.getServerconnection().isConnected())
            executionServerConnected();
    }

    @Override
    public void connectityChanged(int state) {
        TextView item = (TextView) view.findViewById(R.id.no_connection_text);
        switch (state) {
            case ConnectionObserver.STATE_NOCONNECTION:
                break;
            case ConnectionObserver.STATE_WLANON:
                item.setText(R.string.connecting1_text);
                break;
            case ConnectionObserver.STATE_HOSTLOOK:
                item.setText(R.string.connecting2_text);
                break;
            case ConnectionObserver.STATE_CONNECTED:
                item.setText(R.string.connecting3_text);
                break;
        }
    }

    @Override
    public void onClick(View arg0) {
        LedApplication.setHost(mUrl.getText().toString());
        LedApplication.setPort(Integer.valueOf(mPort.getText().toString()).intValue());
        LedApplication.tryConnect();
    }

    @Override
    public void executionServerDisconnected() {
        TextView item = (TextView) view.findViewById(R.id.no_connection_text);
        this.setVisibility(View.VISIBLE);
        item.setText(R.string.no_connection_text);
    }

    @Override
    public void executionServerConnected() {
        this.setVisibility(View.GONE);
    }
}
