/*
 * Packet.h
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */

#ifndef PACKET_H_
#define PACKET_H_
#include "PacketHeader.h"
#include "PacketBuffer.h"
#include "PacketTime.h"
#include "Log.h"

#include <vector>

enum PktDirection
{
	PKT_DIR_UNKNOWN,
	PKT_DIR_UPLINK,
	PKT_DIR_DOWNLINK,
};

class Packet: public ThreadSafePooledObject<Packet>
{
public:
	Packet()
		:m_direction(PKT_DIR_UNKNOWN)
	{
	}

	~Packet()
	{
		clear();
	}

	BYTE getHeaderCount() const
	{
		return m_pktHeaders.size();
	}

	PacketHeader* getHeader(BYTE bIndex) const
	{
		if(bIndex >= m_pktHeaders.size()){
			return NULL;
		}
		return m_pktHeaders[bIndex];
	}

	PacketHeader* getBackHeader() const
	{
		if(m_pktHeaders.empty()){
			return NULL;
		}
		return m_pktHeaders.back();
	}

	PacketHeader* getFrontHeader() const
	{
		if(m_pktHeaders.empty()){
			return NULL;
		}
		return m_pktHeaders.front();
	}

	void addHeader(PacketHeader* pHdr)
	{
		m_pktHeaders.push_back(pHdr);
	}

	const PacketTime& getTime() const
	{
		return m_time;
	}

	void setTime(const PacketTime& t)
	{
		m_time = t;
	}

	PktDirection getDirection() const
	{
		return m_direction;
	}

	void setDirection(PktDirection dir)
	{
		m_direction = dir;
	}

	void addBuffer(PacketBuffer* pBuffer)
	{
		m_pktBuffers.push_back(pBuffer);
	}

	PacketBuffer* getBuffer(WORD32 dwIndex) const
	{
		if(dwIndex >= m_pktBuffers.size()){
			return NULL;
		}
		return m_pktBuffers[dwIndex];
	}

	WORD32 getBufferCount() const
	{
		return m_pktBuffers.size();
	}

	WORD32 getPktLength() const
	{
		WORD32 dwPktLen = 0;
		for(size_t i = 0; i < m_pktBuffers.size(); i++)
		{
			dwPktLen += m_pktBuffers[i]->getLength();
		}
		return dwPktLen;
	}

	WORD32 getPktData(WORD32 dwPos, BYTE* pBuff, WORD32 dwLen) const
	{
		WORD32 dwCopyed = 0;

		WORD32 dwBufferPos = 0;
		size_t beginBufferIdx = 0;
		for(; beginBufferIdx < m_pktBuffers.size(); beginBufferIdx++)
		{
			PacketBuffer* pBuffer = m_pktBuffers[beginBufferIdx];
			WORD16 wBufferLen = pBuffer->getLength();
			if(dwPos < dwBufferPos + wBufferLen)
			{
				BYTE* buff = pBuffer->getBuffer() + (dwPos - dwBufferPos);
				WORD16 wCopyBytes = dwBufferPos + wBufferLen - dwPos;
				wCopyBytes = (dwLen-dwCopyed)>wCopyBytes?wCopyBytes : (dwLen-dwCopyed);
				memcpy(pBuff + dwCopyed, buff, wCopyBytes);
				dwCopyed += wCopyBytes;
				break;
			}
			dwBufferPos += pBuffer->getLength();
		}
		if(dwCopyed == dwLen)
		{
			return dwCopyed;
		}

		for(size_t i = beginBufferIdx+1; i < m_pktBuffers.size(); i++)
		{
			PacketBuffer* pBuffer = m_pktBuffers[beginBufferIdx];
			WORD16 wBufferLen = pBuffer->getLength();
			BYTE* buff = pBuffer->getBuffer();
			WORD16 wCopyBytes = (dwLen-dwCopyed)>wBufferLen?wBufferLen : (dwLen-dwCopyed);
			memcpy(pBuff + dwCopyed, buff, wCopyBytes);
			dwCopyed += wCopyBytes;
			if(dwCopyed == dwLen)
			{
				return dwCopyed;
			}

		}

		return dwCopyed;
	}

	WORD16 consumePktData(WORD16 bytes)
	{
		WORD16 wConsumed = 0;
		for(size_t i = 0; i < m_pktBuffers.size(); i++)
		{
			PacketBuffer* pBuffer = m_pktBuffers[i];
			wConsumed += pBuffer->consumeData(bytes - wConsumed);
			if(wConsumed == bytes)
			{
				return wConsumed;
			}
		}
		return wConsumed;
	}


private:
	void clear()
	{
		for(size_t i = 0; i < m_pktHeaders.size(); i++)
		{
			delete m_pktHeaders[i];
		}
		m_pktHeaders.clear();

		for(size_t i = 0; i < m_pktBuffers.size(); i++)
		{
			delete m_pktBuffers[i];
		}
		m_pktBuffers.clear();
	}

	PacketTime m_time;
	PktDirection m_direction;
	std::vector<PacketHeader*> m_pktHeaders;
	std::vector<PacketBuffer*> m_pktBuffers;

};


#endif /* PACKET_H_ */
