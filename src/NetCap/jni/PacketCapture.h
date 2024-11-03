/*
 * PacketCapture.h
 *
 *  Created on: 2015-11-5
 *      Author: Administrator
 */

#ifndef PACKETCAPTURE_H_
#define PACKETCAPTURE_H_
#include <pcap.h>
#include <pcap/sll.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <time.h>
#include <string.h>
#include <errno.h>

#include "Log.h"
#include "Thread.h"
#include "Packet.h"
#include "PacketHandler.h"

class PacketCapture
{
public:

	static PacketCapture* getInstance()
	{
		static PacketCapture instance;
		return &instance;
	}

	void setHandler(PacketHandler* pHandler)
	{
		m_pHandler = pHandler;
	}

	bool init();
	void unInit();
	bool hasIfcfgIpv4();
	WORD32 getIpv4Addr();
	WORD32 getIpv4Mask();

private:

	PacketCapture()
		:m_task(*this),
		 m_thread(m_task),
		 m_pHandler(NULL),
		 m_pcap(NULL),
		 m_datalink(0),
		 m_ifaddrIpv4(0)
	{
	}

	void OnPacketCaptured(const struct pcap_pkthdr* pkthdr, const u_char*data);
	bool ifcfg(const char* device, char* errbuf);

	class CaptureTask : public Task
	{
	public:
		CaptureTask(PacketCapture& cap)
			:_cap(cap),_quit(false)
		{
		}

		void quit()
		{
			_quit = true;
		}

		void run()
		{
			while(!_quit){
				pcap_dispatch(_cap.m_pcap, 1, capture_callback, (u_char*)&_cap);
			}
		}

	private:
		static void	capture_callback(u_char *user, const struct pcap_pkthdr *h,
					const u_char *sp)
		{
			PacketCapture* cap = (PacketCapture*)user;
			cap->OnPacketCaptured(h, sp);
		}
		PacketCapture& _cap;
		volatile bool _quit;
	};

	CaptureTask m_task;
	Thread m_thread;

	PacketHandler* m_pHandler;
	pcap_t* m_pcap;
	int m_datalink;

	bool m_hasIfcfgIpv4;
	WORD32 m_ifaddrIpv4;
	WORD32 m_ifmaskIpv4;
};

#endif /* PACKETCAPTURE_H_ */
