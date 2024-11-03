package com.zte.netanalysis.capcontrol;

public class MsgHeader {
	
	static public final int HDRBUFF_LEN = 8;
	
	private int length = 0;
	private int type = 0;
	
	public final void setLength(int len){
		length = len;
	}
	
	public final int getLength(){
		return length;
	}
	
	public final void setType(int t){
		type = t;
	}
	
	public final int getType(){
		return type;
	}
	
	public final boolean deserializeFrom(byte buf[]){
		if(buf.length < HDRBUFF_LEN){
			return false;
		}
		length = deserializeInt(buf, 0);
		type = deserializeInt(buf, 4);		
		return true;
	}
	
	public final byte[] serializeBytes(){
		byte[] buf = new byte[HDRBUFF_LEN];
		serializeInt(length, buf, 0);
		serializeInt(type, buf, 4);
		return buf;
	}
	
	private int deserializeInt(byte buf[], int offset){
		int ret = 0;
		int bits = 0;
		for(int i = offset; i < offset+4; i++){
			ret |= buf[i]<<bits;
			bits += 8;
		}
		return ret;
	}
	
	private void serializeInt(int i, byte buf[], int offset){		
		int bits = 0;
		for(int j = offset; j < offset+4; j++){
			buf[j] = (byte)(i >> bits);
			bits += 8;
		}
	}

}
