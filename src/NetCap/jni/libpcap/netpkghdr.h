//netpkghdr.h
#ifndef _NET_PKG_HDR_H_
#define _NET_PKG_HDR_H_



#pragma pack(push, 1)

// ethernet header
typedef struct ether_header
{
    unsigned char  dst[6];         // 目标MAC
    unsigned char  src[6];         // 源MAC
    unsigned short type;           // 上层协议标识
} eth_hdr;

// ipv4 address
typedef struct ip_address
{
    unsigned char b1, b2, b3, b4;
} ip_addr;

// ipv4 header
typedef struct ip_header
{
    unsigned char  ver_ihl;        // 版本信息(4)头长度(4)
    unsigned char  tos;            // 服务类型
    unsigned short len;            // 数据包长度
    unsigned short id;             // 数据包标识
    unsigned short slice;          // 片偏移
    unsigned char  ttl;            // ttl
    unsigned char  proto;          // 协议
    unsigned short sum;            // 校验和
    ip_addr        saddr;          // 源IP
    ip_addr        daddr;          // 目标IP
} ip_hdr;

// tcp header
typedef struct tcp_header
{
    unsigned short sport;          // 源端口
    unsigned short dport;          // 目标端口
    unsigned int   seq;            // 序列号
    unsigned int   ack;            // 应答
    unsigned short len_code;       // TCP头长度(4)保留(6)标志(6)
    unsigned short window;         // 窗口大小
    unsigned short sum;            // 校验和
    unsigned short urp;            // 紧急数据偏移
} tcp_hdr;

// udp header
typedef struct udp_header
{
    unsigned short sport;          // 源端口
    unsigned short dport;          // 目标端口
    unsigned short len;            // 包长
    unsigned short sum;            // 校验和
} udp_hdr;

#pragma pack(pop)


#endif /* _NET_PKG_HDR_H_ */
