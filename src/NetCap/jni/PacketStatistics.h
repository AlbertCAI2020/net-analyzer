/*
 * PacketStatistics.h
 *
 *  Created on: 2015-11-26
 *      Author: Administrator
 */

#ifndef PACKETSTATISTICS_H_
#define PACKETSTATISTICS_H_

class PacketStatistics
{
public:
	PacketStatistics()
	{
		reset();
	}

	~PacketStatistics()
	{
	}

	void reset()
	{
		dwPktNum = 0;
		dwBytes = 0;
	}

	void addPktNum(WORD32 added)
	{
		dwPktNum += added;
	}

	WORD32 getPktNum() const
	{
		return dwPktNum;
	}

	void addPktBytes(WORD32 added)
	{
		dwBytes += added;
	}

	WORD32 getPktBytes() const
	{
		return dwBytes;
	}

protected:
	WORD32 dwPktNum;
	WORD32 dwBytes;
};


#endif /* PACKETSTATISTICS_H_ */
