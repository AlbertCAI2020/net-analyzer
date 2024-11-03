/*
 * TcpAnalyzer.cpp
 *
 *  Created on: 2015-11-26
 *      Author: Administrator
 */

#include "TcpSessionHandler.h"

#define FLOW_STAT_ADD(flow, pktstat, pkts, bytes) \
	flow->selfStat->pktstat.addPktNum(pkts);\
	flow->selfStat->pktstat.addPktBytes(bytes);\
	if(flow->sumStat){\
		flow->sumStat->pktstat.addPktNum(pkts);\
		flow->sumStat->pktstat.addPktBytes(bytes);\
	}

bool TcpSessionHandler::handlePacket(Packet* pPkt)
{
	PacketHeader* pHdr = pPkt->getBackHeader();
	if(NULL == pHdr || pHdr->getType() != HDR_TYPE_TCP)
	{
		return false;
	}
	TcpHeader* pTcpHdr = (TcpHeader*)pHdr;
	TcpFlowCtx* fwd = NULL;
	TcpFlowCtx* rev = NULL;

	if(PKT_DIR_DOWNLINK == pPkt->getDirection())
	{
		fwd = &m_downlinkFlow;
		rev = &m_uplinkFlow;
	}
	else if(PKT_DIR_UPLINK == pPkt->getDirection())
	{
		fwd = &m_uplinkFlow;
		rev = &m_downlinkFlow;
	}
	else
	{
		return false;
	}

	handleTcpPktInFlow(fwd, rev, pTcpHdr, pPkt);
	delete pPkt;
	return true;
}

const Data& TcpSessionHandler::collectData(WORD64 startTime, WORD64 endTime)
{
	m_periodData.startTime = startTime;
	m_periodData.endTime = endTime;
	m_periodData.build();
	return m_periodData;
}

void TcpSessionHandler::clearData()
{
	m_periodData.clear();
}

void TcpSessionHandler::reportTotalStat()
{
	if(NULL == _reporter)
	{
		return;
	}
	TcpSessionInfo data;
	data.dwSessionId = m_dwSessionId;
	data.hasDlInfo = true;
	data.dl.set(m_downlinkFlow);

	data.build();
	_reporter->reportData(data);
}

void TcpSessionHandler::reportRTT()
{
	if(NULL == _reporter)
	{
		return;
	}
	TcpSessionInfo data;
	data.dwSessionId = m_dwSessionId;
	data.hasRTT = true;
	data.rtt = ts_first_rtt.milliSecond();

	data.build();
	_reporter->reportData(data);
}

void TcpSessionHandler::setFlowStats()
{
	m_uplinkFlow.selfStat = &m_periodData.ul;
	m_downlinkFlow.selfStat = &m_periodData.dl;
	if(NULL != m_pSumData)
	{
		m_uplinkFlow.sumStat = &m_pSumData->ul;
		m_downlinkFlow.sumStat = &m_pSumData->dl;
	}
	else
	{
		m_uplinkFlow.sumStat = NULL;
		m_downlinkFlow.sumStat = NULL;
	}
}

/*Idea for gt: either x > y, or y is much bigger(assume wrap)*/
#define GT_SEQ(x,y) ((int)((y)-(x)) < 0)
#define LT_SEQ(x,y) ((int)((x)-(y)) < 0)
#define GE_SEQ(x,y) ((int)((y)-(x)) <= 0)
#define LE_SEQ(x,y) ((int)((x)-(y)) <= 0)
#define EQ_SEQ(x,y) (x)==(y)

#define TCP_A_RETRANSMISSION          0x0001
#define TCP_A_LOST_PACKET             0x0002
#define TCP_A_ACK_LOST_PACKET         0x0004
#define TCP_A_KEEP_ALIVE              0x0008
#define TCP_A_DUPLICATE_ACK           0x0010
#define TCP_A_ZERO_WINDOW             0x0020
#define TCP_A_ZERO_WINDOW_PROBE       0x0040
#define TCP_A_ZERO_WINDOW_PROBE_ACK   0x0080
#define TCP_A_KEEP_ALIVE_ACK          0x0100
#define TCP_A_OUT_OF_ORDER            0x0200
#define TCP_A_FAST_RETRANSMISSION     0x0400
#define TCP_A_WINDOW_UPDATE           0x0800
#define TCP_A_WINDOW_FULL             0x1000
#define TCP_A_REUSED_PORTS            0x2000
#define TCP_A_SPURIOUS_RETRANSMISSION 0x4000

void TcpSessionHandler::handleTcpPktInFlow(TcpFlowCtx* fwd, TcpFlowCtx* rev,
		TcpHeader* hdr, Packet* pkt)
{
	T_TCPHead* st = hdr->getStruct();
	WORD32 window = st->wWinSize;
	WORD32 seglen = hdr->getPayloadLen();
	WORD32 nextseq = 0;
	WORD32 flags = 0;

	if(st->bSyn)
	{
		if(fwd->base_seq != 0 && st->dwSeqNum != fwd->base_seq)
		{
			/*reused port*/
			//fwd->reset();
			//rev->reset();
			//setSumStat(m_pSumData);
		}

		if(!st->bAck)
		{
			fwd->bActiveSyn = true;
			fwd->bWinScale = hdr->getOptWinScale();
			ts_mru_syn = pkt->getTime();
		}
		else
		{
			fwd->bPassiveSyn = true;
			if((BYTE)-1 == hdr->getOptWinScale() ||
				(BYTE)-1 == rev->bWinScale)
			{
				fwd->bWinScale = 0;
				rev->bWinScale = 0;
			}
			else
			{
				fwd->bWinScale = hdr->getOptWinScale();
			}
			ts_mru_synack = pkt->getTime();
		}
		fwd->wMss = hdr->getOptMss();
	}
	else
	{
		if((BYTE)-1 != fwd->bWinScale && (BYTE)-1 != rev->bWinScale)
		{
			window = window<<fwd->bWinScale;
		}
	}

	if(!st->bSyn && st->bAck && ts_mru_syn.microSecond() != 0 &&
		ts_first_rtt.microSecond() == 0)
	{
		ts_first_rtt = pkt->getTime() - ts_mru_syn;
		reportRTT();
	}

	FLOW_STAT_ADD(fwd, total, 1, seglen);
	fwd->dwTotalPktNum++;
	fwd->dwTotalBytes += seglen;

	if(fwd->beginTime.microSecond() == 0){
		fwd->beginTime = pkt->getTime();
	}
	fwd->endTime = pkt->getTime();

	if(0 != seglen)
	{
		if(fwd->firstPayloadTime.microSecond() == 0){
			fwd->firstPayloadTime = pkt->getTime();
		}
		fwd->lastPayloadTime = pkt->getTime();
	}

	if(st->bFin)
	{
		fwd->bFin = true;
	}
	if(st->bRst)
	{
		fwd->bRst = true;
	}

	if(0 == fwd->base_seq)
	{
		fwd->base_seq = st->dwSeqNum;
		if(!st->bSyn){
			fwd->base_seq--;
		}
	}

	if(rev->base_seq == 0 && st->bAck)
	{
		rev->base_seq = st->dwAckNum - 1;
	}

	if(seglen == 1 && st->dwSeqNum == fwd->nextseq && rev->window == 0)
	{
		/*zero window probe*/
		flags |= TCP_A_ZERO_WINDOW_PROBE;
		goto finished_fwd;
	}

	if(window == 0 && (st->bSyn|st->bFin|st->bRst) == 0)
	{
		/*zero window*/
		flags |= TCP_A_ZERO_WINDOW;
	}

	if(fwd->nextseq && GT_SEQ(st->dwSeqNum, fwd->nextseq) && !st->bRst)
	{
		/*packet lost*/
		flags |= TCP_A_LOST_PACKET;
	}

	if(seglen <= 1 &&
		st->dwSeqNum == fwd->nextseq - 1 &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		/*keep alive*/
		flags |= TCP_A_KEEP_ALIVE;
	}

	if(seglen == 0 && window != 0 && window != fwd->window &&
		st->dwSeqNum == fwd->nextseq && st->dwAckNum == fwd->lastack &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		/*window update*/
		flags |= TCP_A_WINDOW_UPDATE;
	}

	if(seglen > 0 && (BYTE)-1 != rev->bWinScale &&
		(st->dwSeqNum + seglen) == (rev->lastack + rev->window) &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		/*window full*/
		flags |= TCP_A_WINDOW_FULL;
	}

	if(seglen == 0 && window == fwd->window &&
		fwd->nextseq != 0 && st->dwAckNum == fwd->lastack &&
		(rev->lastsegmentflags&TCP_A_KEEP_ALIVE) &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		/*keep alive ack*/
		flags |= TCP_A_KEEP_ALIVE_ACK;
		goto finished_fwd;
	}

	if(seglen == 0 && window == 0 && window == fwd->window &&
		st->dwSeqNum == fwd->nextseq && st->dwAckNum == fwd->lastack &&
		(rev->lastsegmentflags&TCP_A_ZERO_WINDOW_PROBE) &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		flags |= TCP_A_ZERO_WINDOW_PROBE_ACK;
		goto finished_fwd;
	}

	if(seglen == 0 && window != 0 && window == fwd->window &&
		st->dwSeqNum == fwd->nextseq && st->dwAckNum == fwd->lastack &&
		(st->bSyn|st->bFin|st->bRst) == 0)
	{
		fwd->dwDupAckNum++;
		fwd->dupacknum++;
		flags |= TCP_A_DUPLICATE_ACK;
	}

finished_fwd:

	if(0 == (flags&TCP_A_DUPLICATE_ACK))
	{
		fwd->dupacknum = 0;
	}

	if(rev->maxseqtobeacked != 0 &&
		GT_SEQ(st->dwAckNum, rev->maxseqtobeacked) &&
		st->bAck)
	{
		/*acked lost packet*/
		flags |= TCP_A_ACK_LOST_PACKET;
		rev->maxseqtobeacked = rev->nextseq;
	}

	/*retransmission, fast retransmission, out-of-order*/
	if((seglen > 0 || (st->bSyn|st->bFin) != 0) &&
		fwd->nextseq != 0 && LT_SEQ(st->dwSeqNum, fwd->nextseq))
	{
		WORD64 t1 = 0;
		WORD64 t2 = 0;
		WORD64 ooo_thres = 0;
		WORD32 seq_inc = seglen;
		if(st->bSyn || st->bFin){
			seq_inc++;
		}

		if(ts_first_rtt.microSecond() == 0){
			ooo_thres = 30000;
		}else{
			ooo_thres = ts_first_rtt.microSecond();
		}

		if(flags&TCP_A_KEEP_ALIVE){
			goto finished_checking_retransmission_type;
		}

		t1 = pkt->getTime().microSecond() - rev->lastacktime.microSecond();
		t2 = pkt->getTime().microSecond() - fwd->nextseqtime.microSecond();

		if(rev->isSegAcked(st->dwSeqNum, seq_inc))
		{
			fwd->dwRetransAckedNum++;
			fwd->dwRetransAckedBytes += seglen;
			fwd->dwRetransPktNum++;
			fwd->dwRetransBytes += seglen;
			FLOW_STAT_ADD(fwd, retrans, 1, seglen);
			FLOW_STAT_ADD(fwd, retransacked, 1, seglen);
		}
		else if(rev->dupacknum >= 2 && fwd->maxseqtobeacked != 0 &&
			LT_SEQ(st->dwSeqNum, fwd->maxseqtobeacked) &&
			((isDownlinkFlow(fwd) && t1 < ts_first_rtt.microSecond() + 20000) ||
			 (isUplinkFlow(fwd) && t1 < 20000) ))
		{
			fwd->dwFastRetransNum++;
			fwd->dwFastRetransBytes += seglen;
			fwd->dwRetransPktNum++;
			fwd->dwRetransBytes += seglen;
			FLOW_STAT_ADD(fwd, retrans, 1, seglen);
		}
		else if(t2 < ooo_thres && fwd->nextseq != st->dwSeqNum + seglen)
		{
			fwd->dwDisorderPktNum++;
			fwd->dwDisorderBytes += seglen;
		}
		else
		{
			fwd->dwRetransPktNum++;
			fwd->dwRetransBytes += seglen;
			FLOW_STAT_ADD(fwd, retrans, 1, seglen);
		}
	}

finished_checking_retransmission_type:

	nextseq = st->dwSeqNum + seglen;
	if(st->bSyn || st->bFin){
		nextseq++;
	}

	if(GT_SEQ(nextseq, fwd->nextseq) || 0 == fwd->nextseq)
	{
		if(0 == (flags&TCP_A_ZERO_WINDOW_PROBE)){
			fwd->nextseq = nextseq;
			fwd->nextseqtime = pkt->getTime();
		}
	}

	if(EQ_SEQ(st->dwSeqNum, fwd->maxseqtobeacked) || 0 == fwd->maxseqtobeacked)
	{
		if(0 == (flags&TCP_A_ZERO_WINDOW_PROBE)){
			fwd->maxseqtobeacked = fwd->nextseq;
		}
	}

	fwd->window = window;
	fwd->lastack = st->dwAckNum;
	fwd->lastacktime = pkt->getTime();
	fwd->lastsacknum = hdr->getSAckNum();
	for(BYTE i = 0; i < fwd->lastsacknum; i++){
		fwd->lastsack[i] = hdr->getSAck(i);
	}
	fwd->lastsegmentflags = flags;

	return;
}


bool TcpFlowCtx::isSegAcked(WORD32 dwSeq, WORD32 dwSegLen)
{
	if(LE_SEQ(dwSeq + dwSegLen, lastack)){
		return true;
	}

	for(int i = 0; i < lastsacknum; i++)
	{
		if(GE_SEQ(dwSeq, lastsack[i].dwLeftEdge) &&
		   LE_SEQ(dwSeq + dwSegLen, lastsack[i].dwRightEdge))
		{
			return true;
		}
	}

	return false;
}






