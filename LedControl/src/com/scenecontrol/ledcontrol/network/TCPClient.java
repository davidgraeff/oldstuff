package com.scenecontrol.ledcontrol.network;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.security.KeyStore;

import javax.net.ssl.SSLSocket;

import org.apache.http.conn.ssl.SSLSocketFactory;

import com.scenecontrol.ledcontrol.LedApplication;
import com.scenecontrol.ledcontrol.R;

import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class TCPClient {
    public static final int READCODE = 1;
    public static final int DISCONNECTEDCODE = 2;
    public static final int CONNECTEDCODE = 3;
    private SocketThread m_socketThread = null;
    private Handler m_MainThreadHandler;
    
    public TCPClient(Handler h) {
        m_MainThreadHandler = h;
    }

    public class SocketThread implements Runnable
    {
        Handler mainThreadHandler = null;
        private Socket client = null;
        private BufferedReader in;
        private PrintWriter out;
        private boolean m_useSSL;
        private String m_server;
        private int m_port;
		private boolean inCleanup = false;

        private class SocketSendThread extends AsyncTask<String, Void, Void>{
			@Override
			protected Void doInBackground(String... datas) {
				for(String data:datas) {
	                if (out == null || out.checkError()) {
	                	closeConnection();
	                	break;
	                }
	                out.write(data+"\n");
	                out.flush();
				}
				if (out == null || out.checkError()) {
                	closeConnection();
                }
				return null;
			}
        }

        private class SocketCloseThread extends AsyncTask<Void, Void, Void>{
			@Override
			protected Void doInBackground(Void... datas) {
				if (inCleanup)
					return null;
				inCleanup=true;
	            try {
	            	if (client != null) {
	        			client.close();
	        		}
	            } catch (IOException e) {
	            }
	            try {
	            	if (in != null) {
	            		in.close();
	            	}
	            } catch (IOException e) {
	            }
	            if (out != null)
					out.close();
	            client = null;
	            in = null;
	            out = null;
	            if (mainThreadHandler!=null) {
		        	Message msg = Message.obtain();
		            msg.what = DISCONNECTEDCODE;
		            mainThreadHandler.sendMessage(msg);
		            mainThreadHandler = null;
	            }
	            m_socketThread = null;
	            inCleanup = false;
				return null;
			}
        }
        
        public SocketThread(Handler h, String server, int port, boolean useSSL)
        {
            mainThreadHandler = h;
           m_server = server;
           m_port = port;
           m_useSSL = useSSL;
        }
        
        public void closeConnection() {
        	new SocketCloseThread().execute();
        }
        
        public void write(String... datas) {
            if (isConnected())
            	new SocketSendThread().execute(datas);
        }

		public boolean isConnected() {
			return client != null && client.isConnected();
		}
		
        @Override
        public void run()
        {
            // Establish the remote endpoint for the socket.
            InetAddress serverAddr = null;

            try {
                serverAddr = InetAddress.getByName(m_server);
            } catch (UnknownHostException e) {
                e.printStackTrace();
                return;
            }

            SocketAddress sockaddr = new InetSocketAddress(serverAddr, m_port);
            
            // Create a TCP/IP socket and connect to the server.
            try {
            	if (m_useSSL) {
            		KeyStore localTrustStore = KeyStore.getInstance("BKS");
            		InputStream in = LedApplication.getAppContext().getResources().openRawResource(R.raw.truststore);
            		localTrustStore.load(in, "".toCharArray());
            		SSLSocketFactory sslSocketFactory = new SSLSocketFactory(localTrustStore);
            		client = sslSocketFactory.createSocket();
            		SSLSocket s = (SSLSocket)client;
            		s.setUseClientMode(true);
            		s.setEnableSessionCreation(true);
            		s.connect(sockaddr, 2000);
            		//s.startHandshake();
            	} else {
            		client = new Socket();
            		client.connect(sockaddr, 2000);
            	}
            	
        		out = new PrintWriter(client.getOutputStream(), true);
                in = new BufferedReader( new InputStreamReader(client.getInputStream()));

            } catch (SocketTimeoutException e) {
            	closeConnection();
                return;
            } catch (Exception  e) {
            	closeConnection();
                return; 
    		}

            {
	            Message msg = Message.obtain();
	            msg.obj = null;
	            msg.what = CONNECTEDCODE;

	            m_MainThreadHandler.sendMessage(msg);
            }

//            byte[] buffer = new byte[2500];
//            ByteArrayOutputStream a = new ByteArrayOutputStream();
//            a.
//            StringBuffer data = new StringBuffer();
        	String data;
            while ( !out.checkError() ){
                try {
                	data = in.readLine();
//                    int read = in.read(buffer);
                    if (data == null)
                    	continue;
                } catch (IOException e) {
                    break;
                }

	            if (mainThreadHandler==null)
	            	break;
	            
                // Post the received data back to the UI thread
            	Message msg = Message.obtain();
                msg.what = READCODE;
                msg.obj = data;
                mainThreadHandler.sendMessage(msg);
            } 
            
            closeConnection();
        }
    }

    public void Connect(String server, int port, boolean ssl) 
    {
    	if (m_socketThread==null) {
	        m_socketThread = new SocketThread(m_MainThreadHandler, server, port, ssl);
	
	        Thread thread = new Thread(m_socketThread);
	        thread.start();
    	}
    }

    public void CloseConnection() {
    	if (m_socketThread != null) {
    		m_socketThread.closeConnection();
    	}
    }

    public void write(String... datas)
    {
    	if (m_socketThread != null) {
    		m_socketThread.write(datas);
    	}
    }

	public boolean isConnected() {
		if (m_socketThread==null)
			return false;
		return m_socketThread.isConnected();
	}

	public void requestProperty(String id, String componentid, String componentinstance) {
    	if (m_socketThread != null) {
    		m_socketThread.write("{\"componentid_\":\"server\", \"type_\":\"execute\", \"method_\":" +
    		"\"requestProperty\",\"property\":{\"id_\":\""+id+"\",\"componentid_\":\""+componentid+"\",\"instanceid_\":\""+componentinstance+"\"}}");
    	}
	}
}