package com.zte.netanalysis;

import com.zte.netanalysis.capinfo.CapInfoDNS;
import com.zte.netanalysis.capinfo.CapInfoDNS.DnsTransInfo;
import com.zte.netanalysis.capinfo.DnsInfoObserver;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.ExpandableListView;
import android.widget.TextView;

public class DnsInfoActivity extends Activity{

	static private CapInfoDNS capInfoDNS;
	static public void setDNSInfo(CapInfoDNS info){
		capInfoDNS = info;
	}
	
	private DnsTransInfoListAdapter adapter = new DnsTransInfoListAdapter();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_dnsinfo);
		
		ExpandableListView view = (ExpandableListView)findViewById(R.id.dnstransinfolist);
		view.setGroupIndicator(null);
		if(null != capInfoDNS){
			capInfoDNS.registerObserver(adapter);
			view.setAdapter(adapter);
		}
	}
	
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		if(null != capInfoDNS){
			capInfoDNS.unregisterObserver(adapter);
		}
	}
	
	private class DnsTransInfoListAdapter 
		extends BaseExpandableListAdapter implements DnsInfoObserver{

		@Override
		public void onDnsTransInfoUpdated(CapInfoDNS info) {
			notifyDataSetChanged();			
		}
		
		@Override
		public int getGroupCount() {
			return capInfoDNS.getDnsTransInfos().size();
		}

		@Override
		public int getChildrenCount(int groupPosition) {
			return 1;
		}

		@Override
		public Object getGroup(int groupPosition) {
			return capInfoDNS.getDnsTransInfos().get(groupPosition);
		}

		@Override
		public Object getChild(int groupPosition, int childPosition) {
			return capInfoDNS.getDnsTransInfos().get(groupPosition);
		}

		@Override
		public long getGroupId(int groupPosition) {
			return 0;
		}

		@Override
		public long getChildId(int groupPosition, int childPosition) {
			return 0;
		}

		@Override
		public boolean hasStableIds() {
			return false;
		}

		@Override
		public View getGroupView(int groupPosition, boolean isExpanded,
				View convertView, ViewGroup parent) {
			if(null == convertView){
				convertView = View.inflate(DnsInfoActivity.this, R.layout.dnstranslist_groupview, null);
			}			
			DnsTransInfo info = (DnsTransInfo)getGroup(groupPosition);			
			TextView view1 = (TextView)convertView.findViewById(R.id.item1);
			view1.setText(info.hostName);			
			TextView view2 = (TextView)convertView.findViewById(R.id.item2);
			view2.setText(String.format("%dms", info.answerTime - info.queryTime));
			if(isExpanded){
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
			if(null == convertView){
				convertView = View.inflate(DnsInfoActivity.this, R.layout.dnstranslist_childview, null);
			}
			DnsTransInfo info = (DnsTransInfo)getGroup(groupPosition);			
			TextView view1 = (TextView)convertView.findViewById(R.id.item1);
			view1.setText(info.toString());
			return convertView;
		}

		@Override
		public boolean isChildSelectable(int groupPosition, int childPosition) {
			return false;
		}

		
		
	}
	
}
