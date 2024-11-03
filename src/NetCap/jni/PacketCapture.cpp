/*
 * PacketCapture.cpp
 *
 *  Created on: 2015-11-20
 *      Author: Administrator
 */

#include "PacketCapture.h"
#include <netinet/if_ether.h>

bool PacketCapture::init()
{
	if(NULL != m_pcap){
		return false;
	}

	char errbuf[PCAP_ERRBUF_SIZE] = {0};
	char *device = pcap_lookupdev(errbuf);
	if(NULL==device)
	{
		LOG("pcap_lookupdev error:%s", errbuf);
		return false;
	}

	LOG("device:%s", device);
	if(!ifcfg(device, errbuf))
	{
		LOG("ifcfg error:%s", errbuf);
		return false;
	}

	pcap_t* pcap = pcap_open_live(device, 65535, 0, 0, errbuf);
	if(NULL== pcap)
	{
		LOG("pcap_open_live error:%s", errbuf);
		return false;
	}
	m_datalink = pcap_datalink(pcap);
	LOG("data-link:%d, %s, %s",m_datalink,
			pcap_datalink_val_to_name(m_datalink),
			pcap_datalink_val_to_description(m_datalink));

	m_pcap = pcap;

	if(!m_thread.start())
	{
		pcap_close(m_pcap);
		m_pcap = NULL;
		return false;
	}
	return true;
}

void PacketCapture::unInit()
{
	m_task.quit();
	if(NULL != m_pcap){
		pcap_close(m_pcap);
		m_pcap = NULL;
	}
	//m_thread.join();
}

bool PacketCapture::hasIfcfgIpv4()
{
	return m_hasIfcfgIpv4;
}

WORD32 PacketCapture::getIpv4Addr()
{
	return m_ifaddrIpv4;
}

WORD32 PacketCapture::getIpv4Mask()
{
	return m_ifmaskIpv4;
}

void PacketCapture::OnPacketCaptured(const struct pcap_pkthdr* pkthdr, const u_char*data)
{
	WORD32 dwLen = 0;
	const BYTE* pData = NULL;
	if(m_datalink == DLT_LINUX_SLL)
	{
		sll_header* sll = (sll_header*)data;
		if(ETHERTYPE_IP != ntohs(sll->sll_protocol))
		{
			return;
		}
		dwLen = pkthdr->caplen - sizeof(sll_header);
		pData = data + sizeof(sll_header);
	}
	else if(m_datalink == DLT_EN10MB)
	{
		ethhdr* eth = (ethhdr*)data;
		WORD16 type = ntohs(eth->h_proto);
		if(ETHERTYPE_IP != type)
		{
			return;
		}
		dwLen = pkthdr->caplen - sizeof(ethhdr);
		pData = data + sizeof(ethhdr);
	}

	if(NULL == pData)
	{
		return;
	}

	PacketTime time;
	time.setTime(pkthdr->ts.tv_sec, pkthdr->ts.tv_usec);

	Packet* pPkt = new Packet;
	pPkt->setTime(time);

	WORD32 dwPushed = 0;
	while(dwPushed < dwLen)
	{
		PacketBuffer* pBuffer = new PacketBuffer;
		dwPushed += pBuffer->pushData(pData + dwPushed, dwLen - dwPushed);
		pPkt->addBuffer(pBuffer);
	}

	if(NULL == m_pHandler || !m_pHandler->handlePacket(pPkt))
	{
		delete pPkt;
	}
}

bool PacketCapture::ifcfg(const char* device, char* errbuf)
{
	in_addr ip ,mask;
	struct sockaddr_in* sin4;
	struct ifreq ifr;
	char szIp[128];
	char szMask[128];

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		snprintf(errbuf, PCAP_ERRBUF_SIZE,
					"socket: %s",
					 pcap_strerror(errno));
		return false;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_addr.sa_family = AF_INET;
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFADDR, (char *)&ifr) < 0)
	{
		if (errno == EADDRNOTAVAIL) {
			snprintf(errbuf, PCAP_ERRBUF_SIZE,
				"%s: no IPv4 address assigned", device);
		} else {
			snprintf(errbuf, PCAP_ERRBUF_SIZE,
				"SIOCGIFADDR: %s: %s",
				device, pcap_strerror(errno));
		}
		close(fd);
		return false;
	}
	sin4 = (struct sockaddr_in *)&ifr.ifr_addr;
	ip = sin4->sin_addr;
	strcpy(szIp, inet_ntoa(ip));

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_addr.sa_family = AF_INET;
	strlcpy(ifr.ifr_name, device, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFNETMASK, (char *)&ifr) < 0) {
		snprintf(errbuf, PCAP_ERRBUF_SIZE,
			"SIOCGIFNETMASK: %s: %s", device, pcap_strerror(errno));
		close(fd);
		return false;
	}
	sin4 = (struct sockaddr_in *)&ifr.ifr_addr;
	mask = sin4->sin_addr;
	strcpy(szMask, inet_ntoa(mask));
	close(fd);

	m_hasIfcfgIpv4 = true;
	m_ifaddrIpv4 = ntohl(ip.s_addr);
	m_ifmaskIpv4 = ntohl(mask.s_addr);

	LOG("ip:0x%x(%s), mask:0x%x(%s)", m_ifaddrIpv4, szIp, m_ifmaskIpv4, szMask);
	return true;
}

