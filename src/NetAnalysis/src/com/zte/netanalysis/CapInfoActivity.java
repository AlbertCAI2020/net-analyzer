package com.zte.netanalysis;

import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoDNS;
import com.zte.netanalysis.capinfo.CapInfoDatabase;
import com.zte.netanalysis.capinfo.CapInfoTCP;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class CapInfoActivity extends Activity{
	
	CapInfo capInfo;
	CapInfoTCP capInfoTCP;
	CapInfoDNS capInfoDNS;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_capinfo);
		
		Intent intent = getIntent();
		long startTime = intent.getLongExtra("startTime", 0);
		
		CapInfoDatabase database = CapInfoDatabase.getInstance();
		capInfo = database.load(startTime);
		if(null == capInfo){
			return;
		}
		capInfoTCP = capInfo.getCapInfoTCP();
		capInfoDNS = capInfo.getCapInfoDNS();
		
		TextView generalInfoView = (TextView)findViewById(R.id.generalInfo);
		generalInfoView.setText(capInfo.toString());
		
		TextView dnsSumInfoView = (TextView)findViewById(R.id.dnsSumInfo);
		dnsSumInfoView.setText(capInfoDNS.toString());
		dnsSumInfoView.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(CapInfoActivity.this, DnsInfoActivity.class);
				DnsInfoActivity.setDNSInfo(capInfoDNS);
				startActivity(intent);
			}
		});
		
		TextView tcpSumInfoView = (TextView)findViewById(R.id.tcpSumInfo);
		tcpSumInfoView.setText(capInfoTCP.toString());
		tcpSumInfoView.setOnClickListener(new View.OnClickListener() {
				@Override
				public void onClick(View v) {
					Intent intent = new Intent(CapInfoActivity.this, TcpInfoActivity.class);
					TcpInfoActivity.setTcpInfo(capInfoTCP);				
					startActivity(intent);					
				}	        	
	    });
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
	}

}
