/*
 * alltest.cpp
 *
 *  Created on: 2015-11-13
 *      Author: Administrator
 */

#ifdef _COMPILE_UNITTEST

#include "Packet.h"
#include "PacketCapture.h"
#include "PacketAnalyzer.h"
#include "DataCollectCenter.h"

void test_capture();
void test_log();

int unit_test_entry(int argc, char* argv[])
{
	//test_log();
	test_capture();
	return 0;
}

void test_pooled_objects()
{
	Packet* pPkt = new Packet;
	IpHeader* ip = new IpHeader;
	pPkt->addHeader(ip);
	TcpHeader* tcp = new TcpHeader;
	pPkt->addHeader(tcp);
	delete pPkt;
}

void test_log()
{
	for(int i = 1; i <= 200; i++)
	{
		LOG("logging %d ...", i);
	}
}

class LogReporter : public DataReporter
{
public:
	void reportData(const Data& data)
	{
		LOG("%s", data.toString());
	}
};


void test_capture()
{
	PacketCapture* capture = PacketCapture::getInstance();
	if(!capture->init())
	{
		LOG("packet capturer init failed!");
		return;
	}
	LOG("packet capturer init success!");

	PacketAnalyzer analyzer;
	capture->setHandler(&analyzer);
	if(capture->hasIfcfgIpv4()){
		analyzer.getProtoStack()->setIpv4Addr(capture->getIpv4Addr());
	}

	DataCollectCenter* dcc = DataCollectCenter::getInstance();
	//dcc->getCollector(IP_STAT_DATA)->setInterval(1000);
	//dcc->getCollector(TCP_STAT_DATA)->setInterval(1000);

	LogReporter reporter;
	dcc->registerReporter(&reporter);


	char szTmp[64];

	while(true)
	{
		bool bret = analyzer.start();
		if(bret){
			LOG("PacketAnalyzer start success!");
		}else{
			LOG("PacketAnalyzer start failed!");
		}

		LOG("enter \"stop\" to stop:");
		scanf("%s", szTmp);
		analyzer.stop();
		LOG("PacketAnalyzer stopped!");

		LOG("totalsecs:%d, totalpkt:%d, droppedpkt:%d",
				analyzer.getTotalSecs(),
				analyzer.getTotalPktNum(),
				analyzer.getDropedPktNum());

		LOG("enter \'c\' to continue, other to quit:");
		scanf("%s", szTmp);
		if(0 == strcmp(szTmp, "c")){
			continue;
		}else{
			break;
		}
	}

	LOG("packet capturer uninitializing...");
	capture->unInit();
	LOG("packet capturer uninitialized.");

}
#endif
