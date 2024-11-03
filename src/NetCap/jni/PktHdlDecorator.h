/*
 * PktDataCollector.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

/*packet-driven data collector,
 * using GOF decorator pattern
 *                    --H.B.Cai
 */

#ifndef PKTHDLDECORATOR_H_
#define PKTHDLDECORATOR_H_
#include "Packet.h"
#include "PacketHandler.h"
#include "DataCollector.h"

class PktHdlDecorator : public PacketHandler
{
public:
	PktHdlDecorator(PacketHandler& hdl, DataCollector& cltr)
		:_handler(hdl), _collector(cltr)
	{
	}

	bool handlePacket(Packet* pPkt)
	{
		_collector.timeDriven(pPkt->getTime().milliSecond());
		return _handler.handlePacket(pPkt);
	}

protected:
	PacketHandler& _handler;
	DataCollector& _collector;
};

#endif /* PKTDATACOLLECTOR_H_ */

