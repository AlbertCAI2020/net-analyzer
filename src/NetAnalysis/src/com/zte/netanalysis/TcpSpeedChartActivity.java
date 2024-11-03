package com.zte.netanalysis;

import com.zte.netanalysis.capinfo.CapInfoTCP;
import com.zte.netanalysis.capinfo.TcpInfoObserver;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class TcpSpeedChartActivity extends Activity {

	private static CapInfoTCP capInfoTCP;
	
	static public void setTcpInfo(CapInfoTCP info){
		capInfoTCP = info;
	}
	
	private TcpInfoObserver observer = new TcpInfoObserverImpl();
	private CapInfoTCP.IntervalStats data;
	private int sessionId;
	
	private TcpSpeedChartView tcpSpeedChart;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_tcpspeedchart);
		tcpSpeedChart = (TcpSpeedChartView)findViewById(R.id.tcp_speed_chart);
		
		if(null == capInfoTCP){
			return;
		}
		
		Intent intent = getIntent();
		int sesId = intent.getIntExtra("TcpSessionId", 0);
		if(sesId == 0){
			data = capInfoTCP.getDlIntervalStats();
		}else{
			CapInfoTCP.TcpSession session = capInfoTCP.getTcpSession(sesId);
			if(null != session){
				data = session.getDlIntervalStats();
			}
		}
		tcpSpeedChart.bindData(data);
		capInfoTCP.registerObserver(observer);
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		if(null != capInfoTCP){
			capInfoTCP.unregisterObserver(observer);
		}
	}
	
	private boolean mbForground;
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		mbForground = true;
		tcpSpeedChart.refresh();
	}
	
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		mbForground = false;
	}
	
	private class TcpInfoObserverImpl implements TcpInfoObserver{

		public void onTcpSesInfoUpdate(int sesId, CapInfoTCP info){			
		}
		
		public void onTcpStatInfoUpdate(int sesId, CapInfoTCP info){
			if(sesId == sessionId && mbForground){
				tcpSpeedChart.refresh();
			}
		}
	}
}
