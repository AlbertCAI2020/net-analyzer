/*
 * DnsTransInfo.h
 *
 *  Created on: 2016-2-14
 *      Author: Administrator
 */

#ifndef DNSTRANSINFO_H_
#define DNSTRANSINFO_H_
#include "DnsPktInfo.h"
#include "Data.h"
#include "messageheader.h"
#include "message.pb.h"
using namespace com::zte::netanalysis::capcontrol;
#include "Log.h"

class DnsTransInfo : public Data,
	public PooledObject<DnsTransInfo, 100, 5>
{
public:
	WORD16 transId;
	BYTE retryCount;
	BYTE addrNum;
	WORD64 queryTime;
	WORD64 answerTime;
	IpAddress dnsServerIp;
	char szHostName[MAX_HOST_NAME];
	IpAddress ipAddrs[MAX_ADDRESS_NUM];

	DnsTransInfo()
	{
		transId = 0;
		retryCount = 0;
		addrNum = 0;
		queryTime = 0;
		answerTime = 0;
		memset(szHostName, 0, MAX_HOST_NAME);
	}

	bool serializeToBuffer(BYTE* buff, size_t len) const
	{
		if(len < serializedBytes()){
			return false;
		}
		MsgHeader* hdr = (MsgHeader*)buff;
		hdr->length = msg.ByteSize();
		hdr->type = DNS_TRANS_NOTIFY;
		if(!msg.SerializeToArray(buff + HDRBUFF_LEN,
				len - HDRBUFF_LEN)){
			return false;
		}
		return true;
	}

	size_t serializedBytes() const
	{
		return HDRBUFF_LEN + msg.ByteSize();
	}

	void build()
	{
		msg.set_retrycount(retryCount);
		msg.set_querytime(queryTime);
		msg.set_answertime(answerTime);

		char szIp[64] = {0};
		dnsServerIp.getString(szIp, sizeof(szIp));
		msg.set_dnsserverip(szIp);
		msg.set_hostname(szHostName);

		ipAddrs[0].getString(szIp, sizeof(szIp));
		msg.set_ipaddr(szIp);
	}

	const char* toString() const
	{
		static char szStr[1024];
		char* pStr = szStr;
		int bytes = 0;
		char szIP[32] = {0};

		dnsServerIp.getString(szIP, sizeof(szIP));
		bytes += sprintf(pStr + bytes, "host name:%s DNS server:%s delay:%llums",
				szHostName, szIP, answerTime-queryTime);

		if(retryCount != 0)
		{
			bytes += sprintf(pStr + bytes, " retry:%u", retryCount);
		}

		bytes += sprintf(pStr + bytes, " IP:");

		for(BYTE i = 0; i < addrNum; i++)
		{
			ipAddrs[i].getString(szIP,sizeof(szIP));
			bytes += sprintf(pStr + bytes, "%s ", szIP);
		}
		return szStr;
	}

private:
	DnsTransNotify msg;
};

#endif /* DNSTRANSINFO_H_ */
