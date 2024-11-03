package com.zte.netanalysis;


import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.captask.CapTask;
import com.zte.netanalysis.captask.CapTaskScheduler;
import com.zte.netanalysis.captask.InternetActor;
import com.zte.netanalysis.utils.Settings;

import android.app.Activity;
import android.content.ActivityNotFoundException;
import android.content.Intent;
import android.widget.Toast;

public class CapTaskActors {

	public static final int ACTOR_CODE = 0xFFAC;
	
	public static class ActorActivity extends Activity{
		
		protected void onActivityResult(int requestCode, int resultCode, Intent data) {
			if(ACTOR_CODE == requestCode && null != data){				
				CapTaskScheduler scheduler = CapTaskScheduler.getInstance();
				CapTaskScheduler.ProgressSetter progressSetter = scheduler.getProgressSetter();
				progressSetter.setCurrentTaskFinished();				
			}
		}
	}
	
	private ActorActivity activity;
	private WebpageBrowseActor defaultWebpageBrowseActor;
	private MediaPlayerActor defaultMediaPlayerActor;
	
	public CapTaskActors(ActorActivity activity){
		this.activity = activity;
		defaultWebpageBrowseActor = new WebpageBrowseActor();
		defaultMediaPlayerActor = new MediaPlayerActor();
	}
	
	public InternetActor createActor(int type, String usrName, String usrPwd){		
		InternetActor actor = null;
		switch (type) {
		case CapInfo.WEBPAGE_BROWSE:
			actor = defaultWebpageBrowseActor;
			break;
		case CapInfo.FILE_DOWNLOAD:
			actor = new FileDownloadActor(usrName, usrPwd);
			break;
		case CapInfo.MEDIA_PLAY:
			actor = defaultMediaPlayerActor;
			break;
		default:
			break;
		}
		return actor;
	}
	
	private class WebpageBrowseActor implements InternetActor{
		
		@Override
		public int getActionType() {
			return CapInfo.WEBPAGE_BROWSE;
		}
		@Override
		public boolean start(CapTask task) {			
			Intent intent = new Intent();
			intent.setClassName("com.zte.netanalysis.netactors", "com.zte.netanalysis.netactors.MainActivity");
			intent.putExtra("url", task.getUrl());
			intent.putExtra("action", getActionType());
			intent.putExtra("autotest", true);
			intent.putExtra("timeout", task.getTimeOut());
			try {
				activity.startActivityForResult(intent, ACTOR_CODE);
			} catch (ActivityNotFoundException e) {
				Toast.makeText(activity, e.getMessage(), Toast.LENGTH_LONG).show();
				return false;
			}
			return true;
		}
		@Override
		public void terminate() {			
		}		
	}
	
	private class FileDownloadActor implements InternetActor{
		
		private String usrName;
		private String usrPwd;
		
		public FileDownloadActor(String usrName, String usrPwd){
			this.usrName = usrName;
			this.usrPwd = usrPwd;
		}
		
		@Override
		public int getActionType() {			
			return CapInfo.FILE_DOWNLOAD;
		}
		@Override
		public boolean start(CapTask task) {
			
			Settings settings = Settings.getInstance();
			
			Intent intent = new Intent();
			intent.setClassName("com.zte.netanalysis.netactors", "com.zte.netanalysis.netactors.MainActivity");
			intent.putExtra("url", task.getUrl());
			intent.putExtra("action", getActionType());
			intent.putExtra("autotest", true);
			intent.putExtra("timeout", task.getTimeOut());
			intent.putExtra("usrname", usrName);
			intent.putExtra("usrpwd", usrPwd);
			if(settings.isSaveDownloadFile()){
				intent.putExtra("savelocalfile", true);
				intent.putExtra("localdirectory", settings.getDownloadFileDirectory());
			}
			
			try {
				activity.startActivityForResult(intent, ACTOR_CODE);
			} catch (ActivityNotFoundException e) {
				Toast.makeText(activity, e.getMessage(), Toast.LENGTH_LONG).show();
				return false;
			}
			return true;
		}
		
		@Override
		public void terminate() {			
		}
	}
	
	private class MediaPlayerActor implements InternetActor{
		
		@Override
		public int getActionType() {
			return CapInfo.MEDIA_PLAY;
		}
		@Override
		public boolean start(CapTask task) {			
			Intent intent = new Intent();
			intent.setClassName("com.zte.netanalysis.netactors", "com.zte.netanalysis.netactors.MainActivity");
			intent.putExtra("url", task.getUrl());
			intent.putExtra("action", getActionType());
			intent.putExtra("autotest", true);
			intent.putExtra("timeout", task.getTimeOut());
			try {
				activity.startActivityForResult(intent, ACTOR_CODE);
			} catch (ActivityNotFoundException e) {
				Toast.makeText(activity, e.getMessage(), Toast.LENGTH_LONG).show();
				return false;
			}
			return true;
		}
		@Override
		public void terminate() {			
		}		
	}
}
