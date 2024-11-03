/*
 * PacketHeaderStructs.h
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */

#ifndef PACKETHEADERSTRUCTS_H_
#define PACKETHEADERSTRUCTS_H_
#include "commdef.h"
#include <sys/endian.h>

#define IP_VERSION_V4 4
#define IP_VERSION_V6 6

#define TCP_PROTOCOL 6
#define UDP_PROTOCOL 17

typedef struct
{
#if _BYTE_ORDER == _LITTLE_ENDIAN
	BITS btHeadLen : 4;
	BITS btVersion : 4;
#else
	BITS btVersion : 4;
	BITS btHeadLen : 4;
#endif
	BYTE bTos;
	WORD16 wPktLen;
	WORD16 wId;
	WORD16 wOffset;
	BYTE bTTL;
	BYTE bPro;
	WORD16 wChkSum;
	WORD32 dwSrcIp;
	WORD32 dwDstIp;
}T_IPV4Head;

#define IPV6_LEN 16
typedef BYTE T_IPV6[IPV6_LEN];

typedef struct
{
#if _BYTE_ORDER == _LITTLE_ENDIAN
	WORD32 dwFlowLabel : 20;
	WORD32 bTC : 8;
	WORD32 bVersion : 4;
#else
	WORD32 bVersion : 4;
	WORD32 bTC : 8;
	WORD32 dwFlowLabel : 20;
#endif
	WORD16 wPayloadLen;
	BYTE bNextHeader;
	BYTE bHopLimit;

	T_IPV6 tSrcIp;
	T_IPV6 tDstIp;
}T_IPV6Head;

typedef struct
{
	WORD16 wSrcPort;
	WORD16 wDstPort;
	WORD16 wLen;
	WORD16 wChkSum;
}T_UDPHead;


/**** TCP protocol ************/
/*
 *  TCP option
 */
#define TCPOPT_NOP              1       /* Padding */
#define TCPOPT_EOL              0       /* End of options */
#define TCPOPT_MSS              2       /* Segment size negotiating */
#define TCPOPT_WINDOW           3       /* Window scaling */
#define TCPOPT_SACK_PERM        4       /* SACK Permitted */
#define TCPOPT_SACK             5       /* SACK Block */
#define TCPOPT_ECHO             6
#define TCPOPT_ECHOREPLY        7
#define TCPOPT_TIMESTAMP        8       /* Better RTT estimations/PAWS */
#define TCPOPT_CC               11
#define TCPOPT_CCNEW            12
#define TCPOPT_CCECHO           13
#define TCPOPT_MD5              19      /* RFC2385 */
#define TCPOPT_MPTCP            0x1e    /* Multipath TCP */
#define TCPOPT_SCPS             20      /* SCPS Capabilities */
#define TCPOPT_SNACK            21      /* SCPS SNACK */
#define TCPOPT_RECBOUND         22      /* SCPS Record Boundary */
#define TCPOPT_CORREXP          23      /* SCPS Corruption Experienced */
#define TCPOPT_QS               27      /* RFC4782 */
#define TCPOPT_USER_TO          28      /* RFC5482 */
#define TCPOPT_EXP_FD           0xfd    /* Experimental, reserved */
#define TCPOPT_EXP_FE           0xfe    /* Experimental, reserved */
/* Non IANA registered option numbers */
#define TCPOPT_RVBD_PROBE       76      /* Riverbed probe option */
#define TCPOPT_RVBD_TRPY        78      /* Riverbed transparency option */

/*
 *     TCP option lengths
 */
#define TCPOLEN_MSS            4
#define TCPOLEN_WINDOW         3
#define TCPOLEN_SACK_PERM      2
#define TCPOLEN_SACK_MIN       2
#define TCPOLEN_ECHO           6
#define TCPOLEN_ECHOREPLY      6
#define TCPOLEN_TIMESTAMP     10
#define TCPOLEN_CC             6
#define TCPOLEN_CCNEW          6
#define TCPOLEN_CCECHO         6
#define TCPOLEN_MD5           18
#define TCPOLEN_MPTCP_MIN      8
#define TCPOLEN_SCPS           4
#define TCPOLEN_SNACK          6
#define TCPOLEN_RECBOUND       2
#define TCPOLEN_CORREXP        2
#define TCPOLEN_QS             8
#define TCPOLEN_USER_TO        4
#define TCPOLEN_RVBD_PROBE_MIN 3
#define TCPOLEN_RVBD_TRPY_MIN 16
#define TCPOLEN_EXP_MIN        2

typedef struct
{
	WORD16 wSrcPort;
	WORD16 wDstPort;
	WORD32 dwSeqNum;
	WORD32 dwAckNum;
#if _BYTE_ORDER == _LITTLE_ENDIAN
	BITS bRsv1 : 4;
	BITS bHdrLen : 4;
	BITS bFin : 1;
	BITS bSyn : 1;
	BITS bRst : 1;
	BITS bPush : 1;
	BITS bAck : 1;
	BITS bUrg : 1;
	BITS bRsv2 : 2;
#else
	BITS bHdrLen : 4;
	BITS bRsv1 : 4;
	BITS bRsv2 : 2;
	BITS bUrg : 1;
	BITS bAck : 1;
	BITS bPush : 1;
	BITS bRst : 1;
	BITS bSyn : 1;
	BITS bFin : 1;
#endif
	WORD16 wWinSize;
	WORD16 wChkSum;
	WORD16 wUrgPoint;
}T_TCPHead;

typedef struct
{
	WORD32 dwLeftEdge;
	WORD32 dwRightEdge;
}T_TCPSAck;


/**** DNS protocol ************/
#define DNS_HEAD_LEN 12
#define DNS_COMPRESSION_MASK 0300
#define DNS_COMPRESSION_POINTER 0300

#define DNS_T_A          1 //type:ipv4 address
#define DNS_T_AAAA       28//type:ipv6 address
#define DNS_C_INTERNET   1//class:internet address

#define DNS_PORT  53

typedef struct
{
	WORD16 wId; //Query id

#if _BYTE_ORDER == _LITTLE_ENDIAN
	BITS bRCode : 4; //Response code
	BITS bRsv : 3; //Should be zero
	BITS bRA : 1; //Recursion available

	BITS bRD : 1; //Recursion desired?
	BITS bTC : 1; //Message was truncated?
	BITS bAA : 1; //Responding nameserver claims authority?
	BITS bOpCode : 4; //Kind of query
	BITS bQR : 1; //Is response?
#else
	BITS bQR : 1; //Is response?
	BITS bOpCode : 4; //Kind of query
	BITS bAA : 1; //Responding nameserver claims authority?
	BITS bTC : 1; //Message was truncated?
	BITS bRD : 1; //Recursion desired?

	BITS bRA : 1; //Recursion available
	BITS bRsv : 3; //Should be zero
	BITS bRCode : 4; //Response code
#endif

	WORD16 wQdCount; //Question section count
	WORD16 wAnCount; //Answer section count
	WORD16 wNsCount; //Authority section count
	WORD16 wArCount; //Additional section count

}T_DNSHead;

#endif /* PACKETHEADERSTRUCTS_H_ */
