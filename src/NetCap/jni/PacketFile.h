/*
 * PacketFile.h
 *
 *  Created on: 2015-12-4
 *      Author: Administrator
 */

#ifndef PACKETFILE_H_
#define PACKETFILE_H_
#include "Packet.h"
#include <stdio.h>

#define FILE_OPEN_WRITE 1
#define FILE_OPEN_READ 2

#define	PCAP_MAGIC	0xa1b2c3d4

/* "libpcap" file header. */
struct pcap_hdr {
	WORD32  magic; /*magic number*/
	WORD16	version_major;	/* major version number */
	WORD16	version_minor;	/* minor version number */
	WORD32	thiszone;	/* GMT to local correction */
	WORD32	sigfigs;	/* accuracy of timestamps */
	WORD32	snaplen;	/* max length of captured packets, in octets */
	WORD32	network;	/* data link type */
};

/* "libpcap" record header. */
struct pcaprec_hdr {
	WORD32	ts_sec;		/* timestamp seconds */
	WORD32	ts_usec;	/* timestamp microseconds (nsecs for PCAP_NSEC_MAGIC) */
	WORD32	incl_len;	/* number of octets of packet saved in file */
	WORD32	orig_len;	/* actual length of packet */
};

class PacketFile
{
public:
	PacketFile()
		:_pf(NULL)
	{
	}

	bool open(const char* file, int mode)
	{
		const char* m = NULL;
		if(FILE_OPEN_WRITE == mode){
			m = "w";
		}else if(FILE_OPEN_READ == mode){
			m = "r";
		}else{
			return false;
		}
		FILE* pf = fopen(file, m);
		if(NULL == pf){
			return false;
		}
		_pf = pf;

		if(FILE_OPEN_WRITE == mode &&
			!writeFileHeader()){
			this->close();
			return false;
		}
		return true;
	}

	void close()
	{
		if(NULL != _pf){
			fclose(_pf);
			_pf = NULL;
		}
	}

	bool write(Packet* pkt)
	{
		if(NULL == pkt || NULL == _pf){
			return false;
		}
		pcaprec_hdr hdr = {0};
		hdr.ts_sec = pkt->getTime().second();
		hdr.ts_usec = pkt->getTime().microSecond()%1000000;
		hdr.incl_len = pkt->getPktLength();
		hdr.orig_len = hdr.incl_len;

		if(1 != fwrite(&hdr, sizeof(hdr), 1, _pf)){
			return false;
		}

		size_t count = pkt->getBufferCount();
		for(size_t i = 0; i < count; i++){
			PacketBuffer* buffer = pkt->getBuffer(i);
			if(1 != fwrite(buffer->getBuffer(),
				buffer->getLength(), 1, _pf)){
				return false;
			}
		}

		fflush(_pf);

		return true;
	}

private:

	bool writeFileHeader()
	{
		pcap_hdr hdr = {0};
		hdr.magic = PCAP_MAGIC;
		hdr.version_major = 2;
		hdr.version_minor = 4;
		hdr.snaplen = 65535;
		hdr.network = 101;
		if(1 != fwrite(&hdr, sizeof(hdr), 1, _pf)){
			return false;
		}
		return true;
	}
	FILE* _pf;
};



#endif /* PACKETFILE_H_ */
