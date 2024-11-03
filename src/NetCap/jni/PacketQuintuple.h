/*
 * PacketQuintuple.h
 *
 *  Created on: 2015-11-24
 *      Author: Administrator
 */

#ifndef PACKETQUINTUPLE_H_
#define PACKETQUINTUPLE_H_
#include "IpAddress.h"

class PacketQuintuple
{
public:
	IpAddress srcIp;
	IpAddress dstIp;
	WORD16 srcPort;
	WORD16 dstPort;
	BYTE proto;

	PacketQuintuple()
	{
		memset(this, 0 , sizeof(PacketQuintuple));
	}

	PacketQuintuple(const T_IPV4Head& ip, const T_TCPHead& tcp)
	{
		memset(this, 0 , sizeof(PacketQuintuple));
		srcIp.setIPv4(ip.dwSrcIp);
		dstIp.setIPv4(ip.dwDstIp);
		srcPort = tcp.wSrcPort;
		dstPort = tcp.wDstPort;
		proto = ip.bPro;
	}

	bool operator<(const PacketQuintuple& other) const
	{
		return memcmp(this, &other, sizeof(PacketQuintuple)) < 0;
	}

	bool operator>(const PacketQuintuple& other) const
	{
		return memcmp(this, &other, sizeof(PacketQuintuple)) > 0;
	}

	bool operator==(const PacketQuintuple& other) const
	{
		return memcmp(this, &other, sizeof(PacketQuintuple)) == 0;
	}

	void getOppsiteQuntp(PacketQuintuple& oppsite) const
	{
		oppsite.srcIp = dstIp;
		oppsite.dstIp = srcIp;
		oppsite.srcPort = dstPort;
		oppsite.dstPort = srcPort;
		oppsite.proto = proto;
	}

	const char* toString()
	{
		static char str[256];
		memset(str, 0, sizeof(str));
		char szSrcIp[64];
		char szDstIp[64];
		srcIp.getString(szSrcIp, sizeof(szSrcIp));
		dstIp.getString(szDstIp, sizeof(szDstIp));
		snprintf(str, sizeof(str),"%s %s:%u->%s:%u",
				protoStr(proto), szSrcIp, srcPort, szDstIp, dstPort);
		return str;
	}

	static const char* protoStr(BYTE pro)
	{
		switch(pro){
		case TCP_PROTOCOL: return "TCP";
		case UDP_PROTOCOL: return "UDP";
		default: return "";
		}
	}

};


#endif /* PACKETQUINTUPLE_H_ */
