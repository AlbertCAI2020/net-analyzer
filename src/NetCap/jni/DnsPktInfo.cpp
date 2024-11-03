/*
 * DnsPktInfo.cpp
 *
 *  Created on: 2015-12-8
 *      Author: Administrator
 */

#include "Packet.h"
#include "DnsPktInfo.h"

bool DnsPktInfo::parseFromPkt(Packet* pPkt)
{
	PacketBuffer* pBuffer = pPkt->getBuffer(0);
	if(NULL == pBuffer)
	{
		return false;
	}
	BYTE* pBuff = pBuffer->getBuffer();
	WORD16 wBuffLen = pBuffer->getLength();
	T_DNSHead* hdr = this->getStruct();
	int decoded = dnsDecodeHead(hdr, pBuff, wBuffLen);
	if(decoded != DNS_HEAD_LEN)
	{
		return false;
	}
	if(hdr->bTC)//truncated
	{
		return false;
	}
	if((hdr->bQR==0) && (dnsDecodeQuery(hdr, pBuff, wBuffLen) > 0))
	{
		m_isQuery = true;
	}
	else if((hdr->bQR==1) && (dnsDecodeAnswer(hdr, pBuff, wBuffLen) > 0))
	{
		m_isAnswer = true;
	}
	else
	{
		return false;
	}
	return true;
}

int DnsPktInfo::dnsDecodeHead(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen)
{
	if(pktlen < sizeof(T_DNSHead))
	{
		return -1;
	}
	*hdr = *(T_DNSHead*)pkt;
	hdr->wId = ntohs(hdr->wId);
	hdr->wQdCount = ntohs(hdr->wQdCount);
	hdr->wAnCount = ntohs(hdr->wAnCount);
	hdr->wNsCount = ntohs(hdr->wNsCount);
	hdr->wArCount = ntohs(hdr->wArCount);
	return sizeof(T_DNSHead);
}

int DnsPktInfo::dnsDecodeName(char* buffer, WORD16 bufflen, const BYTE* name,
		const BYTE* pkt, WORD16 pktlen)
{
	if(NULL == name || NULL == pkt || 0 == pktlen ||
		name < pkt + DNS_HEAD_LEN ||
		name >= pkt + pktlen)
	{
		return -1;
	}

	int decode_bytes = 0;
	bool still_counting = true;
	int count = 0;
	int loop = 0;

	while(true)
	{
		loop++;
		if(loop > 100)
		{
			return -1;
		}

		count = *name;
		if(0 == count)
		{
			if(still_counting)
			{
				decode_bytes++;
			}
			break;
		}

		switch(count & DNS_COMPRESSION_MASK)
		{
		case DNS_COMPRESSION_POINTER:
			{
				if(still_counting)
				{
					decode_bytes += 2;
					still_counting = false;
				}
				name =  pkt + ((name[0] & ~DNS_COMPRESSION_MASK) << 8) + name[1];
				if((name < pkt + DNS_HEAD_LEN) ||
					(name >= pkt + pktlen))
				{
					return -1;
				}
			}
			break;
		case 0:
			{
				if(name + count + 1 >= pkt + pktlen)
				{
					return -1;
				}

				if(still_counting)
				{
					decode_bytes += count+1;
				}

				if(NULL != buffer && bufflen > count)
				{
					name++;
					memcpy(buffer, name, count);
					buffer += count;
					bufflen -= count;
					name += count;
					*buffer = (*name == 0)? '\0' : '.';
					buffer++;
					bufflen--;
				}
				else
				{
					name += count+1;
				}
			}
			break;
		default:
			return -1;
		}

	}

	return decode_bytes;
}

int DnsPktInfo::dnsDecodeQuery(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen)
{
	if(hdr->wQdCount != 1)
	{
		return -1;
	}
	int decode_bytes = 0;
	char szName[MAX_HOST_NAME] = {0};
	const BYTE* buff = pkt + DNS_HEAD_LEN;
	WORD16 bufflen = pktlen - DNS_HEAD_LEN;
	int decoded = dnsDecodeName(szName, MAX_HOST_NAME, buff, pkt, pktlen);
	if(decoded < 0)
	{
		return -1;
	}
	buff += decoded;
	bufflen -= decoded;
	decode_bytes += decoded;
	if(bufflen < 4)
	{
		return -1;
	}
	WORD16 qtype, qclass;
	qtype = ntohs(*(WORD16*)buff);
	buff += 2;
	bufflen -= 2;
	qclass = ntohs(*(WORD16*)buff);
	buff += 2;
	bufflen -= 2;
	decode_bytes += 4;

	if((qtype != DNS_T_A && qtype != DNS_T_AAAA) ||
			qclass != DNS_C_INTERNET)
	{
		return -1;
	}
	strcpy(m_szHostName, szName);

	return decode_bytes;
}

int DnsPktInfo::dnsDecodeAnswer(T_DNSHead* hdr, const BYTE* pkt, WORD16 pktlen)
{
	int decode_bytes = 0;
	const BYTE* buff = pkt + DNS_HEAD_LEN;
	WORD16 bufflen = pktlen - DNS_HEAD_LEN;
	int decoded = dnsDecodeQuery(hdr, pkt, pktlen);
	if(decoded < 0)
	{
		return -1;
	}
	buff += decoded;
	bufflen -= decoded;
	decode_bytes += decoded;

	WORD32 n = hdr->wAnCount + hdr->wArCount + hdr->wNsCount;
	while(n > 0)
	{
		decoded = dnsDecodeName(NULL, 0, buff, pkt, pktlen);
		if(decoded < 0)
		{
			return -1;
		}
		buff += decoded;
		bufflen -= decoded;
		decode_bytes += decoded;
		if(bufflen < 10)
		{
			return -1;
		}
		WORD16 qtype, qclass, rdcount;
		WORD32 ttl;
		qtype = ntohs(*(WORD16*)buff);
		buff += 2;
		qclass = ntohs(*(WORD16*)buff);
		buff += 2;
		ttl = ntohl(*(WORD32*)buff);
		buff += 4;
		rdcount = ntohs(*(WORD16*)buff);
		buff += 2;

		bufflen -= 10;
		decode_bytes += 10;

		if(bufflen < rdcount)
		{
			return -1;
		}

		if(DNS_T_A == qtype)
		{
			if(rdcount != 4)
			{
				return -1;
			}
			if(m_addrNum < MAX_ADDRESS_NUM)
			{
				WORD32 ipv4 = ntohl(*(WORD32*)buff);
				m_Addresses[m_addrNum].setIPv4(ipv4);
				m_addrNum++;
			}
		}else if(DNS_T_AAAA == qtype)
		{
			if(rdcount != 16)
			{
				return -1;
			}
			if(m_addrNum < MAX_ADDRESS_NUM)
			{
				BYTE ipv6[16];
				memcpy(ipv6, buff, 16);
				m_Addresses[m_addrNum].setIPv6(ipv6);
				m_addrNum++;
			}
		}
		buff += rdcount;
		bufflen -= rdcount;
		decode_bytes += rdcount;
		n--;
	}

	return decode_bytes;
}


bool DnsPktInfo::isQuery() const
{
	return m_isQuery;
}

bool DnsPktInfo::isAnswer() const
{
	return m_isAnswer;
}

const char* DnsPktInfo::getHostName() const
{
	return m_szHostName;
}

int DnsPktInfo::getAddrCount() const
{
	return m_addrNum;
}

const IpAddress& DnsPktInfo::getIpAddr(int index) const
{
	if(index < 0 || index >= m_addrNum)
	{
		index = m_addrNum - 1;
	}
	return m_Addresses[index];
}




