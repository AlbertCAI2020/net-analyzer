/*
 * PktDataReporter.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

#ifndef PKTDATAREPORTER_H_
#define PKTDATAREPORTER_H_
#include "DataReporter.h"
#include "NamedPipe.h"
#include "Log.h"

class PktDataReporter : public DataReporter
{
public:
	PktDataReporter(NamedPipe& pipe)
		:m_writepipe(pipe)
	{
	}

	void reportData(const Data& data)
	{
		size_t len = data.serializedBytes();
		if(0 == len)
		{
			return;
		}
		if(len > sizeof(m_databuff))
		{
			LOG("Reporter error:too many data bytes!");
			return;
		}
		if(!data.serializeToBuffer(m_databuff, sizeof(m_databuff)))
		{
			LOG("Reporter error:data serialize error!");
			return;
		}
		if(len != m_writepipe.Write(m_databuff, len))
		{
			LOG("Reporter error:reporter-pipe write error:%d!", m_writepipe.GetLastErr());
		}
	}

private:
	NamedPipe& m_writepipe;
	BYTE m_databuff[512];
};


#endif /* PKTDATAREPORTER_H_ */
