package com.zte.netanalysis.utils;

import java.io.File;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.PreferenceManager;


public final class Settings {

	private Settings(){
		//String sdcard = Environment.getExternalStorageDirectory().getAbsolutePath();
		String sdcard = "/sdcard"; 
		pcapFileDirectory = sdcard + "/NetAnalysis/pcap/";
		downloadFileDirectory = sdcard + "/NetAnalysis/download/";
	}
	
	static private Settings instance = new Settings();
	static public Settings getInstance(){
		return instance;
	}
	
	public void load(Context context){
		SharedPreferences sharedPreferences = PreferenceManager.getDefaultSharedPreferences(context);    	
		savePcapFile = sharedPreferences.getBoolean("savingPcapFile", false);	
		String str = sharedPreferences.getString("tcpStatInterval", "1000");
		tcpStatInterval = Integer.parseInt(str);
		saveDownloadFile = sharedPreferences.getBoolean("savingDownload", false);
		
		File directory = new File(pcapFileDirectory);
		if(!directory.exists()){
			directory.mkdirs();
		}
		directory = new File(downloadFileDirectory);
		if(!directory.exists()){
			directory.mkdirs();
		}
	}
	
	private boolean savePcapFile = false;
	private boolean saveDownloadFile = false;	

	private String pcapFileDirectory;
	private String downloadFileDirectory;
	private int tcpStatInterval = 1000;
	
	public boolean isSavePcapFile(){
		return savePcapFile;
	}
	
	public void setSavePcapFile(boolean b){
		savePcapFile = b;
	}
	
	public String getPcapFileDirectory(){
		return pcapFileDirectory;
	}
	
	public void setPcapFileDirectory(String dir){
		pcapFileDirectory = dir;
	}
	
	public int getTcpStatInterval(){
		return tcpStatInterval;
	}
	
	public void  setTcpStatInterval(int interval){
		tcpStatInterval = interval;
	}
	
	public boolean isSaveDownloadFile() {
		return saveDownloadFile;
	}

	public void setSaveDownloadFile(boolean saveDownloadFile) {
		this.saveDownloadFile = saveDownloadFile;
	}
	
	public String getDownloadFileDirectory(){
		return downloadFileDirectory;
	}
	
}
