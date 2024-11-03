/*
 * DnsProtoHandler.h
 *
 *  Created on: 2016-2-1
 *      Author: Administrator
 */

#ifndef DNSPROTOHANDLER_H_
#define DNSPROTOHANDLER_H_
#include "commdef.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "DnsPktInfo.h"
#include "DnsTransInfo.h"
#include <map>

class ProtocolStack;

class DnsProtoHandler : public PacketHandler
{
public:
	DnsProtoHandler(ProtocolStack& protoStack)
		:m_protoStack(protoStack)
	{
	}

	bool handlePacket(Packet* pPkt);

	void reset(){
		clearTransInfo();
	}

private:
	ProtocolStack& m_protoStack;

	bool handleDNSQueryPkt(Packet* pPkt, const DnsPktInfo& dnsPktInfo);
	bool handleDNSAnswerPkt(Packet* pPkt, const DnsPktInfo& dnsPktInfo);

	DnsTransInfo* findTransInfo(WORD16 transId);
	void eraseTransInfo(DnsTransInfo* info);
	void addTransInfo(DnsTransInfo* info);
	void clearTransInfo();

	std::map<WORD16, DnsTransInfo*> m_transIdInfoMap;
};



#endif /* DNSPROTOHANDLER_H_ */
