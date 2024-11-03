/*
 * TcpProtoHandler.h
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */

#ifndef TCPPROTOHANDLER_H_
#define TCPPROTOHANDLER_H_
#include "commdef.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "PktHdlDecorator.h"
#include "TcpStatData.h"
#include "TcpSessionHandler.h"

class ProtocolStack;

class TcpProtoHandler : public PacketHandler, public DataCollector
{
public:
	TcpProtoHandler(ProtocolStack& protoStack)
		:m_protoStack(protoStack),
		 m_decorator(*this, *this)
	{
	}

	PktHdlDecorator* getDecorator()
	{
		return &m_decorator;
	}

	bool handlePacket(Packet* pPkt);

	void reset()
	{
		m_sumData.clear();
		m_tcpSesHdlMgr.clear();
		DataCollector::setBeginTime(0);
	}

	void reportTotalStat()
	{
		m_tcpSesHdlMgr.reportTotalStat();
		if(NULL != _reporter){
			TcpSessionInfo data;
			m_tcpSesHdlMgr.getDlSummaryInfo(data.dl);
			data.dwSessionId = 0;
			data.hasDlInfo = true;
			data.build();
			_reporter->reportData(data);
		}
	}

	/*implements DataCollector*/
	const Data& collectData(WORD64 startTime, WORD64 endTime);
	void clearData();

private:
	void reportSessionInfo(WORD32 dwSesId, PktDirection dir,
			const PacketQuintuple& quntp);

	ProtocolStack& m_protoStack;
	PktHdlDecorator m_decorator;
	TcpStatData m_sumData;
	TcpSesHdlMgr m_tcpSesHdlMgr;
};




#endif /* TCPPROTOHANDLER_H_ */
