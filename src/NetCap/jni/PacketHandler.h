/*
 * PacketHandler.h
 *
 *  Created on: 2015-11-12
 *      Author: Administrator
 */

#ifndef PACKETHANDLER_H_
#define PACKETHANDLER_H_

class Packet;

class PacketHandler
{
public:
	virtual ~PacketHandler(){}

	//if the handler do not handle and returns false,
	//you are the responsibility to handle it.
	virtual bool handlePacket(Packet* pPkt) = 0;
};


#endif /* PACKETHANDLER_H_ */
