package com.zte.netanalysis;

import java.util.HashMap;

import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoDatabase;
import com.zte.netanalysis.utils.TimeString;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class HistoryRecordActivity extends Activity{

	private CapInfo.DbCursorWrapper capInfoList;
	private ListView capInfoListView;
	private BaseAdapter capInfoListAdapter = new CapInfoListAdapter();
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_historyrecord);
		
		capInfoListView = (ListView)findViewById(R.id.capinfolist);
		capInfoListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {				
				Intent intent = new Intent(HistoryRecordActivity.this, CapInfoActivity.class);
				CapInfo capInfo = capInfoList.get(position);
				long startTime = capInfo.getStartTime();
				long endTime = capInfo.getEndTime();
				if(0 != endTime){
					intent.putExtra("startTime", startTime);
					startActivity(intent);
				}
			}
		});
		
		capInfoList = CapInfoDatabase.getInstance().loadAll();
		capInfoListView.setAdapter(capInfoListAdapter);
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		if(capInfoList != null){
			capInfoList.close();
		}
	}
	
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.historyrecord, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if(id == R.id.cleardatabase){
        	clearAllRcds();
        }else if(id == R.id.exportdatabase){
        	exportDataBase();
        }
        
        return super.onOptionsItemSelected(item);
    }    
	
	void clearAllRcds(){
		AlertDialog.Builder builder = new AlertDialog.Builder(HistoryRecordActivity.this);
		builder.setTitle("Warnning");
		builder.setMessage("Are you sure? It's unrecoverable!");
		builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {					
			@Override
			public void onClick(DialogInterface dialog, int which) {						
				CapInfoDatabase.getInstance().deleteAll();
				if(capInfoList != null){
					capInfoList.close();
					capInfoList = CapInfoDatabase.getInstance().loadAll();
					capInfoListAdapter.notifyDataSetChanged();
				}
			}
		});
		builder.setNegativeButton("Cancel", null);
		builder.show();
	}
	
	void exportDataBase(){
		if(CapInfoDatabase.getInstance().
				exportToExternalStorage(getApplicationContext(),"capinfo.db")){
			Toast.makeText(HistoryRecordActivity.this, 
					"Export database to /sdcard/capinfo.db", Toast.LENGTH_SHORT).show();
		}else {
			Toast.makeText(HistoryRecordActivity.this, 
					"Export database failed", Toast.LENGTH_SHORT).show();
		}
	}
	
	private class CapInfoListAdapter extends BaseAdapter{

		private HashMap<String, Integer> showedDates = new HashMap<String, Integer>();
		
		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return capInfoList.size();
		}

		@Override
		public Object getItem(int position) {
			// TODO Auto-generated method stub
			return capInfoList.get(position);
		}

		@Override
		public long getItemId(int position) {
			// TODO Auto-generated method stub
			return capInfoList.get(position).getStartTime();
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			CapInfo capInfo = (CapInfo)getItem(position);
			
			if(null == convertView){
				convertView = View.inflate(HistoryRecordActivity.this, R.layout.historyrecordlist_groupview, null);
			}
			
			LinearLayout title = (LinearLayout)convertView.findViewById(R.id.title);
			TextView date = (TextView)title.findViewById(R.id.date);
			String dateString = TimeString.format4(capInfo.getStartTime());
			date.setText(dateString);
			
			if(!showedDates.containsKey(dateString)){
				showedDates.put(dateString, position);
			}
			if(showedDates.containsValue(position)){
				title.setVisibility(View.VISIBLE);
			}else{
				title.setVisibility(View.GONE);
			}
			
			TextView view1 = (TextView)convertView.findViewById(R.id.item1);
			view1.setText(TimeString.format2(capInfo.getStartTime()));
			TextView view2 = (TextView)convertView.findViewById(R.id.item2);
			view2.setText(capInfo.getUrl());
			TextView view3 = (TextView)convertView.findViewById(R.id.item3);
			double speed = capInfo.getCapInfoTCP().getDlSumInfo().effectivSpeed();
			view3.setText(String.format("%.3fKB/s", speed));
			CheckBox cb = (CheckBox)convertView.findViewById(R.id.item4);
			cb.setVisibility(View.GONE);
			
			return convertView;
		}
	}
	
	
	
}
