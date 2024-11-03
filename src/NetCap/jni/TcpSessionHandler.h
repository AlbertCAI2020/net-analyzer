/*
 * TcpSessionHandler.h
 *
 *  Created on: 2015-11-24
 *      Author: Administrator
 */

#ifndef TCPANALYZER_H_
#define TCPANALYZER_H_
#include "commdef.h"
#include "ObjectPool.h"
#include "PacketHandler.h"
#include "PktHdlDecorator.h"
#include "DataCollector.h"
#include "TcpStatData.h"
#include "TcpFlowCtx.h"
#include "PacketQuintuple.h"
#include <map>

class TcpSesHdlMgr;

class TcpSessionHandler : public PacketHandler,
					public DataCollector,
					public PooledObject<TcpSessionHandler, 100, 5>
{
public:
	PktHdlDecorator* getDecorator()
	{
		return &m_decorator;
	}

	bool handlePacket(Packet* pPkt);

	WORD32 getSessionId() const
	{
		return m_dwSessionId;
	}

	void setSumStat(TcpStatData* pSumData)
	{
		m_pSumData = pSumData;
		setFlowStats();
	}

	const TcpFlowCtx& getDlFlow(){
		return m_downlinkFlow;
	}

	const TcpFlowCtx& getUlFlow(){
		return m_uplinkFlow;
	}

	void reportTotalStat();

	/*implements DataCollector*/
	const Data& collectData(WORD64 startTime, WORD64 endTime);
	void clearData();

private:
	TcpSessionHandler(WORD32 id)
		:m_dwSessionId(id),
		 m_pSumData(NULL),
		 m_decorator(*this, *this)
	{
		m_periodData.dwSessionId = id;
		setFlowStats();
	}

	~TcpSessionHandler()
	{
	}

	void handleTcpPktInFlow(TcpFlowCtx* fwd, TcpFlowCtx* rev,
			TcpHeader* hdr, Packet* pkt);

	void setFlowStats();
	void reportRTT();

	bool isUplinkFlow(TcpFlowCtx* flow)
	{
		return flow == &m_uplinkFlow;
	}
	bool isDownlinkFlow(TcpFlowCtx* flow)
	{
		return flow == &m_downlinkFlow;
	}

	WORD32 m_dwSessionId;
	PacketQuintuple m_quntp;

	TcpFlowCtx m_uplinkFlow;
	TcpFlowCtx m_downlinkFlow;

	TcpStatData m_periodData;
	TcpStatData* m_pSumData;

	PacketTime ts_mru_syn;
	PacketTime ts_mru_synack;
	PacketTime ts_first_rtt;

	PktHdlDecorator m_decorator;
	friend class TcpSesHdlMgr;
	TcpSesHdlMgr* m_pMgr;
};

class TcpSesHdlMgr
{
public:
	TcpSessionHandler* createHandler(const PacketQuintuple& qntp)
	{
		TcpSessionHandler* hdl = new TcpSessionHandler(generateId());
		if(NULL == hdl){
			return NULL;
		}
		hdl->m_pMgr = this;
		hdl->m_quntp = qntp;
		m_HandlerMap[qntp] = hdl;
		PacketQuintuple oppsite;
		qntp.getOppsiteQuntp(oppsite);
		m_OppositeMap[oppsite] = hdl;
		return hdl;
	}
	TcpSessionHandler* findHandler(const PacketQuintuple& qntp)
	{
		HANDLER_MAP::iterator it = m_HandlerMap.find(qntp);
		if(it != m_HandlerMap.end()){
			return it->second;
		}

		it = m_OppositeMap.find(qntp);
		if(it != m_OppositeMap.end()){
			return it->second;
		}
		return NULL;
	}
	void deleteHandler(const PacketQuintuple& qntp)
	{
		PacketQuintuple oppsite;
		qntp.getOppsiteQuntp(oppsite);

		TcpSessionHandler* hdl = NULL;
		HANDLER_MAP::iterator it;

		it = m_HandlerMap.find(qntp);
		if(it != m_HandlerMap.end()){
			hdl = it->second;
			m_HandlerMap.erase(it);
			it = m_OppositeMap.find(oppsite);
			if(it != m_OppositeMap.end()){
				m_OppositeMap.erase(it);
			}
			delete hdl;
			return;
		}

		it = m_HandlerMap.find(oppsite);
		if(it != m_HandlerMap.end()){
			hdl = it->second;
			m_HandlerMap.erase(it);
			it = m_OppositeMap.find(qntp);
			if(it != m_OppositeMap.end()){
				m_OppositeMap.erase(it);
			}
			delete hdl;
			return;
		}
	}

	void clear()
	{
		HANDLER_MAP::iterator it = m_HandlerMap.begin();
		for(; it != m_HandlerMap.end(); it++){
			delete it->second;
		}
		m_HandlerMap.clear();
		m_OppositeMap.clear();
	}

	void reportTotalStat()
	{
		HANDLER_MAP::iterator it = m_HandlerMap.begin();
		for(; it != m_HandlerMap.end(); it++){
			it->second->reportTotalStat();
		}
	}

	void getDlSummaryInfo(TcpSessionInfo::FlowInfo& info){
		info.reset();
		HANDLER_MAP::iterator it = m_HandlerMap.begin();
		for(; it != m_HandlerMap.end(); it++){
			info.add(it->second->getDlFlow());
		}
	}

private:
	typedef std::map<PacketQuintuple, TcpSessionHandler*> HANDLER_MAP;
	HANDLER_MAP m_HandlerMap;
	HANDLER_MAP m_OppositeMap;

	static WORD32 generateId()
	{
		static WORD32 idGenerator = 0;
		return ++idGenerator;
	}
};


#endif /* TCPFLOW_H_ */
