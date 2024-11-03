/*
 * NetCapServer.cpp
 *
 *  Created on: 2015-11-17
 *      Author: Administrator
 */
#include "NetCapServer.h"
#include "PacketCapture.h"
#include "DataCollectCenter.h"

NetCapServer* NetCapServer::getInstance()
{
	static NetCapServer inst;
	return &inst;
}

NetCapServer::NetCapServer()
	:m_reporter(m_writepipe)
{
}

NetCapServer::~NetCapServer() {
}

bool NetCapServer::init(const char* szPipeDir)
{
	if(!openSvrPipe(szPipeDir))
	{
		return false;
	}

	PacketCapture* capture = PacketCapture::getInstance();
	if(!capture->init())
	{
		LOG("packet capturer init failed!");
		return false;
	}
	LOG("packet capturer init success!");
	capture->setHandler(&m_analyzer);

	if(capture->hasIfcfgIpv4())
	{
		ProtocolStack* protoStack = m_analyzer.getProtoStack();
		protoStack->setIpv4Addr(capture->getIpv4Addr());
	}

	DataCollectCenter* dcc = DataCollectCenter::getInstance();
	dcc->registerReporter(&m_reporter);

	return true;
}

bool NetCapServer::openSvrPipe(const char* szPipeDir)
{
	if(NULL == szPipeDir)
	{
		return false;
	}
	char buff[512] = {0};
	strcpy(buff, szPipeDir);
	strcat(buff, "/");
	strcat(buff, PIPE_NAME_C2S);
	LOG("trying open read-pipe:%s", buff);

	//此处必须等待app创建管道，如果是root进程创建了管道，app权限不足无法打开
	while(0 != access(buff, F_OK))
	{
	}
	if(!m_readpipe.Open(buff, PIPE_READ))
	{
		LOG("read-pipe open error:%d, file:%s", m_readpipe.GetLastErr(), buff);
		return false;
	}
	LOG("read-pipe open success!");

	strcpy(buff, szPipeDir);
	strcat(buff, "/");
	strcat(buff, PIPE_NAME_S2C);
	LOG("trying open write-pipe:%s", buff);

	//此处必须等待app创建管道，如果是root进程创建了管道，app权限不足无法打开
	while(0 != access(buff, F_OK))
	{
	}
	if(!m_writepipe.Open(buff, PIPE_WRITE))
	{
		LOG("write-pipe open error:%d, file:%s", m_writepipe.GetLastErr(), buff);
		return false;
	}
	LOG("write-pipe open success!");
	return true;
}

void NetCapServer::unInit()
{
	PacketCapture* capture = PacketCapture::getInstance();
	capture->unInit();
	m_analyzer.stop();

	m_writepipe.Close();
	m_readpipe.Close();
}

void NetCapServer::run()
{
	int readed = 0;
	char* phdrbuf = NULL;
	char* pmsgbuf = NULL;

	MsgHeader hdr;
	char buff[512] = {0};

	while(true)
	{
		bool berr = false;
		phdrbuf = (char*)&hdr;
		pmsgbuf = buff;
		readed = 0;
		while(readed < HDRBUFF_LEN){
			if(0 >= m_readpipe.Read(phdrbuf++, 1)){
				berr = true;
				break;
			}
			readed++;
		}

		if(berr){
			LOG("pipe read error:%d!", m_readpipe.GetLastErr());
			break;
		}

		if(hdr.length > sizeof(buff)){
			LOG("error: msg too long!");
			break;
		}

		readed = 0;
		while(readed < hdr.length){
			if(0 >= m_readpipe.Read(pmsgbuf++, 1)){
				berr = true;
				break;
			}
			readed++;
		}
		if(berr){
			LOG("pipe read error:%d!", m_readpipe.GetLastErr());
			break;
		}

		handlePipeMsg(hdr, buff);
	}
}

void NetCapServer::handlePipeMsg(const MsgHeader& hdr, char buff[])
{
	switch(hdr.type)
	{
		case START_CAP_REQ:
		{
			handleStartCapReq(hdr, buff);
		}
		break;
		case STOP_CAP_REQ:
		{
			handleStopCapReq(hdr, buff);
		}
		break;
		default:
		break;
	}
}

void NetCapServer::handleStartCapReq(const MsgHeader& hdr, char buff[])
{
	StartCapReq req;
	bool bret = req.ParseFromArray(buff, hdr.length);
	if(!bret){
		LOG("StartCapReq parse error!");
		return;
	}
	LOG("StartCapReq received!");

	if(req.has_url()){
		LOG("url=%s!", req.url().c_str());
	}

	const char* saveFile = NULL;
	if(req.has_savefile()){
		saveFile = req.savefile().c_str();
		LOG("Save packets to file: %s", saveFile);
	}

	DataCollectCenter* dcc = DataCollectCenter::getInstance();
	if(req.has_tcpstatinterval()){
		WORD32 inter = req.tcpstatinterval();
		dcc->getCollector(TCP_STAT_DATA)->setInterval(inter);
		LOG("TCP data collecting interval: %u ms", inter);
	}
	bret = m_analyzer.start(saveFile);

	if(bret){
		LOG("PacketAnalyzer start success!");
	}else{
		LOG("PacketAnalyzer start failed!");
	}

	WORD64 startTime = (WORD64)m_analyzer.getStartTime()*1000;

	StartCapRes res;
	res.set_ack(bret?0:1);
	res.set_seq(req.seq());
	res.set_time(startTime);

	if(!sendPipeMsg(START_CAP_RES, res))
	{
		LOG("StartCapRes send error!");
	}
	else
	{
		LOG("StartCapRes send success!");
	}
}

void NetCapServer::handleStopCapReq(const MsgHeader& hdr, char buff[])
{
	StopCapReq req;
	bool bret = req.ParseFromArray(buff, hdr.length);
	if(!bret){
		LOG("StopCapReq parse error!");
		return;
	}
	LOG("StopCapReq received!");

	m_analyzer.stop();
	WORD64 stopTime = (WORD64)m_analyzer.getEndTime()*1000;

	StopCapRes res;
	res.set_seq(req.seq());
	res.set_time(stopTime);
	res.set_totalsecs(m_analyzer.getTotalSecs());
	res.set_totalpktnum(m_analyzer.getTotalPktNum());
	res.set_droppedpktnum(m_analyzer.getDropedPktNum());

	if(!sendPipeMsg(STOP_CAP_RES, res))
	{
		LOG("StopCapRes send error!");
	}
	else
	{
		LOG("StopCapRes send success!");
	}
}


bool NetCapServer::sendPipeMsg(int type, const ::google::protobuf::MessageLite& msg)
{
	char buff[512];
	MsgHeader* hdr = (MsgHeader*)buff;
	hdr->length = msg.ByteSize();
	hdr->type = type;

	char* pbuff = buff;
	pbuff += sizeof(MsgHeader);

	if(!msg.SerializeToArray(pbuff, sizeof(buff)-sizeof(MsgHeader)))
	{
		LOG("message serialize error!");
		return false;
	}

	int count = m_writepipe.Write(buff, sizeof(MsgHeader)+msg.ByteSize());

	if(count != sizeof(MsgHeader) + msg.ByteSize())
	{
		LOG("message send error!");
		return false;
	}
	return true;
}



