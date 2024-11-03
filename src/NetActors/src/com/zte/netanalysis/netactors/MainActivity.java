package com.zte.netanalysis.netactors;


import java.util.Timer;
import java.util.TimerTask;

import com.zte.netanalysis.netactors.R;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;

public class MainActivity extends Activity {
	
	private static final int CAPTASK_TIMEOUT  = 1;
	private boolean autotest = false;
	
	//action types
	public static final int WEBPAGE_BROWSE = 1;
	public static final int FILE_DOWNLOAD = 2;
	public static final int MEDIA_PLAY = 3;
	
	private Handler timerHandler = new Handler(Looper.getMainLooper()){
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case CAPTASK_TIMEOUT:
				Log.d("netactor", "time out!");
				Intent intent = new Intent();
				setResult(1, intent);
				finish();
				android.os.Process.killProcess(android.os.Process.myPid());
			default:
				break;
			}
		}
	};
	
	private class TimeOut{
		private Timer timer = new Timer();
		private TimerTask task = new TimerTask() {				
			@Override
			public void run() {
				Message msg = timerHandler.obtainMessage();
				msg.what = CAPTASK_TIMEOUT;
				timerHandler.sendMessage(msg);		
			}
		};
		private long delay;
		
		TimeOut(long delay){
			this.delay = delay;
		}
		
		public void start(){
			timer.schedule(task, delay);
		}
		
		public void cancel(){
			task.cancel();
			timer.cancel();
		}
		
	}
	
	@SuppressLint("SetJavaScriptEnabled") @Override
	protected void onCreate(Bundle savedInstanceState) {	
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		Intent intent = getIntent();
		int action = intent.getIntExtra("action", -1);
		autotest = intent.getBooleanExtra("autotest", false);
		startActionActivity(intent, action);
		if(autotest){
			long timeout = intent.getLongExtra("timeout", 1000);
			Log.d("netactor", String.format("timeout=%dms",timeout));
			new TimeOut(timeout).start();
		}
	}
	
	private void startActionActivity(Intent request, int action){
		
		Intent intent = null;
		if(action == WEBPAGE_BROWSE){
			intent = new Intent(request);
			intent.setClass(this, BrowserActivity.class);
		}else if(action == FILE_DOWNLOAD){
			intent = new Intent(request);
			intent.setClass(this, DownloaderActivity.class);
		}else if(action == MEDIA_PLAY){
			intent = new Intent(request);
			intent.setClass(this, MediaPlayerActivity.class);
		}
		
		if(intent != null){
			startActivityForResult(intent, 0);
		}
	}
	
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		if(autotest){
			Intent intent = new Intent();
			setResult(0, intent);
			finish();
			android.os.Process.killProcess(android.os.Process.myPid());
		}
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK){
			Intent intent = new Intent();
			setResult(2, intent);
			finish();
			android.os.Process.killProcess(android.os.Process.myPid());
		}
		return super.onKeyDown(keyCode, event);
	}

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
