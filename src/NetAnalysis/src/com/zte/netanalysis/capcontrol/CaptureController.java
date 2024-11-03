package com.zte.netanalysis.capcontrol;

import android.os.Message;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.MessageLite;
import com.zte.netanalysis.capcontrol.Message.DnsTransNotify;
import com.zte.netanalysis.capcontrol.Message.IpStatNotify;
import com.zte.netanalysis.capcontrol.Message.MsgType;
import com.zte.netanalysis.capcontrol.Message.StartCapReq;
import com.zte.netanalysis.capcontrol.Message.StartCapRes;
import com.zte.netanalysis.capcontrol.Message.StopCapReq;
import com.zte.netanalysis.capcontrol.Message.StopCapRes;
import com.zte.netanalysis.capcontrol.Message.TcpSessionNotify;
import com.zte.netanalysis.capcontrol.Message.TcpStatNotify;
import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoTCP;

public final class CaptureController {
	
	private NamedPipe mReadpipe = new NamedPipe();
	private NamedPipe mWritepipe = new NamedPipe();
	
	private volatile boolean mbQuit = false;
	private volatile boolean mbPrepared = false;
	CapEventHandler mEventHandler = new CapEventHandler();
	CapInfo capInfo;
	
	private Thread mEventListenThread;
	
	private static CaptureController instance = new CaptureController();
	private CaptureController(){
		
	}
	public static CaptureController getInstance(){
		return instance;
	}
	
	public void registerObserver(CapEventObserver ob){
		mEventHandler.registerObserver(ob);
	}
	
	public void unregisterObserver(CapEventObserver ob){
		mEventHandler.unregisterObserver(ob);
	}
	
	//app 启动时调用
	public boolean prepare(String pipeDir){
		
		if(mbPrepared){
			return true;
		}
		
		if(!mWritepipe.Open(pipeDir + "/" + NamedPipe.PIPE_NAME_C2S, 
				NamedPipe.PIPE_WRITE)){
			return false;
		}
		
		if(!mReadpipe.Open(pipeDir + "/" + NamedPipe.PIPE_NAME_S2C, 
				NamedPipe.PIPE_READ)){
			return false;
		}
		
		mbQuit = false;
		mEventListenThread = new Thread(new CaptureEventListener());
		mEventListenThread.start();
		mbPrepared = true;
		return true;
	}
	
	//app 退出时调用
	public void finallyQuit(){
		mbQuit = true;
		mReadpipe.Close();
		mWritepipe.Close();
		try {
			if(null != mEventListenThread){
				mEventListenThread.join();
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		mbPrepared = false;
	}
	
	private volatile boolean mIsCapturing = false;
	private static EventCondition condition = EventCondition.getInstance();
	
	public boolean isCapturing(){
		return mIsCapturing;
	}
	
	private boolean sendRequest(MessageLite req, int type){
		
		byte[] data = req.toByteArray();
		
		MsgHeader hdr = new MsgHeader();
		hdr.setLength(data.length);
		hdr.setType(type);
		byte[] hdrbuf = hdr.serializeBytes();
		
		if(hdrbuf.length != mWritepipe.Write(hdrbuf) ||
				data.length != mWritepipe.Write(data))
		{
			return false;
		}
		return true;
	}
	
	public boolean startCapture(String url, String pcapFile, 
			CapInfo capInfo, int tcpInterval){
	
		if(!mbPrepared){
			return false;
		}
		
		if(mIsCapturing){
			return false;
		}
		if(null != capInfo ){
			setCurrentCapInfo(capInfo);
		}
		
		//发送开始抓包请求
		int eventNum = condition.newEventNumber();
		
		StartCapReq.Builder builder = StartCapReq.newBuilder();	
		builder.setSeq(eventNum);
		if(null != url){
			builder.setUrl(url);
		}
		if(null != pcapFile){
			builder.setSaveFile(pcapFile);
		}
		builder.setTcpStatInterval(tcpInterval);
		
		if(!sendRequest(builder.build(), MsgType.START_CAP_REQ_VALUE)){
			return false;
		}
		
		//等待响应到来
		condition.waitOn(eventNum);
		
		return mIsCapturing;
	}
	
	public boolean stopCapture(){
		if(!mbPrepared){
			return false;
		}
		
		if(!mIsCapturing){
			return false;
		}
		int eventNum = condition.newEventNumber();
		StopCapReq req = StopCapReq
				.newBuilder()
				.setSeq(eventNum)
				.build();
		if(!sendRequest(req, MsgType.STOP_CAP_REQ_VALUE)){
			return false;
		}
		condition.waitOn(eventNum);
		
		return !mIsCapturing;
	}
	
	public CapInfo getCurrentCapInfo(){
		return capInfo;
	}
	
	public void setCurrentCapInfo(CapInfo capInfo){
		this.capInfo = capInfo;
		mEventHandler.setCapInfo(capInfo);
	}
	
	private class CaptureEventListener implements Runnable {
		
		public void run() {
			
			MsgHeader msgHdr = new MsgHeader();
			byte[] hdrBuf = new byte[MsgHeader.HDRBUFF_LEN];
			
			//一直监听抓包进程的消息，直到用户主动退出或发生异常
			while(!mbQuit){
				
				//读取消息首部
				if(hdrBuf.length != mReadpipe.Read(hdrBuf) ||
						!msgHdr.deserializeFrom(hdrBuf)){
					break;
				}
				
				//读取消息内容
				byte[] content = new byte[msgHdr.getLength()];
				if(content.length != mReadpipe.Read(content)){
					break;
				}
				
				try {
					//根据消息类型进行不同处理
					switch (msgHdr.getType()) 
					{
					case MsgType.START_CAP_RES_VALUE:
					{
						StartCapRes res = StartCapRes.parseFrom(content);
						int eventNum = res.getSeq();
						int ack = res.getAck();						
						if(0 == ack){
							mIsCapturing = true;							
						}
						if(null != capInfo){
							capInfo.setStartTime(res.getTime());
						}
						condition.notify(eventNum);
						
						if(0 == ack){
							Message msg = mEventHandler.obtainMessage();
							msg.what = CapEvent.CAP_START_RES;
							msg.obj = res;
							mEventHandler.sendMessage(msg);
						}
					}
					break;
						
					case MsgType.STOP_CAP_RES_VALUE:
					{
						StopCapRes res = StopCapRes.parseFrom(content);
						
						mIsCapturing = false;
						if(null != capInfo){
							capInfo.setEndTime(res.getTime());
						}
						int eventNum = res.getSeq();
						condition.notify(eventNum);
						
						Message msg = mEventHandler.obtainMessage();
						msg.what = CapEvent.CAP_STOP_RES;
						msg.obj = res;
						mEventHandler.sendMessage(msg);
					}
					break;
					
					case MsgType.IP_STAT_NOTIFY_VALUE:
					{
						IpStatNotify notify = IpStatNotify.parseFrom(content);						
						Message msg = mEventHandler.obtainMessage();
						msg.what = CapEvent.IP_STAT_NOTIFY;
						msg.obj = notify;
						mEventHandler.sendMessage(msg);
					}
					break;
					
					case MsgType.TCP_STAT_NOTIFY_VALUE:
					{
						TcpStatNotify notify = TcpStatNotify.parseFrom(content);
						Message msg = mEventHandler.obtainMessage();
						msg.what = CapEvent.TCP_STAT_NOTIFY;
						msg.obj = notify;
						mEventHandler.sendMessage(msg);
					}
					break;
					
					case MsgType.TCP_SES_NOTIFY_VALUE:
					{
						TcpSessionNotify notify = TcpSessionNotify.parseFrom(content);
						int sesId = notify.getSessionId();
						if(null != capInfo && notify.hasDlInfo()){
							CapInfoTCP tcpInfo = capInfo.getCapInfoTCP();
							if(0 == sesId){
								tcpInfo.setDlSumInfo(notify.getDlInfo());				
							}else{							
								CapInfoTCP.TcpSession session = tcpInfo.getTcpSession(sesId);
								if(null != session){
									session.setDlFlowInfo(notify.getDlInfo());
								}
							}
						}
						Message msg = mEventHandler.obtainMessage();
						msg.what = CapEvent.TCP_SES_NOTIFY;
						msg.obj = notify;
						mEventHandler.sendMessage(msg);
					}
					break;
					
					case MsgType.DNS_TRANS_NOTIFY_VALUE:
					{
						DnsTransNotify notify = DnsTransNotify.parseFrom(content);
						Message msg = mEventHandler.obtainMessage();
						msg.what = CapEvent.DNS_TRANS_NOTIFY;
						msg.obj = notify;
						mEventHandler.sendMessage(msg);
					}
					break;
					
					default:
						break;
					}
					
				} catch (InvalidProtocolBufferException e) {
					break;
				}
				
			}
			
			//此处要退出了，如果是异常导致，给界面发个提示消息
			if(!mbQuit){
				mbPrepared = false;
				Message msg = mEventHandler.obtainMessage();
				msg.what = CapEvent.TEXT_MSG_NOTIFY;
				msg.obj = "capturing event listener aborted!";
				mEventHandler.sendMessage(msg);
			}
		}
	}
}
