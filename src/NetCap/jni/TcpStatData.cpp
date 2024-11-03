/*
 * TcpStatData.cpp
 *
 *  Created on: 2015-12-30
 *      Author: Administrator
 */

#include "TcpStatData.h"
#include "TcpFlowCtx.h"

bool TcpStatData::serializeToBuffer(BYTE* buff, size_t len) const
{
	if(len < serializedBytes()){
		return false;
	}
	MsgHeader* hdr = (MsgHeader*)buff;
	hdr->length = msg.ByteSize();
	hdr->type = TCP_STAT_NOTIFY;
	if(!msg.SerializeToArray(buff + HDRBUFF_LEN,
			len - HDRBUFF_LEN)){
		return false;
	}
	return true;
}

size_t TcpStatData::serializedBytes() const
{
	return HDRBUFF_LEN + msg.ByteSize();
}

void TcpStatData::clear()
{
	startTime = 0;
	endTime = 0;
	ul.reset();
	dl.reset();
	msg.Clear();
}

void TcpStatData::build()
{
	msg.set_sessionid(dwSessionId);
	msg.set_starttime(startTime);
	msg.set_endtime(endTime);

	msg.mutable_dlstat()->set_pktnum(dl.total.getPktNum());
	msg.mutable_dlstat()->set_bytes(dl.total.getPktBytes());
	msg.mutable_dlstat()->set_retransnum(dl.retrans.getPktNum());
	msg.mutable_dlstat()->set_retransbytes(dl.retrans.getPktBytes());
	msg.mutable_dlstat()->set_retransackednum(dl.retransacked.getPktNum());
	msg.mutable_dlstat()->set_retransackedbytes(dl.retransacked.getPktBytes());
}

const char* TcpStatData::toString() const
{
	static char str[128];
	snprintf(str, 127, "TCP(%u) ul(%upkts, %.3fkB/s), dl(%upkts, %.3fkB/s)", dwSessionId,
			ul.total.getPktNum(), ul.total.getPktBytes()*1.0/(endTime-startTime),
			dl.total.getPktNum(), dl.total.getPktBytes()*1.0/(endTime-startTime));
	return str;
}

bool TcpSessionInfo::serializeToBuffer(BYTE* buff, size_t len) const
{
	if(len < serializedBytes()){
		return false;
	}
	MsgHeader* hdr = (MsgHeader*)buff;
	hdr->length = msg.ByteSize();
	hdr->type = TCP_SES_NOTIFY;
	if(!msg.SerializeToArray(buff + HDRBUFF_LEN,
			len - HDRBUFF_LEN)){
		return false;
	}
	return true;
}

size_t TcpSessionInfo::serializedBytes() const
{
	return HDRBUFF_LEN + msg.ByteSize();
}

void TcpSessionInfo::clear()
{
	hasIpPort = false;
	hasRTT = false;
	hasDlInfo = false;
	hasUlInfo = false;

	localIp.setIPv4(0);
	remoteIp.setIPv4(0);
	wLocalPort = 0;
	wRemotePort = 0;

	rtt = 0;

	dl.reset();
	ul.reset();

	msg.Clear();
}

void TcpSessionInfo::build()
{
	msg.set_sessionid(dwSessionId);

	if(hasIpPort)
	{
		char szIp[64] = {0};
		localIp.getString(szIp, sizeof(szIp));
		msg.mutable_ipport()->set_localip(szIp);

		remoteIp.getString(szIp, sizeof(szIp));
		msg.mutable_ipport()->set_remoteip(szIp);

		msg.mutable_ipport()->set_localport(wLocalPort);
		msg.mutable_ipport()->set_remoteport(wRemotePort);
	}

	if(hasRTT)
	{
		msg.set_rtt(rtt);
	}

	if(hasDlInfo)
	{
		msg.mutable_dlinfo()->set_begintime(dl.beginTime);
		msg.mutable_dlinfo()->set_endtime(dl.endTime);
		msg.mutable_dlinfo()->set_firstpayloadtime(dl.firstPayloadTime);
		msg.mutable_dlinfo()->set_lastpayloadtime(dl.lastPayloadTime);

		msg.mutable_dlinfo()->set_totalpktnum(dl.totalPktNum);
		msg.mutable_dlinfo()->set_totalbytes(dl.totalBytes);
		msg.mutable_dlinfo()->set_retranspktnum(dl.retransPktNum);
		msg.mutable_dlinfo()->set_retransbytes(dl.retransBytes);
		msg.mutable_dlinfo()->set_retransackednum(dl.retransAckedNum);
		msg.mutable_dlinfo()->set_retransackedbytes(dl.retransAckedBytes);
		msg.mutable_dlinfo()->set_fastretransnum(dl.fastRetransNum);
		msg.mutable_dlinfo()->set_fastretransbytes(dl.fastRetransBytes);
		msg.mutable_dlinfo()->set_disorderpktnum(dl.disorderPktNum);
		msg.mutable_dlinfo()->set_disorderbytes(dl.disorderBytes);
		msg.mutable_dlinfo()->set_dupacknum(dl.dupAckNum);
	}
}

const char* TcpSessionInfo::toString() const
{
	static char str[256];
	char* pStr = str;

	pStr += sprintf(pStr, "TCP(%u)", dwSessionId);

	if(hasIpPort)
	{
		char ip1[64];
		char ip2[64];
		localIp.getString(ip1, sizeof(ip1));
		remoteIp.getString(ip2, sizeof(ip2));
		pStr += sprintf(pStr," %s:%u <-> %s:%u", ip1, wLocalPort, ip2, wRemotePort);
	}

	if(hasRTT)
	{
		pStr += sprintf(pStr, " RTT:%ums", rtt);
	}

	if(hasUlInfo)
	{
		pStr += sprintf(pStr, " UL:%.3fKB/s", ul.effectivSpeed());
	}

	if(hasDlInfo)
	{
		pStr += sprintf(pStr, " DL:%.3fKB/s", dl.effectivSpeed());
	}

	return str;
}

void TcpSessionInfo::FlowInfo::set(const TcpFlowCtx& flow)
{
	beginTime = flow.beginTime.milliSecond();
	endTime = flow.endTime.milliSecond();
	firstPayloadTime = flow.firstPayloadTime.milliSecond();
	lastPayloadTime = flow.lastPayloadTime.milliSecond();

	totalPktNum = flow.dwTotalPktNum;
	totalBytes = flow.dwTotalBytes;
	retransPktNum = flow.dwRetransPktNum;
	retransBytes = flow.dwRetransBytes;
	retransAckedNum = flow.dwRetransAckedNum;
	retransAckedBytes = flow.dwRetransAckedBytes;
	fastRetransNum = flow.dwFastRetransNum;
	fastRetransBytes = flow.dwFastRetransBytes;
	disorderPktNum = flow.dwDisorderPktNum;
	disorderBytes = flow.dwDisorderBytes;
	dupAckNum = flow.dwDupAckNum;
}

void TcpSessionInfo::FlowInfo::add(const TcpFlowCtx& flow)
{
	WORD64 bt = flow.beginTime.milliSecond();
	WORD64 et = flow.endTime.milliSecond();
	if((beginTime > bt || 0 == beginTime) && bt != 0){
		beginTime = bt;
	}
	if(endTime < et){
		endTime = et;
	}

	WORD64 ft = flow.firstPayloadTime.milliSecond();
	WORD64 lt = flow.lastPayloadTime.milliSecond();
	if((firstPayloadTime > ft || 0 == firstPayloadTime) && ft != 0){
		firstPayloadTime = ft;
	}
	if(lastPayloadTime < lt){
		lastPayloadTime = lt;
	}

	totalPktNum += flow.dwTotalPktNum;
	totalBytes += flow.dwTotalBytes;
	retransPktNum += flow.dwRetransPktNum;
	retransBytes += flow.dwRetransBytes;
	retransAckedNum += flow.dwRetransAckedNum;
	retransAckedBytes += flow.dwRetransAckedBytes;
	fastRetransNum += flow.dwFastRetransNum;
	fastRetransBytes += flow.dwFastRetransBytes;
	disorderPktNum += flow.dwDisorderPktNum;
	disorderBytes += flow.dwDisorderBytes;
	dupAckNum += flow.dwDupAckNum;
}






