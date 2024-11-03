package com.zte.netanalysis.capinfo;

import java.util.ArrayList;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

public final class CapInfoDNS {

	public static final String DNSINFO_TBL_NAME = "dnsinfo";
	
	public static void createDatabaseTable(SQLiteDatabase db){
		db.execSQL("CREATE TABLE IF NOT EXISTS " + DNSINFO_TBL_NAME +
				"(parentId INTEGER, retryCount INTEGER, queryTime INTEGER, answerTime INTEGER, " +
				"dnsServerIp TEXT, hostName TEXT, hostIpAddr TEXT)");
	}
	
	public static void upgradeDatabase(SQLiteDatabase db, int oldVersion, int newVersion){
		db.execSQL("DROP TABLE IF EXISTS " + DNSINFO_TBL_NAME);
		createDatabaseTable(db);
	}
	
	public boolean saveToDatabse(long parentId, SQLiteDatabase db){
		for (DnsTransInfo info : dnsTransInfos) {
			if(!info.saveToDatabse(parentId, db)){
				return false;
			}
		}
		return true;
	}
	
	public static CapInfoDNS loadFromDatabase(long parentId, SQLiteDatabase db){
		CapInfoDNS dnsinfo = new CapInfoDNS();
		ArrayList<DnsTransInfo> infos = DnsTransInfo.loadAllFromDataBase(parentId, db);
		for(DnsTransInfo info : infos){
			dnsinfo.addDnsTransInfo(info);
		}
		return dnsinfo;
	}
	
	public static class DnsTransInfo{
		public int retryCount;
		public long queryTime;
		public long answerTime;
		public String dnsServerIp;
		public String hostName;
		public String hostIpAddr;
		
		public boolean saveToDatabse(long parentId, SQLiteDatabase db){		
			ContentValues values = new ContentValues();
			values.put("parentId", parentId);
			values.put("retryCount", retryCount);
			values.put("queryTime", queryTime);
			values.put("answerTime", answerTime);
			values.put("dnsServerIp", dnsServerIp);
			values.put("hostName", hostName);
			values.put("hostIpAddr", hostIpAddr);

			if(db.insert(DNSINFO_TBL_NAME, null, values) < 0){
				return false;
			}
			return true;
		}
		
		public static ArrayList<DnsTransInfo> loadAllFromDataBase(long parentId, SQLiteDatabase db){
			ArrayList<DnsTransInfo> list = new ArrayList<DnsTransInfo>();
			
			Cursor cursor = db.query(true, DNSINFO_TBL_NAME, null, 
					String.format("parentId=%d", parentId),
					null, null, null, null, null);

			if(!cursor.moveToFirst()){
				return list;
			}
			
			do {
				DnsTransInfo info = new DnsTransInfo();
				info.retryCount = cursor.getInt(cursor.getColumnIndex("retryCount"));
				info.queryTime = cursor.getLong(cursor.getColumnIndex("queryTime"));
				info.answerTime = cursor.getLong(cursor.getColumnIndex("answerTime"));
				info.dnsServerIp = cursor.getString(cursor.getColumnIndex("dnsServerIp"));
				info.hostName = cursor.getString(cursor.getColumnIndex("hostName"));
				info.hostIpAddr = cursor.getString(cursor.getColumnIndex("hostIpAddr"));
				list.add(info);
			} while (cursor.moveToNext());			
			cursor.close();
			return list;
		}
		
		public String toString(){
			StringBuilder strInfoBuilder =  new StringBuilder();
			strInfoBuilder.ensureCapacity(128);
			strInfoBuilder.append(String.format("Host name: %s\r\n", hostName));
			strInfoBuilder.append(String.format("IP address: %s\r\n", hostIpAddr));
			strInfoBuilder.append(String.format("Used time: %dms\r\n", answerTime - queryTime));
			if(0 != retryCount){
				strInfoBuilder.append(String.format("Retry count: %d\r\n", retryCount));
			}
			strInfoBuilder.append(String.format("DNS Server: %s\r\n", dnsServerIp));
			return strInfoBuilder.toString();
		}
	}
	
	private ArrayList<DnsInfoObserver> observers = new ArrayList<DnsInfoObserver>();	

	public void registerObserver(DnsInfoObserver ob)
	{
		if(null == ob || observers.contains(ob)){
			return;
		}
		observers.add(ob);
	}
	
	public void unregisterObserver(DnsInfoObserver ob){
		if(null == ob || !observers.contains(ob)){
			return;
		}
		observers.remove(ob);
	}
	
	public void notifyDnsTransInfoChanged()
	{
		for(DnsInfoObserver ob : observers){
			ob.onDnsTransInfoUpdated(this);
		}
	}
	
	private ArrayList<DnsTransInfo> dnsTransInfos = new ArrayList<DnsTransInfo>();
	private long totalUsedTime;
	private int totalRetryCount;
	
	public void addDnsTransInfo(DnsTransInfo info){
		dnsTransInfos.add(info);		
		totalUsedTime += info.answerTime - info.queryTime;
		totalRetryCount += info.retryCount;
	}
	
	public void clear(){
		dnsTransInfos.clear();
		totalUsedTime = 0;
		totalRetryCount = 0;
	}
	
	public ArrayList<DnsTransInfo> getDnsTransInfos(){
		return dnsTransInfos;
	}
	
	public String toString(){
		StringBuilder strInfoBuilder =  new StringBuilder();
		strInfoBuilder.ensureCapacity(128);
		int num = dnsTransInfos.size();
		double total = (double)totalUsedTime/1000;
		double average = 0.0;
		if(num > 0){
			average = total/num;
		}
		strInfoBuilder.append(String.format("Name queries: %d\r\n", num));
		strInfoBuilder.append(String.format("Total used time: %.3fs\r\n",total));
		strInfoBuilder.append(String.format("Average used time: %.3fs\r\n",average));
		strInfoBuilder.append(String.format("Total retry count: %d",totalRetryCount));
		return strInfoBuilder.toString();
	}
}
