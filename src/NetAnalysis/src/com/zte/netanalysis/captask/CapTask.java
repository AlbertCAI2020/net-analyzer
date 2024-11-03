package com.zte.netanalysis.captask;

import com.zte.netanalysis.capinfo.CapInfo;

public class CapTask {

	protected String url;
	protected long timeOut;
	protected InternetActor actor;
	protected CapInfo capInfo;
	
	protected int progress;
	protected boolean running;	
	protected long startTime;
	protected long endTime;	
	protected double dlSpeed;
	
	public InternetActor getActor() {
		return actor;
	}
	public void setActor(InternetActor actor) {
		this.actor = actor;
		if(null != capInfo && null != actor){
			capInfo.setActionType(actor.getActionType());
		}
	}
	public CapInfo getCapInfo() {
		return capInfo;
	}
	
	public void setCapInfo(CapInfo info){
		capInfo = info;
		if(null != capInfo){
			capInfo.setUrl(url);
			if(null != actor){
				capInfo.setActionType(actor.getActionType());
			}
		}
	}
	public String getUrl() {
		return url;
	}
	public void setUrl(String url) {
		this.url = url;
		if(null != capInfo){
			capInfo.setUrl(url);
		}
	}
	
	public long getTimeOut() {
		return timeOut;
	}
	public void setTimeOut(long maxTime) {
		this.timeOut = maxTime;
	}
	
	public boolean isRunning() {
		return running;
	}
	public void setRunning(boolean running) {
		this.running = running;
	}
	
	public long getStartTime() {
		return startTime;
	}
	public void setStartTime(long startTime) {
		this.startTime = startTime;
	}
	public long getEndTime() {
		return endTime;
	}
	public void setEndTime(long endTime) {
		this.endTime = endTime;
	}
	public double getDlSpeed() {
		return dlSpeed;
	}
	public void setDlSpeed(double dlSpeed) {
		this.dlSpeed = dlSpeed;
	}
	
	public int getProgress() {
		return progress;
	}
	public void setProgress(int progress) {
		this.progress = progress;
	}
	
	public void reset(){
		running = false;
		progress = 0;
		capInfo = null;
		startTime = 0;
		endTime = 0;
		dlSpeed = 0;
	}
}
