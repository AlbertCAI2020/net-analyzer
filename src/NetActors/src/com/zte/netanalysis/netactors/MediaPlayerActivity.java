package com.zte.netanalysis.netactors;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.MediaController;
import android.widget.VideoView;

public class MediaPlayerActivity extends Activity{

	private VideoView videoView;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);		
		setContentView(R.layout.activity_mediaplayer);
		videoView = (VideoView)findViewById(R.id.videoview);
		
		MediaController controller = new MediaController(this);		
		videoView.setMediaController(controller);
		
		videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener(){
			@Override
			public void onCompletion(MediaPlayer mp) {
				Log.d("mediaplayer", "playing finished!");
				
				Intent intent = new Intent();
				setResult(0, intent);
				finish();	
			}
		});
		
		videoView.setOnErrorListener(new MediaPlayer.OnErrorListener() {			
			@Override
			public boolean onError(MediaPlayer mp, int what, int extra) {
				Log.d("mediaplayer", "error caused!");
				
				Intent intent = new Intent();
				setResult(0, intent);
				finish();
				return true;
			}
		});
		
		Intent intent = getIntent();
		String url = intent.getStringExtra("url");		
		Log.d("mediaplayer", String.format("url=%s",url));
		
		if(null != url){
			Uri uri = Uri.parse(url);
			videoView.setVideoURI(uri);		
			videoView.start();
		}

	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}
	
}
