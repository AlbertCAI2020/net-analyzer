package com.zte.netanalysis.capcontrol;

import java.util.ArrayList;

import com.zte.netanalysis.capcontrol.Message.DnsTransNotify;
import com.zte.netanalysis.capcontrol.Message.IpStatNotify;
import com.zte.netanalysis.capcontrol.Message.StartCapRes;
import com.zte.netanalysis.capcontrol.Message.StopCapRes;
import com.zte.netanalysis.capcontrol.Message.TcpSessionNotify;
import com.zte.netanalysis.capcontrol.Message.TcpStatNotify;
import com.zte.netanalysis.capinfo.CapInfo;
import com.zte.netanalysis.capinfo.CapInfoDNS;
import com.zte.netanalysis.capinfo.CapInfoIP;
import com.zte.netanalysis.capinfo.CapInfoTCP;
import com.zte.netanalysis.capinfo.CapInfoTCP.TcpSession;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

public class CapEventHandler extends Handler {
	
	public CapEventHandler() {
		super(Looper.getMainLooper());
	}
	
	private CapInfo capInfo;	
	
	public CapInfo getCapInfo() {
		return capInfo;
	}

	public void setCapInfo(CapInfo capInfo) {
		this.capInfo = capInfo;
	}

	private ArrayList<CapEventObserver> observers = new ArrayList<CapEventObserver>();	

	public void registerObserver(CapEventObserver ob){
		if(null == ob || observers.contains(ob)){
			return;
		}
		observers.add(ob);
	}
	
	public void unregisterObserver(CapEventObserver ob){
		if(null == ob || !observers.contains(ob)){
			return;
		}
		observers.remove(ob);
	}
	
	private void notifyTextMsg(String msg){
		for(CapEventObserver ob : observers){
			ob.onTxtMsgNotify(msg);
		}
	}
	private void notifyCaptureStarted(StartCapRes msg){		
		for(CapEventObserver ob : observers){
			ob.onCaptureStarted(msg);
		}
	}
	private void notifyCaptureStoped(StopCapRes msg) {		
		for(CapEventObserver ob : observers){
			ob.onCaptureStopped(msg);
		}
	}
	
	 public void handleMessage(Message msg){
		 
		switch (msg.what) {
		
		case CapEvent.TEXT_MSG_NOTIFY:
			notifyTextMsg((String)msg.obj);
			break;
		case CapEvent.CAP_START_RES:{
			StartCapRes res = (StartCapRes)msg.obj;			
			notifyCaptureStarted(res);
		}
			break;
		case CapEvent.CAP_STOP_RES:{
			StopCapRes res = (StopCapRes)msg.obj;
			notifyCaptureStoped(res);
		}
			break;
		case CapEvent.IP_STAT_NOTIFY:{
			IpStatNotify notify = (IpStatNotify)msg.obj;
			
			if(null == capInfo){
				break;
			}
			CapInfoIP ipinfo = capInfo.getCapInfoIP();
			CapInfoIP.InterStat stat = new CapInfoIP.InterStat();
			stat.beginTime = notify.getStartTime();
			stat.endTime = notify.getEndTime();
			stat.bytes = notify.getBytes();
			stat.pktNum = notify.getPktNum();
			ipinfo.addDlIntervalStat(stat);
			
			ipinfo.notifyIpStatInfoChanged();

		}
			break;
		case CapEvent.TCP_STAT_NOTIFY:{
			TcpStatNotify notify = (TcpStatNotify)msg.obj;
			
			int sesId = notify.getSessionId();
			if(null == capInfo){
				break;
			}
			CapInfoTCP tcpinfo = capInfo.getCapInfoTCP();
			CapInfoTCP.InterStat stat = new CapInfoTCP.InterStat();
		
			stat.beginTime = notify.getStartTime();
			stat.endTime = notify.getEndTime();
			
			stat.pktNum = notify.getDlStat().getPktNum();
			stat.bytes = notify.getDlStat().getBytes();
			stat.retransNum = notify.getDlStat().getRetransNum();
			stat.retransBytes = notify.getDlStat().getRetransBytes();
			stat.retransAckedNum = notify.getDlStat().getRetransAckedNum();
			stat.retransAckedBytes = notify.getDlStat().getRetransAckedBytes();
			
			tcpinfo.addDlIntervalStat(sesId, stat);
			tcpinfo.notifyTcpStatInfoChanged(sesId);
		}
			break;
		case CapEvent.TCP_SES_NOTIFY:{
			if(null == capInfo){
				break;
			}
			CapInfoTCP tcpinfo = capInfo.getCapInfoTCP();
			
			TcpSessionNotify notify = (TcpSessionNotify)msg.obj;
			int sesId = notify.getSessionId();
			
			if(0 != sesId){
				TcpSession session = tcpinfo.getTcpSession(sesId);			
				if(null == session && notify.hasIpPort()){
					int id = notify.getSessionId();
					String localIp = notify.getIpPort().getLocalIP();
					String remoteIp = notify.getIpPort().getRemoteIP();
					int localPort = notify.getIpPort().getLocalPort();
					int remotePort = notify.getIpPort().getRemotePort();
					session = tcpinfo.addTcpSession(id, localIp, remoteIp, localPort, remotePort);
				}
				if(null != session && notify.hasRtt()){
					session.rtt = notify.getRtt();
				}
			}
			
			tcpinfo.notifyTcpSesInfoChanged(sesId);
			
		}
			break;
		case CapEvent.DNS_TRANS_NOTIFY:{
			if(null == capInfo){
				break;
			}
			CapInfoDNS dnsinfo = capInfo.getCapInfoDNS();
			DnsTransNotify notify = (DnsTransNotify)msg.obj;
			
			CapInfoDNS.DnsTransInfo info = new CapInfoDNS.DnsTransInfo();
			info.retryCount = notify.getRetryCount();
			info.queryTime = notify.getQueryTime();
			info.answerTime = notify.getAnswerTime();
			info.dnsServerIp = notify.getDnsServerIp();
			info.hostName = notify.getHostName();
			info.hostIpAddr = notify.getIpAddr();
			
			dnsinfo.addDnsTransInfo(info);
			dnsinfo.notifyDnsTransInfoChanged();
			
		}
			break;
		default:
			break;
		}
		 
	 }
}