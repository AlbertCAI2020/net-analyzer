package com.zte.netanalysis.captask;

public interface InternetActor {
	
	int getActionType();
	
	boolean start(CapTask task);
	
	void terminate();
}
