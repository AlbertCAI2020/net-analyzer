/*
 * IpProtoHandler.cpp
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */
#include "IpProtoHandler.h"
#include "ProtocolStack.h"
#include "IpAddress.h"

bool IpProtoHandler::handlePacket(Packet* pPkt)
{
	IpHeader* pHeader = new IpHeader;
	if(!pHeader->parseFromPkt(pPkt))
	{
		delete pHeader;
		return false;
	}
	pPkt->consumePktData(pHeader->getLength());
	pPkt->addHeader(pHeader);

	T_IPV4Head* pHead = pHeader->getStruct();

	if(pHead->dwSrcIp == m_dwIpAddr)
	{
		pPkt->setDirection(PKT_DIR_UPLINK);
		m_periodData.ulStats.addPktNum(1);
		m_periodData.ulStats.addPktBytes(pHead->wPktLen);
	}
	else if(pHead->dwDstIp == m_dwIpAddr)
	{
		pPkt->setDirection(PKT_DIR_DOWNLINK);
		m_periodData.dlStats.addPktNum(1);
		m_periodData.dlStats.addPktBytes(pHead->wPktLen);
	}

	if((pHead->wOffset& 0x1fff) != 0) //ip非首个分片报文，暂时不处理
	{
		return false;
	}

#if 0
	char szIp1[16];
	char szIp2[16];
	IpAddress src(pHead->dwSrcIp);
	IpAddress dst(pHead->dwDstIp);
	src.getString(szIp1, 16);
	dst.getString(szIp2, 16);
	LOG("IP: %s->%s, proto=%u, length=%d", szIp1, szIp2, pHead->bPro, pHead->wPktLen);
#endif

	PacketHandler* pNextHandler = NULL;
	if(TCP_PROTOCOL == pHead->bPro)
	{
		pNextHandler = m_protoStack.getProtoHandler(HDR_TYPE_TCP);
	}
	else if(UDP_PROTOCOL == pHead->bPro)
	{
		pNextHandler = m_protoStack.getProtoHandler(HDR_TYPE_UDP);
	}

	if(NULL == pNextHandler || !pNextHandler->handlePacket(pPkt))
	{
		delete pPkt;
	}
	return true;
}

const Data& IpProtoHandler::collectData(WORD64 startTime, WORD64 endTime)
{
	m_periodData.startTime = startTime;
	m_periodData.endTime = endTime;
	m_periodData.build();
	return m_periodData;
}

void IpProtoHandler::clearData()
{
	m_periodData.clear();
}



