/*
 * TcpProtoHandler.cpp
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */
#include "TcpProtoHandler.h"
#include "ProtocolStack.h"

bool TcpProtoHandler::handlePacket(Packet* pPkt)
{
	PacketHeader* pHdr = pPkt->getBackHeader();
	if(NULL == pHdr || pHdr->getType() != HDR_TYPE_IPV4)
	{
		return false;
	}
	IpHeader* pIpHdr = (IpHeader*)pHdr;
	T_IPV4Head* pIpStruct = pIpHdr->getStruct();

	TcpHeader* pTcpHdr = new TcpHeader;
	if(!pTcpHdr->parseFromPkt(pPkt))
	{
		delete pTcpHdr;
		return false;
	}
	pPkt->addHeader(pTcpHdr);
	pPkt->consumePktData(pTcpHdr->getLength());

	T_TCPHead* pTcpStruct = pTcpHdr->getStruct();
	WORD16 wPayload = pIpStruct->wPktLen - pIpHdr->getLength() - pTcpHdr->getLength();
	pTcpHdr->setPayloadLen(wPayload);

#if 0
	char szFlag[64] = {0};
	if(pTcpStruct->bSyn){
		strcat(szFlag, "SYN ");
	}
	if(pTcpStruct->bFin){
		strcat(szFlag, "FIN ");
	}
	if(pTcpStruct->bRst){
		strcat(szFlag, "RST ");
	}
	LOG("TCP: port %u->%u, %s", pTcpStruct->wSrcPort, pTcpStruct->wDstPort, szFlag);
#endif


	TcpSessionHandler* pSesHandler = NULL;

	PacketQuintuple quntp(*pIpStruct, *pTcpStruct);
	pSesHandler = m_tcpSesHdlMgr.findHandler(quntp);

	if(NULL == pSesHandler)
	{
		if(1 == pTcpStruct->bSyn && 0 == pTcpStruct->bAck)
		{
			pSesHandler = m_tcpSesHdlMgr.createHandler(quntp);
			if(NULL != pSesHandler)
			{
				pSesHandler->setBeginTime(this->getBeginTime());
				pSesHandler->setReporter(this->getReporter());
				pSesHandler->setInterval(this->getInterval());
				pSesHandler->setSumStat(&m_sumData);
				reportSessionInfo(pSesHandler->getSessionId(), pPkt->getDirection(), quntp);
			}
		}
	}

	if(NULL == pSesHandler)
	{
		return false;
	}

	return pSesHandler->getDecorator()->handlePacket(pPkt);
}

const Data& TcpProtoHandler::collectData(WORD64 startTime, WORD64 endTime)
{
	m_sumData.startTime = startTime;
	m_sumData.endTime = endTime;
	m_sumData.build();
	return m_sumData;
}

void TcpProtoHandler::clearData()
{
	m_sumData.clear();
}

void TcpProtoHandler::reportSessionInfo(WORD32 dwSesId, PktDirection dir,
		const PacketQuintuple& quntp)
{
	if(NULL == _reporter)
	{
		return;
	}
	TcpSessionInfo data;
	data.dwSessionId = dwSesId;
	data.hasIpPort = true;

	if(PKT_DIR_UPLINK == dir)
	{
		data.localIp = quntp.srcIp;
		data.remoteIp = quntp.dstIp;
		data.wLocalPort = quntp.srcPort;
		data.wRemotePort = quntp.dstPort;
	}
	else
	{
		data.localIp = quntp.dstIp;
		data.remoteIp = quntp.srcIp;
		data.wLocalPort = quntp.dstPort;
		data.wRemotePort = quntp.srcPort;
	}
	data.build();
	_reporter->reportData(data);
}




