/*
 * TcpStatData.h
 *
 *  Created on: 2015-11-26
 *      Author: Administrator
 */

#ifndef TCPSTATDATA_H_
#define TCPSTATDATA_H_
#include "Data.h"
#include "IpAddress.h"
#include "PacketStatistics.h"
#include "messageheader.h"
#include "message.pb.h"
using namespace com::zte::netanalysis::capcontrol;

class TcpFlowCtx;

class TcpStatData : public Data
{
public:
	TcpStatData()
		:dwSessionId(0)
	{
		clear();
	}

	bool serializeToBuffer(BYTE* buff, size_t len) const;

	size_t serializedBytes() const;

	void clear();

	void build();

	const char* toString() const;

	WORD64 startTime;
	WORD64 endTime;
	WORD32 dwSessionId;//0 表示所有会话的综合总计

	struct DirStat
	{
		PacketStatistics total;
		PacketStatistics retrans;
		PacketStatistics retransacked;

		void reset(){
			total.reset();
			retrans.reset();
			retransacked.reset();
		}
	};
	DirStat ul;
	DirStat dl;

private:
	TcpStatNotify msg;
};

class TcpSessionInfo : public Data
{
public:
	TcpSessionInfo()
		:dwSessionId(0)
	{
		clear();
	}

	bool serializeToBuffer(BYTE* buff, size_t len) const;

	size_t serializedBytes() const;

	void clear();

	void build();

	const char* toString() const;

	WORD32 dwSessionId;
	bool hasIpPort;
	bool hasRTT;
	bool hasDlInfo;
	bool hasUlInfo;

	IpAddress localIp;
	IpAddress remoteIp;
	WORD16 wLocalPort;
	WORD16 wRemotePort;

	WORD32 rtt;

	struct FlowInfo
	{
		WORD64 beginTime;
		WORD64 endTime;
		WORD64 firstPayloadTime;
		WORD64 lastPayloadTime;

		WORD32 totalPktNum;
		WORD32 totalBytes;
		WORD32 retransPktNum;
		WORD32 retransBytes;
		WORD32 retransAckedNum;
		WORD32 retransAckedBytes;
		WORD32 fastRetransNum;
		WORD32 fastRetransBytes;
		WORD32 disorderPktNum;
		WORD32 disorderBytes;

		WORD32 dupAckNum;

		void reset(){
			memset(this, 0, sizeof(FlowInfo));
		}

		void set(const TcpFlowCtx& flow);

		void add(const TcpFlowCtx& flow);

		double effectivSpeed() const
		{
			long interval = lastPayloadTime-firstPayloadTime;
			if(0 != interval){
				return (double)(totalBytes-retransAckedBytes)/interval;
			}
			return 0.0;
		}
	};

	FlowInfo dl;
	FlowInfo ul;

private:
	TcpSessionNotify msg;
};


#endif /* TCPSTATDATA_H_ */
