#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "Log.h"
#include "NetCapServer.h"

extern int unit_test_entry(int argc, char* argv[]);

class MainClear
{
public:
	~MainClear()
	{
		LOG_UNINIT();
	}
};

int main(int argc, char* argv[])
{
	MainClear clearer;

	LOG_INIT("/sdcard/netcap.log");

	if(argc < 2)
	{
		LOG("param error!");
		return 1;
	}

	if(0 == strcmp("--test", argv[1]))
	{
		LOG("run unit tests...");
#ifdef _COMPILE_UNITTEST
		return unit_test_entry(argc, argv);
#endif
		return 0;
	}

	NetCapServer* server =  NetCapServer::getInstance();

	LOG("server initializing...");
	if(!server->init(argv[1]))
	{
		LOG("server initialize failed!");
		return 2;
	}
	LOG("server initialize success!");

	LOG("server running...");
	server->run();
	LOG("server stopped!");

	server->unInit();

	return 0;
}








