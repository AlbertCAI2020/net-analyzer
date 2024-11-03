package com.zte.netanalysis.capinfo;

import java.util.AbstractList;
import com.zte.netanalysis.utils.TimeString;
import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.SparseArray;

public final class CapInfo {
	
	public static final String CAPINFO_TBL_NAME = "capinfo";
	
	public static void createDatabaseTable(SQLiteDatabase db){
		db.execSQL("CREATE TABLE IF NOT EXISTS " + CAPINFO_TBL_NAME +
				"(startTime INTEGER PRIMARY KEY, endTime INTEGER, url TEXT, actionType INTEGER)");
		CapInfoTCP.createDatabaseTable(db);
		CapInfoDNS.createDatabaseTable(db);
	}
	
	public static void upgradeDatabase(SQLiteDatabase db, int oldVersion, int newVersion){
		db.execSQL("DROP TABLE IF EXISTS " + CAPINFO_TBL_NAME);
		CapInfoTCP.upgradeDatabase(db, oldVersion, newVersion);
		CapInfoDNS.upgradeDatabase(db, oldVersion, newVersion);
		createDatabaseTable(db);
	}
	
	public boolean saveToDatabse(SQLiteDatabase db){		
		ContentValues values = new ContentValues();
		values.put("startTime", startTime);
		values.put("endTime", endTime);
		values.put("url", url);
		values.put("actionType", actionType);
		if(db.insert(CAPINFO_TBL_NAME, null, values) < 0){
			return false;
		}
		tcpinfo.saveToDatabse(startTime, db);
		dnsinfo.saveToDatabse(startTime, db);
		return true;
	}
	
	public static CapInfo loadFromDatabase(long startTime, SQLiteDatabase db){
		Cursor cursor = db.query(true, CAPINFO_TBL_NAME, null, String.format("startTime=%d", startTime), 
				null, null, null, null, null);
		if(cursor.getCount() != 1){
			return null;
		}
		cursor.moveToFirst();
		CapInfo info = new CapInfo();
		info.startTime = cursor.getLong(cursor.getColumnIndex("startTime"));
		info.endTime = cursor.getLong(cursor.getColumnIndex("endTime"));
		info.url = cursor.getString(cursor.getColumnIndex("url"));
		info.actionType = cursor.getInt(cursor.getColumnIndex("actionType"));
		info.tcpinfo = CapInfoTCP.loadFromDatabase(startTime, db);
		info.dnsinfo = CapInfoDNS.loadFromDatabase(startTime, db);
		cursor.close();
		return info;
	}
	
	public static class DbCursorWrapper extends AbstractList<CapInfo>{

		private SQLiteDatabase db;
		private Cursor cursor;
		private SparseArray<CapInfo> cache = new SparseArray<CapInfo>();
		
		public DbCursorWrapper(SQLiteDatabase db, Cursor cursor){
			this.db = db;
			this.cursor = cursor;
			cursor.moveToFirst();
		}
		
		@Override
		public CapInfo get(int location) {
			
			CapInfo info = cache.get(location);
			if(null != info){
				return info;
			}
			
			cursor.moveToPosition(location);
			
			info = new CapInfo();
			info.startTime = cursor.getLong(cursor.getColumnIndex("startTime"));
			info.endTime = cursor.getLong(cursor.getColumnIndex("endTime"));
			info.url = cursor.getString(cursor.getColumnIndex("url"));
			info.actionType = cursor.getInt(cursor.getColumnIndex("actionType"));
			info.tcpinfo = CapInfoTCP.loadFromDatabase(info.startTime, db);
			info.dnsinfo = CapInfoDNS.loadFromDatabase(info.startTime, db);
			
			cache.put(location, info);
			
			return info;
		}

		@Override
		public int size() {
			return cursor.getCount();
		}
		
		public void close(){
			cache.clear();
			cursor.close();
		}
	}
	
	public static CapInfo.DbCursorWrapper loadAllFromDatabase(SQLiteDatabase db){
		
		Cursor cursor = db.query(true, CAPINFO_TBL_NAME, null, null, 
				null, null, null, null, null);
		
		return new CapInfo.DbCursorWrapper(db, cursor);	
	}
	
	public static void deleteAll(SQLiteDatabase db){
		db.delete(CAPINFO_TBL_NAME, null, null);
		CapInfoTCP.deleteAll(db);
	}
	
	public static final int WEBPAGE_BROWSE = 1;
	public static final int FILE_DOWNLOAD = 2;
	public static final int MEDIA_PLAY = 3;

	private long startTime = 0;
	private long endTime = 0;
	private String url;
	private int actionType;
	private CapInfoIP ipinfo = new CapInfoIP();
	private CapInfoTCP tcpinfo = new CapInfoTCP(); 
	private CapInfoDNS dnsinfo = new CapInfoDNS();
	
	public String getUrl() {
		return url;
	}

	public void setUrl(String url) {
		this.url = url;
	}

	public int getActionType() {
		return actionType;
	}

	public void setActionType(int actionType) {
		this.actionType = actionType;
	}
	
	public String getActionName(){
		switch (actionType) {
		case WEBPAGE_BROWSE:
			return "webpage browse";
		case FILE_DOWNLOAD:
			return "file download";
		case MEDIA_PLAY:			
			return "media play";
		default:
			return "unknown";
		}
	}
	
	public CapInfoIP getCapInfoIP(){
		return ipinfo;
	}
	
	public CapInfoTCP getCapInfoTCP(){
		return tcpinfo;
	}
	
	public CapInfoDNS getCapInfoDNS(){
		return dnsinfo;
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
	
	public void clear(){
		startTime = 0;
		endTime =0;
		url = null;
		actionType = 0;
		ipinfo.clear();
		tcpinfo.clear();
		dnsinfo.clear();
	}
	
	public String toString(){
		StringBuilder strInfoBuilder =  new StringBuilder();
		strInfoBuilder.ensureCapacity(128);
		
		strInfoBuilder.append(String.format("Time: %s-%s\r\n", TimeString.format1(startTime),
				TimeString.format2(endTime)));
		strInfoBuilder.append(String.format("Url: %s\r\n", url));
		strInfoBuilder.append(String.format("Action: %s", getActionName()));
		
		return strInfoBuilder.toString();
	}
	
}
