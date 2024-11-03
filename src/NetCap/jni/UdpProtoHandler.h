/*
 * UdpProtoHandler.h
 *
 *  Created on: 2016-2-1
 *      Author: Administrator
 */

#ifndef UDPPROTOHANDLER_H_
#define UDPPROTOHANDLER_H_
#include "commdef.h"
#include "Packet.h"
#include "PacketHandler.h"

class ProtocolStack;

class UdpProtoHandler : public PacketHandler
{
public:
	UdpProtoHandler(ProtocolStack& protoStack)
		:m_protoStack(protoStack)
	{
	}

	bool handlePacket(Packet* pPkt);

private:
	ProtocolStack& m_protoStack;
};



#endif /* UDPPROTOHANDLER_H_ */
