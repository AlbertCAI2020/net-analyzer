package com.zte.netanalysis.capcontrol;

import com.zte.netanalysis.capcontrol.Message.StartCapRes;
import com.zte.netanalysis.capcontrol.Message.StopCapRes;


public interface CapEventObserver {

	public void onTxtMsgNotify(String msg);
	public void onCaptureStarted(StartCapRes msg);
	public void onCaptureStopped(StopCapRes msg);
}
