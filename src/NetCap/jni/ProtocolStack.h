/*
 * ProtocolHandlers.h
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */

#ifndef PROTOCOLHANDLERS_H_
#define PROTOCOLHANDLERS_H_
#include "DataCollectCenter.h"
#include "IpProtoHandler.h"
#include "TcpProtoHandler.h"
#include "UdpProtoHandler.h"
#include "DnsProtoHandler.h"

class ProtocolStack
{
public:
	ProtocolStack()
		:m_ipHandler(*this),
		 m_tcpHandler(*this),
		 m_udpHandler(*this),
		 m_dnsHandler(*this)
	{
		DataCollectCenter* pInst = DataCollectCenter::getInstance();
		pInst->registerCollector(IP_STAT_DATA, &m_ipHandler);
		pInst->registerCollector(TCP_STAT_DATA, &m_tcpHandler);
	}

	~ProtocolStack()
	{
		DataCollectCenter* pInst = DataCollectCenter::getInstance();
		pInst->unregisterCollector(IP_STAT_DATA);
		pInst->unregisterCollector(TCP_STAT_DATA);
	}

	PacketHandler* getProtoHandler(HdrType protoType)
	{
		PacketHandler* pHandler = NULL;
		switch(protoType)
		{
		case HDR_TYPE_IPV4:
			pHandler = m_ipHandler.getDecorator();
			break;
		case HDR_TYPE_IPV6:
			break;
		case HDR_TYPE_TCP:
			pHandler = m_tcpHandler.getDecorator();
			break;
		case HDR_TYPE_UDP:
			pHandler = &m_udpHandler;
			break;
		case HDR_TYPE_DNS:
			pHandler = &m_dnsHandler;
			break;
		default:
			break;
		}
		return pHandler;
	}

	void setIpv4Addr(WORD32 ip)
	{
		m_ipHandler.setIpAddr(ip);
	}

	void reset()
	{
		m_ipHandler.reset();
		m_tcpHandler.reset();
		m_dnsHandler.reset();
	}

	void reportTotalStat()
	{
		m_tcpHandler.reportTotalStat();
	}

private:
	IpProtoHandler m_ipHandler;
	TcpProtoHandler m_tcpHandler;
	UdpProtoHandler m_udpHandler;
	DnsProtoHandler m_dnsHandler;
};



#endif /* PROTOCOLHANDLERS_H_ */
