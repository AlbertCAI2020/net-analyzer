package com.zte.netanalysis.capinfo;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.os.Environment;


public class CapInfoDatabase {
	
	public static final String NAME = "capinfo.db";
	public static final int VERSION = 11;
	
	private SQLiteDatabase database;
	private SQLiteOpenHelper dbHelper;
	
	private class DbAdapter extends SQLiteOpenHelper{
		
		public DbAdapter(Context context) {
			super(context, NAME, null, VERSION);
		}

		@Override
		public void onCreate(SQLiteDatabase db) {
			CapInfo.createDatabaseTable(db);
		}

		@Override
		public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
			CapInfo.upgradeDatabase(db, oldVersion, newVersion);
		}
		
	}
	
	private static CapInfoDatabase instance = new CapInfoDatabase();
	private CapInfoDatabase(){
	}
	public static CapInfoDatabase getInstance(){
		return instance;
	}
	
	public void connect(Context context){
		if(null == dbHelper){
			dbHelper = new DbAdapter(context);	
			database = dbHelper.getWritableDatabase();
		}
	}
	
	public void close(){
		if(null != dbHelper){
			dbHelper.close();
			database = null;
			dbHelper = null;
		}
	}
	
	public CapInfo load(long startTime){
		if(null == database){
			return null;
		}
		return CapInfo.loadFromDatabase(startTime, database);
	}
	
	public CapInfo.DbCursorWrapper loadAll(){
		if(null == database){
			return null;
		}
		return CapInfo.loadAllFromDatabase(database);
	}
	
	public boolean delete(CapInfo info){
		return false;
	}
	
	public void deleteAll(){
		if(null == database){
			return;
		}
		CapInfo.deleteAll(database);
	}
	
	public boolean save(CapInfo info){
		if(null == info || null == database){
			return false;
		}
		return info.saveToDatabse(database);
	}
	
	public boolean exportToExternalStorage(Context context, String file){
		if(null == context || null == file){
			return false;
		}
		try {
			String dbPath = "/data/data/com.zte.netanalysis/databases/"+ NAME;
			String outPath = Environment.getExternalStorageDirectory() + File.separator + file;
			File readFile = new File(dbPath);
			File writeFile = new File(outPath);
			
			InputStream is = new FileInputStream(readFile);
			FileOutputStream os = new FileOutputStream(writeFile);
			byte[] buffer = new byte[1024];
			int bytesread = 0;
			while (-1 != (bytesread = is.read(buffer))) {
				os.write(buffer, 0, bytesread);				
			}
			is.close();
			os.close();			
			
		} catch (Exception e) {
			return false;
		}
		
		return true;
	}
}
