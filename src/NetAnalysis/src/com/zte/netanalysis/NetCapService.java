package com.zte.netanalysis;

import java.io.IOException;

import com.stericson.RootTools.RootTools;
import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;

import android.widget.Toast;

public class NetCapService extends Service{

	static final String NETCAP_NAME = "netcap";	
	
	private boolean serverStarted = false;
	private SvrBinder svrBinder = new SvrBinder();
	
	
	@Override
	public IBinder onBind(Intent intent) {
		return svrBinder;
	}
	
	public class SvrBinder extends Binder{

		public boolean isServerStarted(){
			return serverStarted;
		}
	}
		
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		if(!serverStarted){
			serverStarted = startNetCapServer();
		}
		return super.onStartCommand(intent, flags, startId);
	}
	
	@Override
	public void onCreate() {
		super.onCreate();
		serverStarted = startNetCapServer();		
	}
	
	@Override
	public void onDestroy() {
		super.onDestroy();
		stopNetCapServer();
	}	
	
	
	private boolean startNetCapServer(){
		if(!RootTools.isRootAvailable()){
			Toast.makeText(this, "root unavailable!", Toast.LENGTH_LONG).show();
			return false;
		}
    	String fileDir = getFilesDir().getAbsolutePath();
    	String procName = fileDir + "/" + NETCAP_NAME;
	    if(RootTools.isProcessRunning(procName)){
	    	RootTools.killProcess(procName);
	    }
	    
	    boolean success = false;
	    boolean retry = false;
	    do{
		    if(!RootTools.installBinary(this, R.raw.netcap, NETCAP_NAME)){
		    	Toast.makeText(this, "install netcap failed!", Toast.LENGTH_SHORT).show();
		    	break;
		    }else{
		    	RootTools.runBinary(this, NETCAP_NAME, fileDir);
		    	if(!RootTools.isProcessRunning(procName)){
		    		if(!retry){
		    			RootTools.deleteFileOrDirectory(procName, false);
		    			retry = true;
		    			continue;
		    		}
		    		Toast.makeText(this, "run netcap failed!", Toast.LENGTH_SHORT).show();
		    		break;
			    }else{
			    	success = true;
			    	break;
			    }
		    }
	    }
	    while(true);
	    
	    return success;
	}
	
	private void stopNetCapServer(){
		String fileDir = getFilesDir().getAbsolutePath();
    	String procName = fileDir + "/" + NETCAP_NAME;
	    if(RootTools.isProcessRunning(procName)){
	    	RootTools.killProcess(procName);
	    }
	    try {
			RootTools.closeAllShells();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	

}
