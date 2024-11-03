package com.zte.netanalysis.utils;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public final class TimeString {

	static SimpleDateFormat format1 = new SimpleDateFormat(
			"MM/dd HH:mm:ss", Locale.CHINA);
	static SimpleDateFormat format2 = new SimpleDateFormat(
			"HH:mm:ss", Locale.CHINA);
	static SimpleDateFormat format3 = new SimpleDateFormat(
			"yyyyMMddHHmmssSSS", Locale.CHINA);
	static SimpleDateFormat format4 = new SimpleDateFormat(
			"yyyy-MM-dd", Locale.CHINA);
	
	static Date date = new Date();

	static public String format1(long time){
		if(0 == time){
			return NullTime1();
		}
		date.setTime(time);
		return format1.format(date);		
	}
	
	static public String NullTime1(){
		return "--/-- --:--:--";
	}
	
	static public String format2(long time){
		if(0 == time){
			return NullTime2();
		}
		date.setTime(time);
		return format2.format(date);		
	}
	
	static public String NullTime2(){
		return "--:--:--";
	}
	
	static public String format3(long time){
		date.setTime(time);
		return format3.format(date);
	}
	
	static public String format4(long time){
		date.setTime(time);
		return format4.format(date);
	}
}

