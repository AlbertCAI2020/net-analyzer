package com.zte.netanalysis;

import java.util.ArrayList;

import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoDatabase;
import com.zte.netanalysis.captask.CapTask;
import com.zte.netanalysis.captask.CapTaskScheduler;
import com.zte.netanalysis.utils.TimeString;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.ActionMode;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class AutoTestActivity extends CapTaskActors.ActorActivity{

	CapInfoDatabase database = CapInfoDatabase.getInstance();
	CapTaskScheduler scheduler = CapTaskScheduler.getInstance();
	CapTaskActors actorsFactory = new CapTaskActors(this);
	
	ArrayList<CapTask> taskListData = new ArrayList<CapTask>();	
	ArrayList<CapTask> checkedTasks = new ArrayList<CapTask>();
	
	View ctrlArea;
	Button startBtn;
	ActionMode actionMode;
	
	ListView taskListView;
	TaskListAdapter adapter = new TaskListAdapter();
	
	CapTaskScheduler.Listener listener = new CapTaskScheduler.Listener(){

		@Override
		public void onTaskShouldStart(CapTask task) {
			// TODO Auto-generated method stub
			task.setCapInfo(new CapInfo());
		}
		
		@Override
		public void onTaskStarted(CapTask task) {
			adapter.notifyDataSetChanged();
		}

		@Override
		public void onTaskTimeOut(CapTask task) {
		}

		@Override
		public void onTaskProgressChange(CapTask task, int progress) {			
		}

		@Override
		public void onTaskFinished(CapTask task) {
			database.save(task.getCapInfo());
			task.setCapInfo(null);
			adapter.notifyDataSetChanged();
		}

		@Override
		public void onStarted() {
			startBtn.setText("Pause");
			startBtn.setCompoundDrawablesWithIntrinsicBounds(android.R.drawable.ic_media_pause, 0, 0, 0);
		}

		@Override
		public void onStopped() {
			startBtn.setText("Start");
			startBtn.setCompoundDrawablesWithIntrinsicBounds(android.R.drawable.ic_media_play, 0, 0, 0);
		}

		@Override
		public void onFinished() {
			Toast.makeText(AutoTestActivity.this, "All task completed", Toast.LENGTH_SHORT).show();
		}

		@Override
		public void onTaskFailed(CapTask task) {
			Toast.makeText(AutoTestActivity.this, "Run task failed", Toast.LENGTH_SHORT).show();
		}
			
	};
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON, 
				WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
		setContentView(R.layout.activity_autotest);

		ctrlArea = findViewById(R.id.control_area);
		startBtn = (Button)findViewById(R.id.startTask);
		taskListView = (ListView)findViewById(R.id.taskList);
		taskListView.setAdapter(adapter);
		
		startBtn.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if(!scheduler.isRunning()){
					scheduler.start();					
				}else{
					scheduler.stop();					
				}
			}
		});
		
		
		taskListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				if(scheduler.isRunning()){
					return;
				}
				Intent intent = new Intent(AutoTestActivity.this, CapInfoActivity.class);
				CapTask capTask = taskListData.get(position);
				long startTime = capTask.getStartTime();
				long endTime = capTask.getEndTime();
				if(0 != endTime){
					intent.putExtra("startTime", startTime);
					startActivity(intent);
				}
			}
		});
		
		taskListView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
			@Override
			public boolean onItemLongClick(AdapterView<?> parent, View view,
					int position, long id) {
				if(scheduler.isRunning() || actionMode != null){
					return false;
				}
				startActionMode(longclkCallback);
				return true;
			}
		});
		
		scheduler.setListener(listener);		
		
	}
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
		scheduler.stop();
		scheduler.removeAll();
	}
	
	private class OnTaskTypeSelect implements DialogInterface.OnClickListener{
		@Override
		public void onClick(DialogInterface dialog, int which) {
			CapTaskEditor editor = null;
			if(0 == which){
				editor = new CapTaskEditor(AutoTestActivity.this, CapInfo.WEBPAGE_BROWSE, new OnTaskEditorResult());
			}else if(1 == which){
				editor = new CapTaskEditor(AutoTestActivity.this, CapInfo.FILE_DOWNLOAD, new OnTaskEditorResult());
			}else if(2 == which){
				editor = new CapTaskEditor(AutoTestActivity.this, CapInfo.MEDIA_PLAY, new OnTaskEditorResult());
			}
			if(null != editor){
				editor.show();
			}
		}
	}	
	
	private class OnTaskEditorResult implements CapTaskEditor.OnResult{
		@Override
		public void onResult(int actionType, CapTaskEditor.Result result) {
			for(int i = 0; i < result.repeat; i++){
				CapTask task = new CapTask();
				task.setUrl(result.url);
				task.setTimeOut(result.timeout);
				task.setActor(actorsFactory.createActor(actionType, result.usrName, result.usrPwd));
				task.setCapInfo(new CapInfo());
				taskListData.add(task);
				scheduler.addTask(task);
			}
			adapter.notifyDataSetChanged();
			if(!taskListData.isEmpty()){
				startBtn.setEnabled(true);
			}
		}
	}
	
	private CompoundButton.OnCheckedChangeListener cbChgListener = new CompoundButton.OnCheckedChangeListener() {				
		@Override
		public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
			CapTask task = (CapTask)buttonView.getTag();
			if(isChecked){
				checkedTasks.add(task);
			}else{
				checkedTasks.remove(task);
			}
		}
	};
	
	private class TaskListAdapter extends BaseAdapter{

		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return taskListData.size();
		}

		@Override
		public Object getItem(int position) {
			// TODO Auto-generated method stub
			return taskListData.get(position);
		}

		@Override
		public long getItemId(int position) {
			// TODO Auto-generated method stub
			return 0;
		}

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			// TODO Auto-generated method stub
			CapTask task = (CapTask)getItem(position);
			
			if(null == convertView){
				convertView = View.inflate(AutoTestActivity.this, R.layout.captasklist_groupview, null);
			}
			
			TextView view1 = (TextView)convertView.findViewById(R.id.item1);
			view1.setText(TimeString.format2(task.getStartTime()));
			
			TextView view2 = (TextView)convertView.findViewById(R.id.item2);
			view2.setText(task.getUrl());
			
			TextView view3 = (TextView)convertView.findViewById(R.id.item3);			
			view3.setText(String.format("%.3fKB/s", task.getDlSpeed()));
			
			CheckBox cb = (CheckBox)convertView.findViewById(R.id.item4);
			cb.setTag(task);
			cb.setOnCheckedChangeListener(null);
			if(checkedTasks.contains(task)){
				cb.setChecked(true);
			}else{
				cb.setChecked(false);
			}
			cb.setOnCheckedChangeListener(cbChgListener);
			
			if(actionMode != null){
				cb.setVisibility(View.VISIBLE);
			}else{
				cb.setVisibility(View.GONE);
			}
			
			return convertView;
		}
	}
	
	@Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.autotest, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if(id == R.id.addtask){
        	addTask();
        }
        
        return super.onOptionsItemSelected(item);
    }
    
    private ActionMode.Callback longclkCallback = new ActionMode.Callback() {
		
		@Override
		public boolean onPrepareActionMode(ActionMode mode, Menu menu) {
			// TODO Auto-generated method stub
			return false;
		}
		
		@Override
		public void onDestroyActionMode(ActionMode mode) {
			ctrlArea.setVisibility(View.VISIBLE);
			checkedTasks.clear();
			actionMode = null;
		}
		
		@Override
		public boolean onCreateActionMode(ActionMode mode, Menu menu) {
			MenuInflater inflater = mode.getMenuInflater();
			inflater.inflate(R.menu.autotest_longclick, menu);
			ctrlArea.setVisibility(View.GONE);
			actionMode = mode;
			return true;
		}
		
		@Override
		public boolean onActionItemClicked(ActionMode mode, MenuItem item) {
			int id = item.getItemId();
			switch (id) {
			case R.id.deletetask:
				if(checkedTasks.size() == taskListData.size()){
					clearTask();
				}else{
					for(CapTask task : checkedTasks){
						deleteTask(task);
					}
				}
				checkedTasks.clear();
				break;
			case R.id.selectall:
				if(checkedTasks.size() != taskListData.size()){
					checkedTasks.clear();
					checkedTasks.addAll(taskListData);
				}else{
					checkedTasks.clear();
				}
				break;
			default:
				return false;
			}
			adapter.notifyDataSetChanged();
			return true;
		}
	};
	
	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK && actionMode != null){
			actionMode.finish();
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
    private void addTask(){
    	if(scheduler.isRunning()){
			return;
		}
		
		AlertDialog.Builder builder = new AlertDialog.Builder(AutoTestActivity.this);
		builder.setTitle("Please select:");
		builder.setItems(new String[]{"Webpage Browse",	"File Download", "Media Play"}, 
				 new OnTaskTypeSelect());
		builder.show();		
    }
    
    private void deleteTask(CapTask task){
    	if(!scheduler.isRunning()){
    		scheduler.removeTask(task);
			taskListData.remove(task);
    	}
    }
    private void clearTask(){
    	if(!scheduler.isRunning()){
			taskListData.clear();
			scheduler.removeAll();
		}
    }
}
