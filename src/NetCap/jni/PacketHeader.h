/*
 * PacketHeader.h
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */

#ifndef PACKETHEADER_H_
#define PACKETHEADER_H_
#include "commdef.h"
#include "protocoldefs.h"
#include "ObjectPool.h"

enum HdrType
{
	HDR_TYPE_IPV4,
	HDR_TYPE_IPV6,
	HDR_TYPE_TCP,
	HDR_TYPE_UDP,
	HDR_TYPE_DNS
};

class Packet;

class PacketHeader
{
public:
	PacketHeader(HdrType type)
		:m_type(type),
		 m_len(0)
	{
	}

	virtual ~PacketHeader()
	{
	}

	HdrType getType() const
	{
		return m_type;
	}

	WORD16 getLength()
	{
		return m_len;
	}

	virtual bool parseFromPkt(Packet* pPkt){
		return false;
	}

protected:
	HdrType m_type;
	WORD16 m_len;
};

template<typename T>
class HeaderStruct
{
public:
	HeaderStruct()
	{
		memset(&m_struct, 0, sizeof(T));
	}
	~HeaderStruct()
	{
	}
	T* getStruct()
	{
		return &m_struct;
	}

	const T* getStruct() const
	{
		return &m_struct;
	}
protected:
	T m_struct;
};

class IpHeader : public PacketHeader,
	public HeaderStruct<T_IPV4Head>,
	public ThreadSafePooledObject<IpHeader>
{
public:
	IpHeader()
		:PacketHeader(HDR_TYPE_IPV4)
	{
	}

	~IpHeader()
	{
	}

	bool parseFromPkt(Packet* pPkt);
};

class TcpHeader : public PacketHeader,
	public HeaderStruct<T_TCPHead>,
	public ThreadSafePooledObject<TcpHeader>
{
public:
	TcpHeader()
		:PacketHeader(HDR_TYPE_TCP)
	{
		wPayloadLen = 0;
		dwTcpOptFlag = 0;
		wMss = 0;
		bWinScale = -1;
		bSAckBlkNum = 0;
	}

	~TcpHeader()
	{
	}

	bool parseFromPkt(Packet* pPkt);

	void setPayloadLen(WORD16 len)
	{
		wPayloadLen = len;
	}

	WORD16 getPayloadLen() const
	{
		return wPayloadLen;
	}

	/*TCP options*/
#define TCP_HASOPT_MSS 1
#define TCP_HASOPT_WINSCALE 2
#define TCP_HASOPT_SACKPERM 4
#define TCP_HASOPT_SACK 8

	bool hasOptFlag(int flag) const
	{
		return 0 != (dwTcpOptFlag&flag);
	}

	void setOptMss(WORD16 wMss)
	{
		this->wMss = wMss;
		dwTcpOptFlag |= TCP_HASOPT_MSS;
	}

	WORD16 getOptMss() const
	{
		return wMss;
	}

	void setOptWinScale(BYTE bWinScale)
	{
		this->bWinScale = bWinScale;
		dwTcpOptFlag |= TCP_HASOPT_WINSCALE;
	}

	BYTE getOptWinScale() const
	{
		return bWinScale;
	}

	BYTE getSAckNum() const
	{
		return bSAckBlkNum;
	}

	bool addSAck(const T_TCPSAck& tSAck)
	{
		if(bSAckBlkNum >= 4){
			return false;
		}
		aTcpSack[bSAckBlkNum] = tSAck;
		dwTcpOptFlag |= TCP_HASOPT_SACK;
		return true;
	}

	const T_TCPSAck& getSAck(BYTE i) const
	{
		if(i >= 4){
			return aTcpSack[3];
		}
		return aTcpSack[i];
	}

private:
	WORD16 wPayloadLen;
	WORD16 dwTcpOptFlag;
	WORD16 wMss;
	BYTE bWinScale;
	BYTE bSAckBlkNum;
	T_TCPSAck aTcpSack[4];
};

class UdpHeader : public PacketHeader,
	public HeaderStruct<T_UDPHead>,
	public ThreadSafePooledObject<UdpHeader>
{
public:
	UdpHeader()
		:PacketHeader(HDR_TYPE_UDP)
	{
	}

	~UdpHeader()
	{
	}

	bool parseFromPkt(Packet* pPkt);
};

#endif /* PACKETHEADER_H_ */
