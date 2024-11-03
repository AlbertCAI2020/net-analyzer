package com.zte.netanalysis.capcontrol;

import java.io.IOException;


public class NamedPipe {
	
	public static final String PIPE_NAME_C2S = "c2s";
	public static final String PIPE_NAME_S2C = "s2c";
	
	public static final int PIPE_READ = 1;
	public static final int PIPE_WRITE = 2;
	
	public final boolean Open(String path, int mode){
		if(mode != PIPE_READ && mode != PIPE_WRITE){
			return false;
		}
		mMode = mode;
		return NativeOpen(path, mode);
	}
	
	public final void Close(){
		mMode = 0;
		NativeClose();
	}
	
	public final int Read(byte[] buf){
		int count = 0;
		try {			
			while(count < buf.length){
				buf[count++] = ReadByte();
			}
		} catch (IOException e) {
			// TODO: handle exception
		}
		return count;
	}
	
	public final int Write(byte[] buf) {
		return NativeWrite(buf, buf.length);
	}
	
	public final int WriteByte(byte b){	
		byte[] buffer = {b};
		return NativeWrite(buffer, 1);
	}
	
	public final byte ReadByte() throws IOException{
		byte[] buffer = {0};
		int count = NativeRead(buffer, 1);
		if(count < 0){
			throw new IOException("pipe read error");
		}else if(count == 0){
			throw new IOException("pipe read eof");
		}
		return buffer[0];
	}
	
	public final int WriteString(String str){
		try {
			byte[] buf = str.getBytes("UTF-8");
			int ret1 = NativeWrite(buf, buf.length);
			if(ret1 != buf.length){
				return ret1;
			}
			int ret2 = WriteByte((byte)0);
			if(1 != ret2){
				return ret2;
			}
			return ret1;
		} catch (IOException e) {
			// TODO: handle exception
			return 0;
		}
		
	}
	
	public final String ReadString(){
		
		StringBuilder strbuilder = new StringBuilder();
		try {
			byte[] buf = new byte[128];
			int readed = 0;
			byte b = 0;
			while (0 != (b = ReadByte())) {
				buf[readed++] = b;
				if(readed == 128){
					strbuilder.append(new String(buf, 0, 128, "UTF-8"));
					readed = 0;
				}
				
			}
			strbuilder.append(new String(buf, 0, readed, "UTF-8"));
		} catch (IOException e) {
			// TODO: handle exception
		}
		
		return strbuilder.toString();
	}
	
	public final int GetLastErr(){
		return mLastErr;
	}
	
	private native boolean NativeOpen(String path, int mode);
	private native void NativeClose();
	private native int NativeRead(byte[] buf, int count);
	private native int NativeWrite(byte[] buf, int count);
	
	private int mLastErr = 0;
	private int mFd = -1;
	private int mMode = 0;
}
