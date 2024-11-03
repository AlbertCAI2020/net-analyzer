/*
 * DnsProtoHandler.cpp
 *
 *  Created on: 2016-2-2
 *      Author: Administrator
 */
#include "DnsProtoHandler.h"
#include "ProtocolStack.h"
#include "Log.h"

bool DnsProtoHandler::handlePacket(Packet* pPkt)
{
	DnsPktInfo dnsPktInfo;
	if(!dnsPktInfo.parseFromPkt(pPkt))
	{
		LOG("dns pkt parse failed");
		return false;
	}

	if(dnsPktInfo.isQuery())
	{
		handleDNSQueryPkt(pPkt, dnsPktInfo);
	}
	else if(dnsPktInfo.isAnswer())
	{
		handleDNSAnswerPkt(pPkt, dnsPktInfo);
	}

	delete pPkt;
	return true;
}

bool DnsProtoHandler::handleDNSQueryPkt(Packet* pPkt,
		const DnsPktInfo& dnsPktInfo)
{
	PacketHeader* hdr = pPkt->getFrontHeader();
	IpHeader* iphdr =NULL;
	if(NULL != hdr && HDR_TYPE_IPV4 == hdr->getType())
	{
		iphdr = dynamic_cast<IpHeader*>(hdr);
	}

	WORD16 transId = dnsPktInfo.getStruct()->wId;
	DnsTransInfo* info = findTransInfo(transId);
	if(NULL != info)
	{
		if(0 != strcmp(info->szHostName, dnsPktInfo.getHostName()))
		{
			LOG("DNS query id conflict, id:%u "
				"old query name: %s new query name:%s", transId,
				info->szHostName, dnsPktInfo.getHostName());
			eraseTransInfo(info);
			delete info;
			return false;
		}

		info->retryCount++;
	}
	else
	{
		info = new DnsTransInfo;
		if(NULL != info)
		{
			if(NULL != iphdr)
			{
				info->dnsServerIp.setIPv4(iphdr->getStruct()->dwDstIp);
			}
			info->transId = transId;
			info->queryTime = pPkt->getTime().milliSecond();
			strcpy(info->szHostName, dnsPktInfo.getHostName());
			addTransInfo(info);
		}
		else
		{
			LOG("Allocate DnsTransInfo failed!");
			return false;
		}
	}

	return true;
}

bool DnsProtoHandler::handleDNSAnswerPkt(Packet* pPkt,
		const DnsPktInfo& dnsPktInfo)
{
	WORD16 transId = dnsPktInfo.getStruct()->wId;
	DnsTransInfo* info = findTransInfo(transId);
	if(NULL == info)
	{
		LOG("DNS answer arrived with no query, answer id:%u, name:%s",
				transId, dnsPktInfo.getHostName());
		return false;
	}

	if(0 != strcmp(info->szHostName, dnsPktInfo.getHostName()))
	{
		LOG("DNS answer host name not matching with the same query id: %u\n"
			"query name: %s\n answer name:%s", transId,
			info->szHostName, dnsPktInfo.getHostName());
		eraseTransInfo(info);
		delete info;
		return false;
	}

	info->answerTime = pPkt->getTime().milliSecond();
	int num = dnsPktInfo.getAddrCount();
	for(int i = 0; i < num; i++)
	{
		info->ipAddrs[i] = dnsPktInfo.getIpAddr(i);
	}
	info->addrNum = num;

	DataReporter* reporter = DataCollectCenter::getInstance()->getReporter();
	if(NULL != reporter)
	{
		info->build();
		reporter->reportData(*info);
	}
	eraseTransInfo(info);
	delete info;

	return true;
}

DnsTransInfo* DnsProtoHandler::findTransInfo(WORD16 transId)
{
	std::map<WORD16, DnsTransInfo*>::iterator it;
	it = m_transIdInfoMap.find(transId);
	if(it != m_transIdInfoMap.end())
	{
		return it->second;
	}
	return NULL;
}

void DnsProtoHandler::eraseTransInfo(DnsTransInfo* info)
{
	m_transIdInfoMap.erase(info->transId);
}

void DnsProtoHandler::addTransInfo(DnsTransInfo* info)
{
	m_transIdInfoMap.insert(std::make_pair(info->transId, info));
}

void DnsProtoHandler::clearTransInfo()
{
	std::map<WORD16, DnsTransInfo*>::iterator it;
	for(it = m_transIdInfoMap.begin(); it != m_transIdInfoMap.end(); it++)
	{
		delete it->second;
	}
	m_transIdInfoMap.clear();
}


