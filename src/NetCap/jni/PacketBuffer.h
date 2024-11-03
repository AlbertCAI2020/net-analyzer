/*
 * PacketBuffer.h
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */

#ifndef PACKETBUFFER_H_
#define PACKETBUFFER_H_
#include "commdef.h"
#include "ObjectPool.h"

#define PKT_BUFF_LEN 1500

class PacketBuffer : public ThreadSafePooledObject<PacketBuffer>
{
public:
	PacketBuffer()
	{
		reset();
	}

	BYTE* getBuffer()
	{
		return (BYTE*)m_buff + m_consumed;
	}
	WORD16 getLength()
	{
		return m_len;
	}

	WORD16 pushData(const BYTE* buff, WORD16 len)
	{
		WORD16 wRemain = PKT_BUFF_LEN - m_consumed - m_len;
		WORD16 wPushed = len > wRemain ? wRemain : len;
		memcpy((BYTE*)m_buff + m_consumed + m_len, buff, wPushed);
		m_len += wPushed;
		return wPushed;
	}

	WORD16 consumeData(WORD16 bytes)
	{
		WORD16 wConsumed = bytes > m_len ? m_len : bytes;
		m_consumed += wConsumed;
		m_len -= wConsumed;
		return wConsumed;
	}

private:

	void reset()
	{
		memset(m_buff, 0, sizeof(m_buff));
		m_len = 0;
		m_consumed = 0;
	}
	BYTE m_buff[PKT_BUFF_LEN];
	WORD16 m_len;
	WORD16 m_consumed;
};


#endif /* PACKETBUFFER_H_ */
