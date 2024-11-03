/*
 * PacketHeader.cpp
 *
 *  Created on: 2015-11-25
 *      Author: Administrator
 */
#include "PacketHeader.h"
#include "Packet.h"

bool IpHeader::parseFromPkt(Packet* pPkt)
{
	PacketBuffer* pBuffer = pPkt->getBuffer(0);
	if(NULL == pBuffer)
	{
		return false;
	}
	BYTE* pBuff = pBuffer->getBuffer();
	WORD16 wBuffLen = pBuffer->getLength();

	BYTE bVer = (*pBuff)>>4;
	if(IP_VERSION_V4 != bVer)
	{
		return false;
	}

	if(wBuffLen < sizeof(T_IPV4Head))
	{
		return false;
	}

	T_IPV4Head* pHead = this->getStruct();
	*pHead = *(T_IPV4Head*)pBuff;

	BYTE bHdrLen = (pHead->btHeadLen)<<2;
	if(bHdrLen < sizeof(T_IPV4Head) || wBuffLen < bHdrLen)
	{
		return false;
	}
	m_len = bHdrLen;

	pHead->wPktLen = ntohs(pHead->wPktLen);
	pHead->wId = ntohs(pHead->wId);
	pHead->wOffset = ntohs(pHead->wOffset);
	pHead->wChkSum = ntohs(pHead->wChkSum);
	pHead->dwSrcIp = ntohl(pHead->dwSrcIp);
	pHead->dwDstIp = ntohl(pHead->dwDstIp);

	return true;
}

bool TcpHeader::parseFromPkt(Packet* pPkt)
{
	PacketBuffer* pBuffer = pPkt->getBuffer(0);
	if(NULL == pBuffer)
	{
		return false;
	}
	BYTE* pBuff = pBuffer->getBuffer();
	WORD16 wBuffLen = pBuffer->getLength();
	if(wBuffLen < sizeof(T_TCPHead))
	{
		return false;
	}

	T_TCPHead* pTcpStruct = this->getStruct();
	*pTcpStruct = *(T_TCPHead*)pBuff;
	pTcpStruct->wSrcPort = ntohs(pTcpStruct->wSrcPort);
	pTcpStruct->wDstPort = ntohs(pTcpStruct->wDstPort);
	pTcpStruct->dwSeqNum = ntohl(pTcpStruct->dwSeqNum);
	pTcpStruct->dwAckNum = ntohl(pTcpStruct->dwAckNum);
	pTcpStruct->wWinSize = ntohs(pTcpStruct->wWinSize);
	pTcpStruct->wChkSum = ntohs(pTcpStruct->wChkSum);
	pTcpStruct->wUrgPoint = ntohs(pTcpStruct->wUrgPoint);

	BYTE bHeadLen = (pTcpStruct->bHdrLen)<<2;
	if(bHeadLen < sizeof(T_TCPHead))
	{
		return false;
	}
	m_len = bHeadLen;

	/*½âÎöTCPÑ¡Ïî*/
	pBuff += sizeof(T_TCPHead);
	wBuffLen -= sizeof(T_TCPHead);
	BYTE bOptLen = bHeadLen - sizeof(T_TCPHead);
	if(wBuffLen < bOptLen)
	{
		return false;
	}
	if(bOptLen > 0)
	{
		BYTE bKind = 0;
		BYTE bLen = 0;
		while(bOptLen > 0)
		{
			bKind = *pBuff;
			if(TCPOPT_EOL == bKind)
			{
				break;
			}
			if(TCPOPT_NOP == bKind)
			{
				bOptLen--;
				pBuff++;
				continue;
			}
			if(bOptLen <= 2)
			{
				break;
			}
			pBuff++;
			bLen = *pBuff;
			if(0 == bLen || bOptLen < bLen)
			{
				break;
			}
			pBuff++;

			if(TCPOPT_MSS == bKind && TCPOLEN_MSS == bLen)
			{
				this->wMss = ntohs(*(WORD16*)pBuff);
				this->dwTcpOptFlag |= TCP_HASOPT_MSS;
				pBuff += 2;
			}
			else if(TCPOPT_WINDOW == bKind &&  TCPOLEN_WINDOW == bLen)
			{
				this->bWinScale = *pBuff;
				this->dwTcpOptFlag |= TCP_HASOPT_WINSCALE;
				pBuff += 1;
			}
			else if(TCPOPT_SACK_PERM == bKind && TCPOLEN_SACK_PERM == bLen)
			{
				this->dwTcpOptFlag |= TCP_HASOPT_SACKPERM;
			}
			else if(TCPOPT_SACK == bKind && TCPOLEN_SACK_MIN < bLen)
			{
				this->dwTcpOptFlag |= TCP_HASOPT_SACK;
				int sAckLen = bLen - 2;
				for(int i = 0; sAckLen > 0; sAckLen -= 8, i++)
				{
					this->aTcpSack[i].dwLeftEdge = ntohl(*(WORD32*)pBuff);
					pBuff += 4;
					this->aTcpSack[i].dwRightEdge = ntohl(*(WORD32*)pBuff);
					pBuff += 4;
					this->bSAckBlkNum++;
				}
			}
			else
			{
				pBuff += (bLen-2);
			}
			bOptLen -= bLen;

		}
	}

	return true;
}

bool UdpHeader::parseFromPkt(Packet* pPkt)
{
	PacketBuffer* pBuffer = pPkt->getBuffer(0);
	if(NULL == pBuffer)
	{
		return false;
	}
	BYTE* pBuff = pBuffer->getBuffer();
	WORD16 wBuffLen = pBuffer->getLength();
	if(wBuffLen < sizeof(T_UDPHead))
	{
		return false;
	}

	T_UDPHead* pUdpStruct = this->getStruct();
	*pUdpStruct = *(T_UDPHead*)pBuff;
	pUdpStruct->wSrcPort = ntohs(pUdpStruct->wSrcPort);
	pUdpStruct->wDstPort = ntohs(pUdpStruct->wDstPort);
	pUdpStruct->wLen = ntohs(pUdpStruct->wLen);
	pUdpStruct->wChkSum = ntohs(pUdpStruct->wChkSum);
	if(pUdpStruct->wLen > pPkt->getPktLength())
	{
		return false;
	}
	m_len = sizeof(T_UDPHead);

	return true;
}

