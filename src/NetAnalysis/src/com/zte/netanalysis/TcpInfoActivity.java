package com.zte.netanalysis;

import com.zte.netanalysis.capinfo.CapInfoTCP;
import com.zte.netanalysis.capinfo.TcpInfoObserver;
import com.zte.netanalysis.capinfo.CapInfoTCP.TcpSession;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ExpandableListView;
import android.widget.BaseExpandableListAdapter;
import android.widget.TextView;

public class TcpInfoActivity extends Activity{

	static private CapInfoTCP capInfoTCP;
	private TcpConnListAdapter adapter = new TcpConnListAdapter();
	
	static public void setTcpInfo(CapInfoTCP info){
		capInfoTCP = info;
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_tcpinfo);
		
		ExpandableListView view = (ExpandableListView)findViewById(R.id.tcpconnectionlist);
		view.setGroupIndicator(null);
		view.setOnChildClickListener(new ExpandableListView.OnChildClickListener() {			
			@Override
			public boolean onChildClick(ExpandableListView parent, View v,
					int groupPosition, int childPosition, long id) {
				
				TcpSession session = (TcpSession)parent.getItemAtPosition(groupPosition);
				if(session.getDlIntervalStats().isEmpty()){
					return true;
				}
				
				Intent intent = new Intent(TcpInfoActivity.this, TcpSpeedChartActivity.class);
				TcpSpeedChartActivity.setTcpInfo(capInfoTCP);				
				intent.putExtra("TcpSessionId", session.sessionId);
				startActivity(intent);
				return true;
			}
		});
		
		if(null != capInfoTCP){
			capInfoTCP.registerObserver(adapter);
			view.setAdapter(adapter);
		}
	}
	
	 @Override
    protected void onDestroy() {
    	super.onDestroy();
    	if(null != capInfoTCP){
    		capInfoTCP.unregisterObserver(adapter);
    	}
    }
	

	private class TcpConnListAdapter extends BaseExpandableListAdapter 
		implements TcpInfoObserver{
		
		public void onTcpSesInfoUpdate(int sesId, CapInfoTCP info){
			notifyDataSetChanged();
		}
		public void onTcpStatInfoUpdate(int sesId, CapInfoTCP info){
			notifyDataSetChanged();
		}
    	
		@Override
		public int getGroupCount() {
			// TODO Auto-generated method stub
			return capInfoTCP.getTcpSessions().size();
		}

		@Override
		public int getChildrenCount(int groupPosition) {
			// TODO Auto-generated method stub
			return 1;
		}

		@Override
		public Object getGroup(int groupPosition) {
			// TODO Auto-generated method stub
			return capInfoTCP.getTcpSessions().get(groupPosition);
		}

		@Override
		public Object getChild(int groupPosition, int childPosition) {
			// TODO Auto-generated method stub
			return capInfoTCP.getTcpSessions().get(groupPosition);
		}

		@Override
		public long getGroupId(int groupPosition) {
			// TODO Auto-generated method stub
			return capInfoTCP.getTcpSessions().get(groupPosition).sessionId;
		}

		@Override
		public long getChildId(int groupPosition, int childPosition) {
			// TODO Auto-generated method stub
			return 1;
		}

		@Override
		public boolean hasStableIds() {
			// TODO Auto-generated method stub
			return true;
		}

		@Override
		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub					
			
			if(null == convertView){
				convertView = View.inflate(TcpInfoActivity.this, R.layout.tcpconnlist_groupview, null);
			}
			
			TcpSession session = (TcpSession)getGroup(groupPosition);
			
			TextView view1 = (TextView)convertView.findViewById(R.id.item1);
			view1.setText(String.format("%s:%d",
					session.remoteIp, session.remotePort));
			
			TextView view2 = (TextView)convertView.findViewById(R.id.item2);
			view2.setText(String.format("%dbytes", session.dl.totalBytes));
			
			
			if(isExpanded){
				//view2.setVisibility(View.GONE);
				view2.setVisibility(View.INVISIBLE);
			}
			else {
				view2.setVisibility(View.VISIBLE);
			}
			
			return convertView;
		}

		@Override
		public View getChildView(int groupPosition, int childPosition,
				boolean isLastChild, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			TcpSession session = (TcpSession)getGroup(groupPosition);
			
			if(null == convertView){
				convertView = View.inflate(TcpInfoActivity.this, R.layout.tcpconnlist_childview, null);
			}
			
			TextView view = (TextView)convertView.findViewById(R.id.item1);			
			view.setText(session.toString());
			return convertView;
		}

		@Override
		public boolean isChildSelectable(int groupPosition, int childPosition) {
			// TODO Auto-generated method stub
			return true;
		}
    	
    }
}
