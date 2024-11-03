/*
 * PacketTime.h
 *
 *  Created on: 2015-11-10
 *      Author: Administrator
 */

#ifndef PACKETTIME_H_
#define PACKETTIME_H_
#include "commdef.h"

class PacketTime
{
public:
	PacketTime()
		:m_dwTimeSec(0),
		 m_qwMilliSecs(0),
		 m_qwMicroSecs(0)
	{
	}

	void setTime(WORD32 sec, WORD32 usec)
	{
		m_dwTimeSec = sec;

		m_qwMilliSecs = ((WORD64)sec)*1000
				+ usec/1000;
		m_qwMicroSecs = ((WORD64)sec)*1000000
				+ usec;
	}

	WORD32 second() const
	{
		return m_dwTimeSec;
	}

	WORD64 milliSecond() const
	{
		return m_qwMilliSecs;
	}

	WORD64 microSecond() const
	{
		return m_qwMicroSecs;
	}

	PacketTime operator-(const PacketTime& other) const
	{
		PacketTime time = *this;
		time.m_qwMicroSecs -= other.m_qwMicroSecs;
		time.m_qwMilliSecs = time.m_qwMicroSecs/1000;
		time.m_dwTimeSec = time.m_qwMicroSecs/1000000;
		return time;
	}

private:
	WORD32 m_dwTimeSec;
	WORD64 m_qwMilliSecs;
	WORD64 m_qwMicroSecs;
};

#endif /* PACKETTIME_H_ */
