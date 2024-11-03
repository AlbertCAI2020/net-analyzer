/*
 * IpProtoHandler.h
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */

#ifndef IPPROTOHANDLER_H_
#define IPPROTOHANDLER_H_
#include "commdef.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "PktHdlDecorator.h"
#include "IpStatData.h"

class ProtocolStack;

class IpProtoHandler : public PacketHandler, public DataCollector
{
public:
	IpProtoHandler(ProtocolStack& protoStack)
		:m_dwIpAddr(0),
		 m_protoStack(protoStack),
		 m_decorator(*this, *this)
	{
	}
	PktHdlDecorator* getDecorator()
	{
		return &m_decorator;
	}

	void setIpAddr(WORD32 ip)
	{
		m_dwIpAddr = ip;
	}

	bool handlePacket(Packet* pPkt);

	void reset()
	{
		m_periodData.clear();
		DataCollector::setBeginTime(0);
	}

	/*implements DataCollector*/
	const Data& collectData(WORD64 startTime, WORD64 endTime);
	void clearData();

private:
	WORD32 m_dwIpAddr;
	ProtocolStack& m_protoStack;
	PktHdlDecorator m_decorator;

	IpStatData m_periodData;
};



#endif /* IPPROTOHANDLER_H_ */
