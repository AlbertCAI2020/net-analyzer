/*
 * DnsPktInfo.h
 *
 *  Created on: 2015-12-8
 *      Author: Administrator
 */

#ifndef DNSPKTINFO_H_
#define DNSPKTINFO_H_
#include "PacketHeader.h"
#include "IpAddress.h"

#define MAX_HOST_NAME 256
#define MAX_ADDRESS_NUM 30

class DnsPktInfo : public PacketHeader,
	public HeaderStruct<T_DNSHead>
{
public:
	DnsPktInfo()
		:PacketHeader(HDR_TYPE_DNS)
	{
		m_isQuery = false;
		m_isAnswer = false;
		m_addrNum = 0;
		memset(m_szHostName, 0, sizeof(m_szHostName));
	}

	bool parseFromPkt(Packet* pPkt);

	bool isQuery() const;

	bool isAnswer() const;

	const char* getHostName() const;

	int getAddrCount() const;

	const IpAddress& getIpAddr(int index) const;

private:
	int dnsDecodeHead(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen);
	int dnsDecodeName(char* buffer, WORD16 bufflen, const BYTE* name,
			const BYTE* pkt, WORD16 pktlen);
	int dnsDecodeQuery(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen);
	int dnsDecodeAnswer(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen);

	bool m_isQuery;
	bool m_isAnswer;
	short m_addrNum;
	char m_szHostName[MAX_HOST_NAME];
	IpAddress m_Addresses[MAX_ADDRESS_NUM];
};

#endif /* DNSPARSER_H_ */
