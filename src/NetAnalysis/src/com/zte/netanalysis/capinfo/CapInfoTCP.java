package com.zte.netanalysis.capinfo;


import java.util.ArrayList;
import java.util.List;

import com.zte.netanalysis.capcontrol.Message.TcpSessionNotify;

import android.content.ContentValues;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.util.SparseIntArray;

public final class CapInfoTCP {

	public static void createDatabaseTable(SQLiteDatabase db){
		TcpSession.createDatabaseTable(db);
		FlowInfo.createDatabaseTable(db);
	}
	
	public static void upgradeDatabase(SQLiteDatabase db, int oldVersion, int newVersion){
		TcpSession.upgradeDatabase(db, oldVersion, newVersion);
		FlowInfo.upgradeDatabase(db, oldVersion, newVersion);
	}
	
	public boolean saveToDatabse(long parentId, SQLiteDatabase db){		
		dlSumInfo.saveToDatabse(parentId, 0, db);
		for (TcpSession session : tcpSessionArray) {
			session.saveToDatabse(parentId, db);
		}
		return true;
	}
	
	public static CapInfoTCP loadFromDatabase(long parentId, SQLiteDatabase db){
		CapInfoTCP info = new CapInfoTCP();
		info.dlSumInfo = FlowInfo.loadFromDatabase(parentId, 0, db);
		info.tcpSessionArray = TcpSession.loadAllFromDataBase(parentId, db);
		for (int i = 0; i < info.tcpSessionArray.size(); i++) {
			int sesId = info.tcpSessionArray.get(i).sessionId;
			info.tcpSesIdIdx.put(sesId, i);
		}
		return info;
	}
	
	public static void deleteAll(SQLiteDatabase db){
		FlowInfo.deleteAll(db);
		TcpSession.deleteAll(db);
	}
	
	private IntervalStats dlIntervalStats = new IntervalStats();	
	private FlowInfo dlSumInfo = new FlowInfo();
	
	private ArrayList<TcpSession> tcpSessionArray = new ArrayList<TcpSession>();
	private SparseIntArray tcpSesIdIdx = new SparseIntArray();
		
	private ArrayList<TcpInfoObserver> observers = new ArrayList<TcpInfoObserver>();	

	public void registerObserver(TcpInfoObserver ob)
	{
		if(null == ob || observers.contains(ob)){
			return;
		}
		observers.add(ob);
	}
	
	public void unregisterObserver(TcpInfoObserver ob){
		if(null == ob || !observers.contains(ob)){
			return;
		}
		observers.remove(ob);
	}
	
	public void notifyTcpSesInfoChanged(int sesId)
	{
		for(TcpInfoObserver ob : observers){
			ob.onTcpSesInfoUpdate(sesId, this);
		}
	}
	public void notifyTcpStatInfoChanged(int sesId)
	{
		for(TcpInfoObserver ob : observers){
			ob.onTcpStatInfoUpdate(sesId, this);
		}
	}
	
	public static class InterStat{
		
		public long beginTime;
		public long endTime;
		public int pktNum;
		public int bytes;
		public int retransNum;
		public int retransBytes;
		public int retransAckedNum;
		public int retransAckedBytes;
		
		public void clear(){
			beginTime = 0;
			endTime = 0;
			pktNum = 0;
			bytes = 0;
			retransNum = 0;
			retransBytes = 0;
			retransAckedNum = 0;
			retransAckedBytes = 0;
		}
		
		public double effectivSpeed(){
			long interval = endTime-beginTime;
			if(0 != interval){
				return (double)(bytes-retransAckedBytes)/interval;
			}
			return 0.0;
		}
	}
	

	public static class IntervalStats{
		
		private ArrayList<InterStat> intervalStats = new ArrayList<InterStat>();
		
		public List<InterStat> getIntervalStats(){
			return intervalStats;
		}
		
		public void addIntervalStat(InterStat stat){
			intervalStats.add(stat);
		}
		
		public void clear(){
			intervalStats.clear();
		}
		
		public boolean isEmpty(){
			return intervalStats.isEmpty();
		}
	}
	
	
	public static class FlowInfo
	{
		public static final String DLFLOW_TBL_NAME = "dlflowinfo";
		
		public static void createDatabaseTable(SQLiteDatabase db){
			db.execSQL("CREATE TABLE IF NOT EXISTS " + DLFLOW_TBL_NAME +
					"(parentId INTEGER, sessionId INTEGER," +
					"beginTime INTEGER, endTime INTEGER, firstPayloadTime INTEGER, lastPayloadTime INTEGER," +
					"totalPktNum INTEGER, totalBytes INTEGER, retransPktNum INTEGER, retransBytes INTEGER," +
					"retransAckedNum INTEGER, retransAckedBytes INTEGER, fastRetransNum INTEGER, fastRetransBytes INTEGER," +
					"disorderPktNum INTEGER, disorderBytes INTEGER, dupAckNum INTEGER)");
		}
		
		public static void upgradeDatabase(SQLiteDatabase db, int oldVersion, int newVersion){
			db.execSQL("DROP TABLE IF EXISTS " + DLFLOW_TBL_NAME);
			createDatabaseTable(db);
		}
		
		public boolean saveToDatabse(long parentId, int sessionId, SQLiteDatabase db){		
			ContentValues values = new ContentValues();
			values.put("parentId", parentId);
			values.put("sessionId", sessionId);
			values.put("beginTime", beginTime);
			values.put("endTime", endTime);
			values.put("firstPayloadTime", firstPayloadTime);
			values.put("lastPayloadTime", lastPayloadTime);
			values.put("totalPktNum", totalPktNum);
			values.put("totalBytes", totalBytes);
			values.put("retransPktNum", retransPktNum);
			values.put("retransBytes", retransBytes);
			values.put("retransAckedNum", retransAckedNum);
			values.put("retransAckedBytes", retransAckedBytes);
			values.put("fastRetransNum", fastRetransNum);
			values.put("fastRetransBytes", fastRetransBytes);
			values.put("disorderPktNum", disorderPktNum);
			values.put("disorderBytes", disorderBytes);
			values.put("dupAckNum", dupAckNum);
		
			if(db.insert(DLFLOW_TBL_NAME, null, values) < 0){
				return false;
			}
			return true;
		}
		
		public static FlowInfo loadFromDatabase(long parentId, int sessionId, SQLiteDatabase db){
			Cursor cursor = db.query(true, DLFLOW_TBL_NAME, null, 
					String.format("parentId=%d AND sessionId=%d", parentId, sessionId),
					null, null, null, null, null);
			
			if(cursor.getCount() != 1){
				return null;
			}
			cursor.moveToFirst();
			FlowInfo info = new FlowInfo();
			info.beginTime = cursor.getLong(cursor.getColumnIndex("beginTime"));
			info.endTime = cursor.getLong(cursor.getColumnIndex("endTime"));
			info.firstPayloadTime = cursor.getLong(cursor.getColumnIndex("firstPayloadTime"));
			info.lastPayloadTime = cursor.getLong(cursor.getColumnIndex("lastPayloadTime"));
			info.totalPktNum = cursor.getInt(cursor.getColumnIndex("totalPktNum"));
			info.totalBytes = cursor.getInt(cursor.getColumnIndex("totalBytes"));
			info.retransPktNum = cursor.getInt(cursor.getColumnIndex("retransPktNum"));
			info.retransBytes = cursor.getInt(cursor.getColumnIndex("retransBytes"));
			info.retransAckedNum = cursor.getInt(cursor.getColumnIndex("retransAckedNum"));
			info.retransAckedBytes = cursor.getInt(cursor.getColumnIndex("retransAckedBytes"));
			info.fastRetransNum = cursor.getInt(cursor.getColumnIndex("fastRetransNum"));
			info.fastRetransBytes = cursor.getInt(cursor.getColumnIndex("fastRetransBytes"));
			info.disorderPktNum = cursor.getInt(cursor.getColumnIndex("disorderPktNum"));
			info.disorderBytes = cursor.getInt(cursor.getColumnIndex("disorderBytes"));			
			info.dupAckNum = cursor.getInt(cursor.getColumnIndex("dupAckNum"));
			cursor.close();
			return info;
		}
		
		public static void deleteAll(SQLiteDatabase db){
			db.delete(DLFLOW_TBL_NAME, null, null);
		}
		
		public long beginTime;
		public long endTime;
		public long firstPayloadTime;
		public long lastPayloadTime;

		public int totalPktNum;
		public int totalBytes;
		public int retransPktNum;
		public int retransBytes;
		public int retransAckedNum;
		public int retransAckedBytes;
		public int fastRetransNum;
		public int fastRetransBytes;
		public int disorderPktNum;
		public int disorderBytes;
		
		public int dupAckNum;
		
		public void clear(){
			beginTime = 0;
			endTime = 0;
			firstPayloadTime = 0;
			lastPayloadTime = 0;
			totalPktNum = 0;
			totalBytes = 0;
			retransPktNum = 0;
			retransBytes = 0;
			retransAckedNum = 0;
			retransAckedBytes = 0;
			fastRetransNum = 0;
			fastRetransBytes = 0;
			disorderPktNum = 0;
			disorderBytes = 0;
			dupAckNum = 0;
		}
		
		public void set(TcpSessionNotify.FlowInfo info){
			beginTime = info.getBeginTime();
			endTime = info.getEndTime();
			firstPayloadTime = info.getFirstPayloadTime();
			lastPayloadTime = info.getLastPayloadTime();			
			totalPktNum = info.getTotalPktNum();
			totalBytes = info.getTotalBytes();
			retransPktNum = info.getRetransPktNum();
			retransBytes = info.getRetransBytes();
			retransAckedNum = info.getRetransAckedNum();
			retransAckedBytes = info.getRetransAckedBytes();
			fastRetransNum = info.getFastRetransNum();
			fastRetransBytes = info.getFastRetransBytes();
			disorderPktNum = info.getDisorderPktNum();
			disorderBytes = info.getDisorderBytes();			
			dupAckNum = info.getDupAckNum();
		}
		
		public void add(InterStat stat){
			
			if(stat.beginTime != 0 &&
					(beginTime > stat.beginTime || beginTime == 0)){
				beginTime = stat.beginTime;
			}
			if(endTime < stat.endTime){
				endTime = stat.endTime;
			}
			if(stat.bytes != 0){
				if(stat.beginTime != 0 && 
						(firstPayloadTime > stat.beginTime || firstPayloadTime == 0)){
					firstPayloadTime = stat.beginTime;
				}
				if(lastPayloadTime < stat.endTime){
					lastPayloadTime = stat.endTime;
				}
			}
			totalPktNum += stat.pktNum;
			totalBytes += stat.bytes;
			retransPktNum += stat.retransNum;
			retransBytes += stat.retransBytes;
			retransAckedNum += stat.retransAckedNum;
			retransAckedBytes += stat.retransAckedBytes;
		}
		

		public double effectivSpeed(){
			long interval = lastPayloadTime-firstPayloadTime;
			if(0 != interval){
				return (double)(totalBytes-retransAckedBytes)/interval;
			}
			return 0.0;
		}
	};
	
	public static class TcpSession{
		public static final String TCPSESSION_TBL_NAME = "tcpsession";
		
		public static void createDatabaseTable(SQLiteDatabase db){
			db.execSQL("CREATE TABLE IF NOT EXISTS " + TCPSESSION_TBL_NAME +
					"(parentId INTEGER, sessionId INTEGER," +
					"localIp TEXT, remoteIp TEXT, localPort INTEGER, remotePort INTEGER, rtt INTEGER)");
		}
		
		public static void upgradeDatabase(SQLiteDatabase db, int oldVersion, int newVersion){
			db.execSQL("DROP TABLE IF EXISTS " + TCPSESSION_TBL_NAME);
			createDatabaseTable(db);
		}
		
		public boolean saveToDatabse(long parentId, SQLiteDatabase db){		
			ContentValues values = new ContentValues();
			values.put("parentId", parentId);
			values.put("sessionId", sessionId);
			values.put("localIp", localIp);
			values.put("remoteIp", remoteIp);
			values.put("localPort", localPort);
			values.put("remotePort", remotePort);
			values.put("rtt", rtt);
		
			if(db.insert(TCPSESSION_TBL_NAME, null, values) < 0){
				return false;
			}
			dl.saveToDatabse(parentId, sessionId, db);
			return true;
		}
		
		public static TcpSession loadFromDatabase(long parentId, int sessionId, SQLiteDatabase db){
			Cursor cursor = db.query(true, TCPSESSION_TBL_NAME, null, 
					String.format("parentId=%d AND sessionId=%d", parentId, sessionId),
					null, null, null, null, null);
			
			if(cursor.getCount() != 1){
				return null;
			}
			cursor.moveToFirst();
			TcpSession session = new TcpSession();
			session.sessionId = cursor.getInt(cursor.getColumnIndex("sessionId"));
			session.localIp = cursor.getString(cursor.getColumnIndex("localIp"));
			session.remoteIp = cursor.getString(cursor.getColumnIndex("remoteIp"));
			session.localPort = cursor.getInt(cursor.getColumnIndex("localPort"));			
			session.remotePort = cursor.getInt(cursor.getColumnIndex("remotePort"));
			session.rtt = cursor.getInt(cursor.getColumnIndex("rtt"));
			session.dl = FlowInfo.loadFromDatabase(parentId, sessionId, db);
			cursor.close();
			return session;
		}
		
		public static ArrayList<TcpSession> loadAllFromDataBase(long parentId, SQLiteDatabase db){
			ArrayList<TcpSession> list = new ArrayList<TcpSession>();
			
			Cursor cursor = db.query(true, TCPSESSION_TBL_NAME, null, 
					String.format("parentId=%d", parentId),
					null, null, null, null, null);

			if(!cursor.moveToFirst()){
				return list;
			}
			
			do {
				TcpSession session = new TcpSession();
				session.sessionId = cursor.getInt(cursor.getColumnIndex("sessionId"));
				session.localIp = cursor.getString(cursor.getColumnIndex("localIp"));
				session.remoteIp = cursor.getString(cursor.getColumnIndex("remoteIp"));
				session.localPort = cursor.getInt(cursor.getColumnIndex("localPort"));			
				session.remotePort = cursor.getInt(cursor.getColumnIndex("remotePort"));
				session.rtt = cursor.getInt(cursor.getColumnIndex("rtt"));
				session.dl = FlowInfo.loadFromDatabase(parentId, session.sessionId, db);
				list.add(session);
			} while (cursor.moveToNext());			
			cursor.close();
			return list;
		}
		
		public static void deleteAll(SQLiteDatabase db){
			db.delete(TCPSESSION_TBL_NAME, null, null);
		}
		
		public int sessionId;
		public String localIp;
		public String remoteIp;		
		public int localPort;		
		public int remotePort;

		public int rtt;
		
		public FlowInfo dl = new FlowInfo();
		public FlowInfo ul = new FlowInfo();
		
		private IntervalStats dlIntervalStats = new IntervalStats(){			
			public void addIntervalStat(InterStat stat){			
				dl.add(stat);
				super.addIntervalStat(stat);
			}			
		};		
		
		
		public IntervalStats getDlIntervalStats(){
			return dlIntervalStats;
		}
		
		public void setDlFlowInfo(TcpSessionNotify.FlowInfo info){
			dl.set(info);
		}
		
		public String toString(){
			StringBuilder strInfoBuilder =  new StringBuilder();
			strInfoBuilder.ensureCapacity(128);
			
			strInfoBuilder.append(String.format("Client: %s:%d\r\n", localIp, localPort));
			strInfoBuilder.append(String.format("Server: %s:%d\r\n", remoteIp, remotePort));
			strInfoBuilder.append(String.format("Total: %dpkts %dbytes\r\n", dl.totalPktNum, dl.totalBytes));
			strInfoBuilder.append(String.format("Retransmission: %dpkts %dbytes\r\n", dl.retransPktNum, dl.retransBytes));
			strInfoBuilder.append(String.format("Retrans-Acked: %dpkts %dbytes\r\n", dl.retransAckedNum, dl.retransAckedBytes));
			strInfoBuilder.append(String.format("Fast retrans: %dpkts %dbytes\r\n", dl.fastRetransNum, dl.fastRetransBytes));
			if(0 != dl.disorderPktNum){
				strInfoBuilder.append(String.format("Disorder: %dpkts %dbytes\r\n", dl.disorderPktNum, dl.disorderBytes));
			}
			if(0 != rtt){
				strInfoBuilder.append(String.format("RTT: %d ms\r\n", rtt));
			}	
			strInfoBuilder.append(String.format("Total time: %.3fs\r\n", 
					(double)(dl.endTime -dl.beginTime)/1000));					
			strInfoBuilder.append(String.format("Payload time: %.3fs\r\n", 
					(double)(dl.lastPayloadTime-dl.firstPayloadTime)/1000));
			if(0 != dl.lastPayloadTime - dl.firstPayloadTime){				
				strInfoBuilder.append(String.format("Average speed: %.3fKB/s\r\n", 
						(double)(dl.totalBytes-dl.retransAckedBytes)/(dl.lastPayloadTime-dl.firstPayloadTime)));
			}
			return strInfoBuilder.toString();
		}
	}
	
	public void clear(){
		dlIntervalStats.clear();
		dlSumInfo.clear();
		tcpSessionArray.clear();
		tcpSesIdIdx.clear();
	}
	
	public TcpSession addTcpSession(int id, String localIp, String remoteIp, 
			int localPort, int remotePort){
		TcpSession session = null;
		int sesIdx = tcpSesIdIdx.get(id, -1);
		if(-1 != sesIdx){
			session = tcpSessionArray.get(sesIdx);
		}else{
			session = new TcpSession();
			tcpSessionArray.add(session);
			tcpSesIdIdx.put(id, tcpSessionArray.size()-1);
		}
		session.sessionId = id;
		session.localIp = localIp;		
		session.remoteIp = remoteIp;
		session.localPort = localPort;
		session.remotePort = remotePort;
		return session;
	}
	
	
	public TcpSession getTcpSession(int sesId){
		int idx = tcpSesIdIdx.get(sesId, -1);
		if(-1 == idx){
			return null;
		}
		return tcpSessionArray.get(idx);
	}
	
	public List<TcpSession> getTcpSessions(){
		return tcpSessionArray;
	}
	
	public void addDlIntervalStat(int sessionId, InterStat stat){
		if(0 == sessionId){
			dlSumInfo.add(stat);
			dlIntervalStats.addIntervalStat(stat);
			return;
		}
		TcpSession session = getTcpSession(sessionId);
		if(null != session){
			session.dlIntervalStats.addIntervalStat(stat);
		}
	}
	
	public IntervalStats getDlIntervalStats(){
		return dlIntervalStats;
	}
	
	public FlowInfo getDlSumInfo(){
		return dlSumInfo;
	}
	
	public void setDlSumInfo(TcpSessionNotify.FlowInfo info){
		dlSumInfo.set(info);
	}
	
	public String toString()
	{
		StringBuilder strInfoBuilder =  new StringBuilder();
		strInfoBuilder.ensureCapacity(128);
		
		strInfoBuilder.append(String.format("TCP connections: %d\r\n", tcpSessionArray.size()));
		strInfoBuilder.append(String.format("Total: %dpkts %dbytes\r\n", dlSumInfo.totalPktNum, dlSumInfo.totalBytes));
		strInfoBuilder.append(String.format("Retransmission: %dpkts %dbytes\r\n", 
				dlSumInfo.retransPktNum, dlSumInfo.retransBytes));
		strInfoBuilder.append(String.format("Retrans-Acked: %dpkts %dbytes\r\n", 
				dlSumInfo.retransAckedNum, dlSumInfo.retransAckedBytes));
		strInfoBuilder.append(String.format("Total time: %.3fs\r\n", 
				(double)(dlSumInfo.endTime-dlSumInfo.beginTime)/1000));
		strInfoBuilder.append(String.format("Payload time: %.3fs\r\n", 
				(double)(dlSumInfo.lastPayloadTime-dlSumInfo.firstPayloadTime)/1000));		
		strInfoBuilder.append(String.format("Average speed: %.3fKB/s", dlSumInfo.effectivSpeed()));
	
		return strInfoBuilder.toString();
	}

}
