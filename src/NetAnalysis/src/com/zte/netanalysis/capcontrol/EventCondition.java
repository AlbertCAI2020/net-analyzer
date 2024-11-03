package com.zte.netanalysis.capcontrol;

import android.util.SparseArray;

public class EventCondition {
	
	private EventCondition(){
		
	}
	
	private static EventCondition instance = new EventCondition();
	
	static public EventCondition getInstance(){
		return instance;
	}
	
	private static class Condition{
		volatile boolean mbNotifyed = false;
		
		public boolean isNotifyed(){
			return mbNotifyed;
		}
		
		public void setNotifyed(){
			mbNotifyed = true;
		}
	}
	
	
	private SparseArray<Condition> mCondtions = new SparseArray<Condition>();
	private Integer mEventNoGenarator = Integer.valueOf(0);
	
	public int newEventNumber() {
		
		synchronized (mEventNoGenarator) {
			mEventNoGenarator += 1;
			return mEventNoGenarator;
		}
	}
	
	public boolean waitOn(int event){
		Condition cond = null;	
		synchronized (mCondtions) {
			cond = mCondtions.get(event);
			if(null == cond){
				cond = new Condition();
				mCondtions.put(event, cond);
			}
		}		
		synchronized (cond) {
			try {
				while(!cond.isNotifyed()){
					cond.wait();
				}
			} catch (InterruptedException e) {
			}			
		}		
		synchronized (mCondtions) {
			mCondtions.remove(event);
		}		
		return cond.isNotifyed();
	}
	
	public boolean waitOn(int event, long millis){		
		Condition cond = null;	
		synchronized (mCondtions) {
			cond = mCondtions.get(event);
			if(null == cond){
				cond = new Condition();
				mCondtions.put(event, cond);
			}
		}	
		synchronized (cond) {
			try {
				cond.wait(millis);				
			} catch (InterruptedException e) {							
			}			
		}		
		synchronized (mCondtions) {
			mCondtions.remove(event);
		}		
		return cond.isNotifyed();
	}
	
	public void notify(int event){		
		Condition cond = null;		
		synchronized (mCondtions) {			
			cond = mCondtions.get(event);
			if(null == cond){
				cond = new Condition();
				mCondtions.put(event, cond);
			}
		}
		synchronized (cond) {
			cond.setNotifyed();
			cond.notify();
		}
	}

}
