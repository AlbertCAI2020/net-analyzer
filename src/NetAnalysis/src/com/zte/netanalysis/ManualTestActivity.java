package com.zte.netanalysis;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.zte.netanalysis.capcontrol.CapEventObserver;
import com.zte.netanalysis.capcontrol.CaptureController;
import com.zte.netanalysis.capcontrol.Message.StartCapRes;
import com.zte.netanalysis.capcontrol.Message.StopCapRes;
import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoDNS;
import com.zte.netanalysis.capinfo.CapInfoDatabase;
import com.zte.netanalysis.capinfo.CapInfoTCP;
import com.zte.netanalysis.capinfo.DnsInfoObserver;
import com.zte.netanalysis.capinfo.TcpInfoObserver;
import com.zte.netanalysis.utils.Settings;
import com.zte.netanalysis.utils.TimeString;

public class ManualTestActivity extends Activity {

	private TextView ctrlBtn;
	private TextView generalInfoView;
	private TextView dnsSumInfoView;
	private TextView tcpSumInfoView;
	private TcpSpeedChartView tcpSpeedChart;
	
	private ObserversImpl mObservers = new ObserversImpl();
	CaptureController mCapCtrl = CaptureController.getInstance();
	CapInfo mCapInfo = new CapInfo();
	CapInfoDatabase database = CapInfoDatabase.getInstance();
	
	private class ObserversImpl implements CapEventObserver, 
		TcpInfoObserver, DnsInfoObserver{
		
		public void onTxtMsgNotify(String msg){
		}
		
		public void onCaptureStarted(StartCapRes msg){			
			Log.i("netcap", "capture started!");
			if(mbForground){
				refreshUI();
			}
		}
		
		public void onCaptureStopped(StopCapRes msg){			
			if(mbForground){
				refreshUI();
			}
			int dropped = msg.getDroppedPktNum();
			if(dropped != 0 ){
			Toast.makeText(ManualTestActivity.this, String.format("dropped packets:%d!", dropped),
					Toast.LENGTH_LONG).show();
			}
			Log.i("netcap", "capture stopped!");
		}

		public void onTcpSesInfoUpdate(int sesId, CapInfoTCP info){
			if(sesId == 0 && mbForground){
				refreshUI();
			}
		}
		
		public void onTcpStatInfoUpdate(int sesId, CapInfoTCP info){
			if(sesId == 0 && mbForground){
				refreshUI();			
			}
		}

		@Override
		public void onDnsTransInfoUpdated(CapInfoDNS info) {			
			if(mbForground){
				refreshUI();
			}
		}
		
	}	
	
	private void refreshUI(){		
		tcpSpeedChart.refresh();
		tcpSumInfoView.setText(mCapInfo.getCapInfoTCP().toString());
		dnsSumInfoView.setText(mCapInfo.getCapInfoDNS().toString());
		generalInfoView.setText(mCapInfo.toString());
	}
	
	private void initUI(){
		ctrlBtn = (TextView)findViewById(R.id.ctrlbutton);
        tcpSumInfoView = (TextView)findViewById(R.id.tcpSumInfo);        
        tcpSpeedChart = (TcpSpeedChartView)findViewById(R.id.chart);
        generalInfoView = (TextView)findViewById(R.id.generalInfo);
        dnsSumInfoView = (TextView)findViewById(R.id.dnsSumInfo);
        
		tcpSpeedChart.bindData(mCapInfo.getCapInfoTCP().getDlIntervalStats());
	}
	
	private void reset(){
		mCapInfo.clear();
		tcpSumInfoView.setText(mCapInfo.getCapInfoTCP().toString());
		tcpSpeedChart.bindData(mCapInfo.getCapInfoTCP().getDlIntervalStats());
	}
	
	private void onCtrlBtnClicked(){
		
		if(!mCapCtrl.isCapturing()){
			//to run
			reset();
			Settings settings = Settings.getInstance();
			String pcapFile = null;
			if(settings.isSavePcapFile()){
				pcapFile = settings.getPcapFileDirectory() + 
						TimeString.format3(System.currentTimeMillis()) + ".pcap";
			}
			int tcpInterval = settings.getTcpStatInterval();
			
			if(!mCapCtrl.startCapture(null, pcapFile, mCapInfo, tcpInterval)){
				Toast.makeText(this, "start capture failed!", Toast.LENGTH_LONG).show();
			}
			
		}else{
			//to stop
			mCapCtrl.stopCapture();
		}
		
		if(mCapCtrl.isCapturing()){
			ctrlBtn.setText(R.string.stopButton);			
		}else{
			ctrlBtn.setText(R.string.startButton);
		}
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_manualtest);
        
        mCapCtrl.setCurrentCapInfo(mCapInfo);
        mCapCtrl.registerObserver(mObservers);
        mCapInfo.getCapInfoTCP().registerObserver(mObservers);
        mCapInfo.getCapInfoDNS().registerObserver(mObservers);
        
        initUI();
        reset();
        
        ctrlBtn.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {				
				onCtrlBtnClicked();							
			}
		});
        
        dnsSumInfoView.setOnClickListener(new View.OnClickListener() {			
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(ManualTestActivity.this, DnsInfoActivity.class);
				DnsInfoActivity.setDNSInfo(mCapInfo.getCapInfoDNS());
				startActivity(intent);
			}
		});
        
        tcpSumInfoView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(ManualTestActivity.this, TcpInfoActivity.class);
				TcpInfoActivity.setTcpInfo(mCapInfo.getCapInfoTCP());
				startActivity(intent);
			}
        	
        });
    }
    
    private boolean mbForground = false;
    @Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		mbForground = true;		
		refreshUI();
	}
    
    @Override
    protected void onPause() {
    	// TODO Auto-generated method stub
    	super.onPause();
    	mbForground = false;
    }

    @Override
    protected void onDestroy() {
    	super.onDestroy();
    	mCapCtrl.stopCapture();
    	mCapCtrl.unregisterObserver(mObservers);
    	mCapInfo.getCapInfoTCP().unregisterObserver(mObservers);
    	mCapInfo.getCapInfoDNS().unregisterObserver(mObservers);
    }
    
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        //getMenuInflater().inflate(R.menu.manualtest, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
        	Intent intent = new Intent(this, SettingsActivity.class);
        	startActivity(intent);
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
