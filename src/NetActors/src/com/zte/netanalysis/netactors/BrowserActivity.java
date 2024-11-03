package com.zte.netanalysis.netactors;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class BrowserActivity extends Activity{

	private WebView webView;
	
	@SuppressLint("SetJavaScriptEnabled") @Override
	protected void onCreate(Bundle savedInstanceState) {
	// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_PROGRESS);
		
		setContentView(R.layout.activity_browser);		
		webView = (WebView)findViewById(R.id.webview);
		
		WebSettings settings = webView.getSettings();
		settings.setJavaScriptEnabled(true);
		settings.setAllowContentAccess(true);
		settings.setAllowFileAccess(true);
		settings.setCacheMode(WebSettings.LOAD_NO_CACHE);
		webView.clearCache(true);
		
		webView.setWebViewClient(new WebViewClient(){
			
			@Override
			public boolean shouldOverrideUrlLoading(WebView view, String url) {
				// TODO Auto-generated method stub
				return super.shouldOverrideUrlLoading(view, url);
			}
			
			@Override
			public void onPageStarted(WebView view, String url, Bitmap favicon) {
				// TODO Auto-generated method stub
				Log.d("webview", String.format("page started, url=%s", url));
				super.onPageStarted(view, url, favicon);
			}
			
			@Override
			public void onPageFinished(WebView view, String url) {
				// TODO Auto-generated method stub
				Log.d("webview", String.format("page finished, url=%s", url));
				Intent intent = new Intent();
				setResult(0, intent);
				finish();				
			}
			
			@Override
			public void onReceivedError(WebView view, int errorCode,
					String description, String failingUrl) {
				// TODO Auto-generated method stub
				Log.d("webview", String.format("error received, code=%d, url=%s", errorCode, failingUrl));
				super.onReceivedError(view, errorCode, description, failingUrl);
			}
		});
		
		webView.setWebChromeClient(new WebChromeClient(){
			
			@Override
			public void onProgressChanged(WebView view, int newProgress) {
				// TODO Auto-generated method stub
				BrowserActivity.this.setProgress(newProgress*100);
			}
			
			@Override
			public void onReceivedTitle(WebView view, String title) {
				// TODO Auto-generated method stub				
			}
		});
		
	
		Intent intent = getIntent();
		String url = intent.getStringExtra("url");
		Log.d("webview", String.format("url=%s",url));
		webView.loadUrl(url);
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		if(null != webView){
			webView.stopLoading();
		}
	}
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		if ((keyCode == KeyEvent.KEYCODE_BACK) &&
				webView.canGoBack()) {
			webView.goBack();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
}
