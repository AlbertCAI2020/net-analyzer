/*
 * TcpFlowCtx.h
 *
 *  Created on: 2015-11-26
 *      Author: Administrator
 */

#ifndef TCPFLOWCTX_H_
#define TCPFLOWCTX_H_
#include "PacketTime.h"
#include "TcpStatData.h"

class TcpFlowCtx
{
public:
	TcpFlowCtx()
	{
		reset();
	}

	void reset()
	{
		memset(this, 0, sizeof(TcpFlowCtx));
		bWinScale = -1;
	}

	bool isSegAcked(WORD32 dwSeq, WORD32 dwSegLen);

	PacketTime beginTime;
	PacketTime endTime;
	PacketTime firstPayloadTime;
	PacketTime lastPayloadTime;

	WORD32 dwTotalPktNum;
	WORD32 dwTotalBytes;
	WORD32 dwRetransPktNum;
	WORD32 dwRetransBytes;
	WORD32 dwRetransAckedNum;
	WORD32 dwRetransAckedBytes;
	WORD32 dwFastRetransNum;
	WORD32 dwFastRetransBytes;
	WORD32 dwDisorderPktNum;
	WORD32 dwDisorderBytes;
	WORD32 dwDupAckNum;

	bool bActiveSyn;
	bool bPassiveSyn;
	bool bFin;
	bool bRst;
	WORD16 wMss;
	BYTE bWinScale;

	PacketTime lastacktime;
	PacketTime nextseqtime;
	WORD32 base_seq;
	WORD32 nextseq;
	WORD32 lastack;
	WORD32 window;
	WORD32 dupacknum;
	WORD32 maxseqtobeacked;
	WORD32 lastsegmentflags;

	BYTE lastsacknum;
	T_TCPSAck lastsack[4];

	TcpStatData::DirStat* selfStat;
	TcpStatData::DirStat* sumStat;
};


#endif /* TCPFLOWCTX_H_ */
