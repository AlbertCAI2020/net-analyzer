package com.zte.netanalysis.capinfo;

import java.util.ArrayList;


public final class CapInfoIP {

	private int dlTotalPkts = 0;
	private int dlTotalBytes = 0;
	private ArrayList<InterStat> dlIntervalStats = new ArrayList<InterStat>();
	
	private ArrayList<IpInfoObserver> observers = new ArrayList<IpInfoObserver>();	

	public void registerObserver(IpInfoObserver ob)
	{
		if(null == ob || observers.contains(ob)){
			return;
		}
		observers.add(ob);
	}
	
	public void unregisterObserver(IpInfoObserver ob){
		if(null == ob || !observers.contains(ob)){
			return;
		}
		observers.remove(ob);
	}
	
	public void notifyIpStatInfoChanged()
	{
		for(IpInfoObserver ob : observers){
			ob.onIpStatInfoUpdate(this);
		}
	}
	
	public static class InterStat{
		public long beginTime = 0;
		public long endTime = 0;
		public int pktNum = 0;
		public int bytes = 0;
	}
	
	public void clear(){
		dlTotalPkts = 0;
		dlTotalBytes = 0;
		dlIntervalStats.clear();
	}
	
	public void addDlIntervalStat(InterStat stat){
		dlTotalPkts += stat.pktNum;
		dlTotalBytes += stat.bytes;
		dlIntervalStats.add(stat);
	}
	
	public int getDlTotalPkts() {
		return dlTotalPkts;
	}

	public int getDlTotalBytes() {
		return dlTotalBytes;
	}

}
