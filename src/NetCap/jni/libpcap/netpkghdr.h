//netpkghdr.h
#ifndef _NET_PKG_HDR_H_
#define _NET_PKG_HDR_H_



#pragma pack(push, 1)

// ethernet header
typedef struct ether_header
{
    unsigned char  dst[6];         // Ŀ��MAC
    unsigned char  src[6];         // ԴMAC
    unsigned short type;           // �ϲ�Э���ʶ
} eth_hdr;

// ipv4 address
typedef struct ip_address
{
    unsigned char b1, b2, b3, b4;
} ip_addr;

// ipv4 header
typedef struct ip_header
{
    unsigned char  ver_ihl;        // �汾��Ϣ(4)ͷ����(4)
    unsigned char  tos;            // ��������
    unsigned short len;            // ���ݰ�����
    unsigned short id;             // ���ݰ���ʶ
    unsigned short slice;          // Ƭƫ��
    unsigned char  ttl;            // ttl
    unsigned char  proto;          // Э��
    unsigned short sum;            // У���
    ip_addr        saddr;          // ԴIP
    ip_addr        daddr;          // Ŀ��IP
} ip_hdr;

// tcp header
typedef struct tcp_header
{
    unsigned short sport;          // Դ�˿�
    unsigned short dport;          // Ŀ��˿�
    unsigned int   seq;            // ���к�
    unsigned int   ack;            // Ӧ��
    unsigned short len_code;       // TCPͷ����(4)����(6)��־(6)
    unsigned short window;         // ���ڴ�С
    unsigned short sum;            // У���
    unsigned short urp;            // ��������ƫ��
} tcp_hdr;

// udp header
typedef struct udp_header
{
    unsigned short sport;          // Դ�˿�
    unsigned short dport;          // Ŀ��˿�
    unsigned short len;            // ����
    unsigned short sum;            // У���
} udp_hdr;

#pragma pack(pop)


#endif /* _NET_PKG_HDR_H_ */
