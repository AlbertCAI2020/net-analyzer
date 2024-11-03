/*
 * IpStatData.h
 *
 *  Created on: 2015-11-26
 *      Author: Administrator
 */

#ifndef IPSTATDATA_H_
#define IPSTATDATA_H_
#include "Data.h"
#include "PacketStatistics.h"
#include "messageheader.h"
#include "message.pb.h"
using namespace com::zte::netanalysis::capcontrol;

class IpStatData : public Data
{
public:
	IpStatData()
	{
		clear();
	}

	bool serializeToBuffer(BYTE* buff, size_t len) const
	{
		if(len < serializedBytes()){
			return false;
		}
		MsgHeader* hdr = (MsgHeader*)buff;
		hdr->length = msg.ByteSize();
		hdr->type = IP_STAT_NOTIFY;
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

	void clear()
	{
		startTime = 0;
		endTime = 0;
		ulStats.reset();
		dlStats.reset();
		msg.Clear();
	}

	void build()
	{
		msg.set_starttime(startTime);
		msg.set_endtime(endTime);
		msg.set_pktnum(dlStats.getPktNum());
		msg.set_bytes(dlStats.getPktBytes());
	}

	const char* toString() const
	{
		static char str[128];
		snprintf(str, 127, "IP ul(%upkts, %.3fkbps), dl(%upkts, %.3fkbps)",
				ulStats.getPktNum(), ulStats.getPktBytes()*8.0/(endTime-startTime),
				dlStats.getPktNum(), dlStats.getPktBytes()*8.0/(endTime-startTime));
		return str;
	}

	WORD64 startTime;
	WORD64 endTime;
	PacketStatistics ulStats;
	PacketStatistics dlStats;

private:
	IpStatNotify msg;
};

#endif /* IPSTATDATA_H_ */
