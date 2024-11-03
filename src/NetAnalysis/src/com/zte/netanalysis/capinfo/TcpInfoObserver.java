package com.zte.netanalysis.capinfo;


public interface TcpInfoObserver {

	public void onTcpSesInfoUpdate(int sesId, CapInfoTCP info);
	public void onTcpStatInfoUpdate(int sesId, CapInfoTCP info);
}
