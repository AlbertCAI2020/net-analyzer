package com.zte.netanalysis;

import com.zte.netanalysis.capcontrol.CapEventObserver;
import com.zte.netanalysis.capcontrol.CaptureController;
import com.zte.netanalysis.capcontrol.Message.StartCapRes;
import com.zte.netanalysis.capcontrol.Message.StopCapRes;
import com.zte.netanalysis.capinfo.CapInfoDatabase;
import com.zte.netanalysis.utils.Settings;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

public class MainActivity extends Activity {

	static{
		System.loadLibrary("NamedPipe");
	}
	
	private ServiceConnection mCapServiceConnection = new ServiceConnection() {		
		@Override
		public void onServiceDisconnected(ComponentName name) {
		}
		
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			NetCapService.SvrBinder svr = (NetCapService.SvrBinder)(service);
			if(svr.isServerStarted()){
				initCapController();	        	
	        }
		}
	};
	
	private CaptureController mCapCtrl = CaptureController.getInstance();
	private ObserversImpl mObservers = new ObserversImpl();
	
	private class ObserversImpl implements CapEventObserver{
		
		public void onTxtMsgNotify(String msg){
			Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show();
		}
		
		public void onCaptureStarted(StartCapRes msg){
		}
		
		public void onCaptureStopped(StopCapRes msg){
		}
	}
	
	private void initCapController(){
		String fileDir = getFilesDir().getAbsolutePath();
    	if(!mCapCtrl.prepare(fileDir)){
	    	Toast.makeText(MainActivity.this, "initializing failed!", Toast.LENGTH_LONG).show();
	    }
    	mCapCtrl.registerObserver(mObservers);
	}
	
	private void initUI(){		
		setContentView(R.layout.activity_main);
		
		Button manTestBtn = (Button)findViewById(R.id.manualtest_button);
        manTestBtn.setOnClickListener(new View.OnClickListener(){
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, ManualTestActivity.class);
				startActivity(intent);
			}        	
        });
        
        Button autoTestBtn = (Button)findViewById(R.id.autotest_button);
        autoTestBtn.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {				
				Intent intent = new Intent(MainActivity.this, AutoTestActivity.class);
				startActivity(intent);
			}
		});
        
        Button settingsBtn = (Button)findViewById(R.id.settings_button);
        settingsBtn.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, SettingsActivity.class);
				startActivity(intent);
			}
		});
        
        Button historyRecordBtn = (Button)findViewById(R.id.historyrecord_button);
        historyRecordBtn.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(MainActivity.this, HistoryRecordActivity.class);
				startActivity(intent);
			}
		});
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);        
        initUI();
        
        Settings.getInstance().load(getApplicationContext());
        
        try {
        	CapInfoDatabase.getInstance().connect(getApplicationContext());
		} catch (Exception e) {
			Toast.makeText(this, e.getMessage(), Toast.LENGTH_LONG).show();
			Log.e("sqlite", e.getMessage());
		}
        
        Intent intent = new Intent(this, NetCapService.class);
		if(!bindService(intent, mCapServiceConnection, BIND_AUTO_CREATE)){
			Toast.makeText(this, "start cap service failed!", Toast.LENGTH_LONG).show();
		}        
        
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		mCapCtrl.unregisterObserver(mObservers);
		mCapCtrl.finallyQuit();
		CapInfoDatabase.getInstance().close();		
		unbindService(mCapServiceConnection);		
	}
    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
    	// TODO Auto-generated method stub
    	if (keyCode == KeyEvent.KEYCODE_BACK) {
    		//android.os.Process.killProcess(android.os.Process.myPid());
			//return true;
		}
    	return super.onKeyDown(keyCode, event);
    }
}
