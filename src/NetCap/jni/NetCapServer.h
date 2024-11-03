/*
 * NetCapServer.h
 *
 *  Created on: 2015-11-17
 *      Author: Administrator
 */

#ifndef NETCAPSERVER_H_
#define NETCAPSERVER_H_
#include "NamedPipe.h"
#include "PacketAnalyzer.h"
#include "PktDataReporter.h"
#include "Log.h"

#include "messageheader.h"
#include "message.pb.h"
using namespace com::zte::netanalysis::capcontrol;

class NetCapServer : public Task
{
public:
	static NetCapServer* getInstance();

	bool init(const char* szPipeDir);
	void unInit();
	void run();

private:
	NetCapServer();
	~NetCapServer();

	bool openSvrPipe(const char* szPipeDir);

	void handlePipeMsg(const MsgHeader& hdr, char buff[]);
	void handleStartCapReq(const MsgHeader& hdr, char buff[]);
	void handleStopCapReq(const MsgHeader& hdr, char buff[]);
	bool sendPipeMsg(int type, const ::google::protobuf::MessageLite& msg);

	NamedPipe m_readpipe;
	NamedPipe m_writepipe;
	PacketAnalyzer m_analyzer;
	PktDataReporter m_reporter;
};



#endif /* NETCAPSERVER_H_ */
