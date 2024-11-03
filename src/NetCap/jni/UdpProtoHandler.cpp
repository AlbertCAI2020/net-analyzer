/*
 * UdpProtoHandler.cpp
 *
 *  Created on: 2016-2-2
 *      Author: Administrator
 */
#include "UdpProtoHandler.h"
#include "ProtocolStack.h"

bool UdpProtoHandler::handlePacket(Packet* pPkt)
{
	UdpHeader* pHdr = new UdpHeader;
	if(!pHdr->parseFromPkt(pPkt))
	{
		delete pHdr;
		return false;
	}
	pPkt->addHeader(pHdr);
	pPkt->consumePktData(pHdr->getLength());

	T_UDPHead* pStruct = pHdr->getStruct();
	PacketHandler* pNextHandler = NULL;

	if(DNS_PORT == pStruct->wSrcPort ||
		DNS_PORT == pStruct->wDstPort)
	{
		pNextHandler = m_protoStack.getProtoHandler(HDR_TYPE_DNS);
	}

	if(NULL == pNextHandler || !pNextHandler->handlePacket(pPkt))
	{
		delete pPkt;
	}
	return true;
}
