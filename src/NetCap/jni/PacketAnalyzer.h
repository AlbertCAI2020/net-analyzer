/*
 * PacketAnalyzer.h
 *
 *  Created on: 2015-11-10
 *      Author: Administrator
 */

#ifndef PACKETANALYZER_H_
#define PACKETANALYZER_H_
#include "Packet.h"
#include "PacketFile.h"
#include "PacketHandler.h"
#include "ProtocolStack.h"
#include "Thread.h"
#include "MessageQueue.h"

class PacketAnalyzer : public PacketHandler
{
public:
	PacketAnalyzer()
		:m_bStarted(false),
		 m_pktQueue(1000),
		 m_task(*this),
		 m_thread(m_task)
	{
		m_pktQueue.setCleanCallback(deletePkt);
	}

	bool start(const char* saveFile = NULL)
	{
		if(m_bStarted)
		{
			return false;
		}
		resetStats();
		m_pktQueue.reset();
		m_protoStack.reset();
		m_pktFile.close();

		if(NULL != saveFile )
		{
			if(!m_pktFile.open(saveFile, FILE_OPEN_WRITE))
			{
				LOG("open saving file failed!");
				return false;
			}
			m_bSaveFile = true;
		}

		if(!m_thread.start())
		{
			LOG("start analyzer thread failed!");
			return false;
		}
		m_bStarted = true;
		m_startTime = time(NULL);
		return true;
	}

	void stop()
	{
		if(!m_bStarted)
		{
			return;
		}
		m_bStarted = false;

		while(!m_pktQueue.enqueue(NULL))
		{
		}
		m_thread.join();
		m_protoStack.reportTotalStat();

		m_pktQueue.reset();
		m_protoStack.reset();
		m_pktFile.close();
		m_endTime = time(NULL);
	}

	ProtocolStack* getProtoStack()
	{
		return &m_protoStack;
	}

	time_t getStartTime(){
		return m_startTime;
	}
	time_t getEndTime(){
		return m_endTime;
	}
	unsigned int getTotalSecs(){
		return m_endTime - m_startTime;
	}
	unsigned int getTotalPktNum(){
		return m_totalPktNum;
	}
	unsigned int getDropedPktNum(){
		return m_dropedPktNum;
	}

private:

	class AnalyzeTask : public Task
	{
	public:
		AnalyzeTask(PacketAnalyzer& analyzer)
			:m_analyzer(analyzer)
		{
		}

		void run()
		{
			m_analyzer.analyzePacket();
		}

	private:
		PacketAnalyzer& m_analyzer;
	};

	bool handlePacket(Packet* pPkt)
	{
		if(!m_bStarted)
		{
			return false;
		}
		m_totalPktNum++;
		if(!m_pktQueue.enqueue(pPkt))
		{
			m_dropedPktNum++;
			return false;
		}
		return true;
	}

	static void deletePkt(Packet* pPkt)
	{
		delete pPkt;
	}

	void analyzePacket()
	{
		Packet* pPkt = NULL;
		PacketHandler* pHandler = m_protoStack.getProtoHandler(HDR_TYPE_IPV4);
		if(NULL == pHandler)
		{
			return;
		}

		while(true)
		{
			pPkt = m_pktQueue.dequeue();
			if(NULL == pPkt)
			{
				break;
			}
			if(m_bSaveFile)
			{
				m_pktFile.write(pPkt);
			}
			if(!pHandler->handlePacket(pPkt))
			{
				delete pPkt;
			}
		}
	}

	void resetStats()
	{
		m_bSaveFile = false;
		m_startTime = 0;
		m_endTime = 0;
		m_totalPktNum = 0;
		m_dropedPktNum = 0;
	}

	volatile bool m_bStarted;
	bool m_bSaveFile;

	BlockableMsgQueue<Packet> m_pktQueue;
	AnalyzeTask m_task;
	Thread m_thread;
	ProtocolStack m_protoStack;
	PacketFile m_pktFile;

	time_t m_startTime;
	time_t m_endTime;
	unsigned int m_totalPktNum;
	unsigned int m_dropedPktNum;
};

#endif /* PACKETANALYZER_H_ */
