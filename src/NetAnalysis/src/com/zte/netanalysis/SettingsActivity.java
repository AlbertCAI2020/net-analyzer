package com.zte.netanalysis;

import com.zte.netanalysis.utils.Settings;

import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.ListPreference;
import android.preference.PreferenceActivity;

public class SettingsActivity extends PreferenceActivity {

	private SettingsChangListener listener = new SettingsChangListener();
	
	private ListPreference tcpIntervalPrefs;
	
	@SuppressWarnings("deprecation")
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		addPreferencesFromResource(R.xml.settingspreference);
		
		SharedPreferences preferences = getPreferenceScreen().getSharedPreferences();
		preferences.registerOnSharedPreferenceChangeListener(listener);
		tcpIntervalPrefs = (ListPreference)findPreference("tcpStatInterval");
		//Toast.makeText(this, tcpIntervalPrefs.getValue(), Toast.LENGTH_SHORT).show();
	}
	
	@SuppressWarnings("deprecation")
	@Override
	protected void onDestroy() {
		// TODO Auto-generated method stub
		super.onDestroy();
		
		SharedPreferences preferences = getPreferenceScreen().getSharedPreferences();
		preferences.unregisterOnSharedPreferenceChangeListener(listener);
	}
	
	public class SettingsChangListener 
		implements SharedPreferences.OnSharedPreferenceChangeListener {
       
        public void onSharedPreferenceChanged(SharedPreferences sharedPreferences, String key)
        {
        	Settings settings = Settings.getInstance();
        	if(key.equals("savingPcapFile")){
        		boolean b = sharedPreferences.getBoolean(key, false);
        		settings.setSavePcapFile(b);
        	}else if(key.equals("tcpStatInterval")){
        		String str = sharedPreferences.getString(key, "1000");
        		settings.setTcpStatInterval(Integer.parseInt(str));
        	}else if(key.equals("savingDownload")){
        		boolean b = sharedPreferences.getBoolean(key, false);
        		settings.setSaveDownloadFile(b);
        	}
        }
    }

}
