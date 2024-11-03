package com.zte.netanalysis.captask;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

import com.zte.netanalysis.capcontrol.CaptureController;
import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.utils.Settings;
import com.zte.netanalysis.utils.TimeString;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public final class CapTaskScheduler {

	private static CapTaskScheduler instance = new CapTaskScheduler();
	private CapTaskScheduler(){		
	}
	public static CapTaskScheduler getInstance(){
		return instance;
	}
	
	private CaptureController capCtrl = CaptureController.getInstance();
	private ArrayList<CapTask> capTasks = new ArrayList<CapTask>();
	private CapTask currentTask = null;
	private int toRun = 0;
	private boolean running = false;
	
	public interface Listener {
		void onStarted();
		void onStopped();
		void onFinished();
		void onTaskShouldStart(CapTask task);
		void onTaskFailed(CapTask task);
		void onTaskStarted(CapTask task);
		void onTaskTimeOut(CapTask task);
		void onTaskProgressChange(CapTask task, int progress);
		void onTaskFinished(CapTask task);
	}
	private Listener listener;
	public void setListener(Listener listener) {
		this.listener = listener;
	}
	
	public interface ProgressSetter{
		void setCurrentTaskProgress(int progress);//progress: 0-100
		void setCurrentTaskFinished();
	}
	
	private class CurrentProgressSetter implements ProgressSetter{
		@Override
		public void setCurrentTaskProgress(int progress) {
			if(null != currentTask && progress > currentTask.getProgress()){
				currentTask.setProgress(progress);
				if(null != listener){
					listener.onTaskProgressChange(currentTask, progress);
				}
				if(progress >= 100){
					stopCurrentTask();
					checkNextTask();
				}
			}
		}

		@Override
		public void setCurrentTaskFinished() {
			setCurrentTaskProgress(100);			
		}
	}
	private ProgressSetter progressSetter = new CurrentProgressSetter();
	public ProgressSetter getProgressSetter(){
		return progressSetter;
	}

	private Handler timerHandler = new Handler(Looper.getMainLooper()){
		public void handleMessage(Message msg) {
			switch (msg.what) {
			case CAPTASK_TIMEOUT:				
				if(null != currentTask && null != listener){
					listener.onTaskTimeOut(currentTask);
				}
				stopCurrentTask();
				checkNextTask();
				break;				
			case CAPTASK_STARTNEXT:
				startNextTask();
				break;
			default:
				break;
			}
		}
	};
	
	private static final int CAPTASK_TIMEOUT  = 1;
	private static final int CAPTASK_STARTNEXT = 2;		
	private TimeOut currentTimeOut;
	private int taskInterval = 2000;
	private StartNext currentStartNext;
	
	public int getTaskInterval() {
		return taskInterval;
	}
	public void setTaskInterval(int taskInterval) {
		this.taskInterval = taskInterval;
	}
	
	private class TimeOut{
		private Timer timer = new Timer();
		private TimerTask task = new TimerTask() {				
			@Override
			public void run() {
				Message msg = timerHandler.obtainMessage();
				msg.what = CAPTASK_TIMEOUT;
				timerHandler.sendMessage(msg);		
			}
		};
		private long delay;
		
		TimeOut(long delay){
			this.delay = delay;
		}
		
		public void start(){
			timer.schedule(task, delay);
		}
		
		public void cancel(){
			task.cancel();
			timer.cancel();
		}
		
	}
	
	private class StartNext{
		private Timer timer = new Timer();
		private TimerTask task = new TimerTask() {				
			@Override
			public void run() {
				Message msg = timerHandler.obtainMessage();
				msg.what = CAPTASK_STARTNEXT;
				timerHandler.sendMessage(msg);					
			}
		};
		private long delay;
		
		StartNext(long delay){
			this.delay = delay;
		}
		public void start(){
			timer.schedule(task, delay);
		}
		
		public void cancel(){
			task.cancel();
			timer.cancel();
		}		
	}
	
	public boolean addTask(CapTask task){
		if (running || null == task) {
			return false;
		}
		capTasks.add(task);
		return true;
	}
	
	public boolean removeTask(CapTask task){
		if (running || null == task) {
			return false;
		}
		int index = capTasks.indexOf(task);
		if(index < 0){
			return false;
		}
		capTasks.remove(index);
		if(index < toRun){
			toRun--;
		}
		return true;
	}
	public boolean removeAll(){
		if (running) {
			return false;
		}
		capTasks.clear();
		toRun = 0;
		return true;
	}
	
	public boolean start(){
		if(running){
			return false;
		}
		if(startNextTask()){
			if(null != listener){
				listener.onStarted();
			}
			return true;
		}
		return false;
	}
	
	public void stop(){
		stopCurrentTask();
		running = false;
		if(null != listener){
			listener.onStopped();
		}
	}
	
	public boolean restart(){
		if (running) {
			return false;
		}
		toRun = 0;
		for (CapTask task : capTasks) {
			task.reset();
		}
		return start();
	}
	
	public boolean isRunning(){
		return running;
	}
		
	private boolean runTask(CapTask task) {
		if (task.isRunning()) {
			return false;
		}
		String pcapFile = null;
		Settings settings = Settings.getInstance();
		if(settings.isSavePcapFile()){
			pcapFile = settings.getPcapFileDirectory() + 
					TimeString.format3(System.currentTimeMillis()) + ".pcap";
		}
		if(!capCtrl.startCapture(task.getUrl(), pcapFile, task.getCapInfo(), 0)){
			return false;
		}
		InternetActor actor = task.getActor();
		if(null != actor){
			if(!actor.start(task)){
				capCtrl.stopCapture();
				return false;
			}
		}		
		task.setRunning(true);
		return true;
	}
	
	private boolean hasNextTask(){
		return toRun < capTasks.size();
	}
	
	private boolean startNextTask(){
		if (toRun < capTasks.size()) {
			CapTask task = capTasks.get(toRun);
			if(null != listener){
				listener.onTaskShouldStart(task);
			}
			if(!runTask(task)){
				running = false;
				if(null != listener){
					listener.onTaskFailed(task);
					listener.onStopped();
				}
				return false;
			}
			running = true;
			toRun++;
			CapInfo capInfo = task.getCapInfo();
			if(null != capInfo){
				task.setStartTime(capInfo.getStartTime());
			}
			currentTask = task;
			currentTimeOut = new TimeOut(task.getTimeOut());
			currentTimeOut.start();
			if (null != listener) {
				listener.onTaskStarted(currentTask);
			}
			return true;
		}else{
			running = false;
			return false;
		}
	}
	
	private void stopCurrentTask(){
		
		if(null != currentTimeOut){
			currentTimeOut.cancel();
			currentTimeOut = null;
		}
		if(null != currentStartNext){
			currentStartNext.cancel();
			currentStartNext = null;
		}
		
		if(null != currentTask){
			if (currentTask.isRunning()) {
				InternetActor actor = currentTask.getActor();
				if(null != actor){
					actor.terminate();
				}
				capCtrl.stopCapture();
				currentTask.setRunning(false);
				CapInfo capInfo = currentTask.getCapInfo();
				if(null != capInfo){
					currentTask.setDlSpeed(capInfo.getCapInfoTCP().getDlSumInfo().effectivSpeed());
					currentTask.setEndTime(capInfo.getEndTime());
				}
			}
			
			if(null != listener){
				listener.onTaskFinished(currentTask);
			}
			currentTask = null;
		}
		
	}
	
	private void checkNextTask(){
		if(hasNextTask()){
			currentStartNext = new StartNext(taskInterval);
			currentStartNext.start();
		}else{
			running = false;
			if(null != listener){
				listener.onStopped();
				listener.onFinished();
			}
		}
	}
	
}
