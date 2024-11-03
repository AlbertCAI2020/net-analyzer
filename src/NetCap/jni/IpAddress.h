/*
 * IpAddress.h
 *
 *  Created on: 2015-11-14
 *      Author: Administrator
 */

#ifndef IPADDRESS_H_
#define IPADDRESS_H_
#include "protocoldefs.h"
#include <stdio.h>

class IpAddress
{
public:
	IpAddress()
	{
		memset(this, 0, sizeof(IpAddress));
	}

	IpAddress(WORD32 ipv4)
	{
		memset(this, 0, sizeof(IpAddress));
		version = IP_VERSION_V4;
		ip.ipv4 = ipv4;
	}

	IpAddress(BYTE ipv6[16])
	{
		memset(this, 0, sizeof(IpAddress));
		version = IP_VERSION_V6;
		memcpy(ip.ipv6, ipv6, 16);
	}

	void setIPv4(WORD32 ipv4)
	{
		memset(this, 0, sizeof(IpAddress));
		version = IP_VERSION_V4;
		ip.ipv4 = ipv4;
	}

	WORD32 getIPv4() const
	{
		return ip.ipv4;
	}

	void getIPv6(BYTE ipv6[16]) const
	{
		memcpy(ipv6, ip.ipv6, 16);
	}

	void setIPv6(BYTE ipv6[16])
	{
		memset(this, 0, sizeof(IpAddress));
		version = IP_VERSION_V6;
		memcpy(ip.ipv6, ipv6, 16);
	}

	int getVersion()
	{
		return version;
	}

	bool getString(char* buff, size_t len) const
	{
		if(IP_VERSION_V4 == version){
			return getIpv4String(buff, len);
		}else if(IP_VERSION_V6 == version){
			return getIpv6String(buff, len);
		}
		return false;
	}

	bool operator<(const IpAddress& other) const
	{
		return memcmp(this, &other, sizeof(IpAddress)) < 0;
	}

	bool operator>(const IpAddress& other) const
	{
		return memcmp(this, &other, sizeof(IpAddress)) > 0;
	}

	bool operator==(const IpAddress& other) const
	{
		return memcmp(this, &other, sizeof(IpAddress)) == 0;
	}

	bool operator!=(const IpAddress& other) const
	{
		return memcmp(this, &other, sizeof(IpAddress)) != 0;
	}

private:

	bool getIpv4String(char* buff, size_t len) const
	{
		if(NULL == buff || len < 16){
			return false;
		}
		unsigned char aIp[4] = {0};
		*(WORD32*)aIp = ip.ipv4;
		int ip1 = aIp[3];
		int ip2 = aIp[2];
		int ip3 = aIp[1];
		int ip4 = aIp[0];
		sprintf(buff, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
		return true;
	}

	bool getIpv6String(char* buff, size_t len) const
	{
		return false;
	}

	int version;
	union
	{
		WORD32 ipv4;
		BYTE ipv6[16];
	}ip;

};


#endif /* IPADDRESS_H_ */
