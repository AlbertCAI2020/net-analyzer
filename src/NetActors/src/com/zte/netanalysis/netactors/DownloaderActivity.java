package com.zte.netanalysis.netactors;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Window;
import android.widget.TextView;

public class DownloaderActivity extends Activity{

	private String strUrl;
	private String usrName;
	private String usrPwd;
	private String localDirectory;
	private boolean saveLocalFile;
	private Thread workThread;
	private DownLoadTask download;
	
	private TextView totalBytesTxt;
	private TextView progressTxt;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_PROGRESS);
		setContentView(R.layout.activity_downloader);
		
		totalBytesTxt = (TextView)findViewById(R.id.downloader_totalbytes);
		progressTxt = (TextView)findViewById(R.id.downloader_progress);
		
		Intent intent = getIntent();
		strUrl = intent.getStringExtra("url");
		usrName = intent.getStringExtra("usrname");
		usrPwd = intent.getStringExtra("usrpwd");		
		saveLocalFile = intent.getBooleanExtra("savelocalfile", false);
		if(saveLocalFile){
			localDirectory = intent.getStringExtra("localdirectory");
		}
		
		Log.d("downloader", String.format("url:%s\r\nuser:%s\r\npassword:%s\r\nlocal directory:%s\r\n",
				strUrl, usrName, usrPwd, localDirectory));
		
		startDownload();
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		if(download != null && null != workThread){
			download.stop();
			try {
				workThread.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	private void startDownload(){
		try{
			URL url = new URL(strUrl);
			
			String fileName = null;
			if(saveLocalFile){
				fileName = url.getPath();
				int pos = fileName.lastIndexOf('/');
				if(pos != -1){
					fileName = fileName.substring(pos+1);
				}
				fileName = localDirectory + fileName;
			}			
			
			if(!usrName.isEmpty() && !usrPwd.isEmpty()){
				String usrInfo = usrName  + ":" + usrPwd;
				StringBuilder builder = new StringBuilder();
				builder.append(url.getProtocol());
				builder.append("://");
				builder.append(usrInfo);
				builder.append("@");
				builder.append(url.getHost());
				if(-1 != url.getPort()){
					builder.append(":");
					builder.append(url.getPort());
				}
				builder.append(url.getFile());					
				url = new URL(builder.toString());
			}			
			
			download = new DownLoadTask(url, fileName);
			workThread = new Thread(download);
			workThread.start();
			Log.d("downloader", String.format("download started"));
			
		}catch(Exception e){
			e.printStackTrace();
		}
	}
	
	static final int TOTAL_BYTES = 1;
	static final int DOWNLOAD_PROGRESS = 2;
	static final int DOWNLOAD_FINISHED = 3;
	
	private Handler handler = new Handler(Looper.getMainLooper()){
		
		public void handleMessage(Message msg) {
			switch (msg.what) {
			
			case TOTAL_BYTES:
				int totalBytes = msg.arg1;
				totalBytesTxt.setText(String.format("Total Bytes: %d", totalBytes));
				Log.d("downloader", String.format("total bytes: %d", totalBytes));
				break;
				
			case DOWNLOAD_PROGRESS:
				int progress = msg.arg1;
				progressTxt.setText(String.format("Progress:%d%%", progress));
				DownloaderActivity.this.setProgress(progress*100);
				break;
				
			case DOWNLOAD_FINISHED:
				Log.d("downloader", String.format("download finished"));
				Intent intent = new Intent();
				setResult(0, intent);
				finish();
				break;
				
			default:
				break;
			}
	    }
	};
	
	class DownLoadTask implements Runnable{
	
		private URL url;
		private String localFile;
		private volatile boolean stop;
		
		DownLoadTask(URL url, String localFile){
			this.url = url;
			this.localFile = localFile;
		}
		
		public void stop(){
			stop = true;
		}
		@Override
		public void run() {
			
			try {
				
				URLConnection conn = url.openConnection();
				conn.setUseCaches(false);
				
				conn.connect();				
				InputStream inputStream = new BufferedInputStream(conn.getInputStream());
				
				int totalBytes = conn.getContentLength();
				if(totalBytes > 0){
					Message msg1 = handler.obtainMessage();
					msg1.what = TOTAL_BYTES;
					msg1.arg1 = totalBytes;
					handler.sendMessage(msg1);
				}
				
				FileOutputStream outputStream = null;				
				if(localFile != null){
					outputStream = new FileOutputStream(new File(localFile));
				}
				
				byte[] buff = new byte[1024];
				int downloadedBytes = 0;
				int downloadProgress = 0;
				
				do {
					int readed = inputStream.read(buff);
					if(readed < 0){
						break;
					}
					if(outputStream != null){
						outputStream.write(buff, 0, readed);
					}
					downloadedBytes += readed;
					
					if(totalBytes > 0){
						int progress  = downloadedBytes*100/totalBytes;
						if(progress > downloadProgress){
							downloadProgress = progress;
							Message msg2 = handler.obtainMessage();
							msg2.what = DOWNLOAD_PROGRESS;
							msg2.arg1 = downloadProgress;
							handler.sendMessage(msg2);
						}
					}
					
				} while (!stop);			
				
				inputStream.close();
				if(outputStream != null){
					outputStream.close();
				}
				
			} catch (Exception e) {
				e.printStackTrace();
			}
			if(!stop){
				Message msg3 = handler.obtainMessage();
				msg3.what = DOWNLOAD_FINISHED;
				handler.sendMessage(msg3);
			}
		}
		
	}
}
