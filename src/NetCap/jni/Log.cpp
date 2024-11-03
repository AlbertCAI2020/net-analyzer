/*
 * Log.cpp
 *
 *  Created on: 2015-11-6
 *      Author: Administrator
 */
#include <stdio.h>
#include <stdarg.h>
#include "Log.h"
#include "Thread.h"
#include "ObjectPool.h"
#include "MessageQueue.h"
#include <time.h>

class LogMsg : public ThreadSafePooledObject<LogMsg, 100>
{
public:
	LogMsg()
	{
		memset(szMsg, 0, sizeof(szMsg));
		time = ::time(NULL);
	}

	char* getMsgBuff()
	{
		return szMsg;
	}

	const char* getMsg()
	{
		return szMsg;
	}

	void setTime(time_t t)
	{
		time = t;
	}

	time_t getTime()
	{
		return time;
	}

	static const int BUFF_LEN = 255;
private:
	char szMsg[BUFF_LEN+1];
	time_t time;
};

class Logger
{
public:
	static Logger& getLogger()
	{
		static Logger inst;
		return inst;
	}

	bool init(const char* szFile, bool printScreen = true)
	{
		if(prepared)
		{
			return false;
		}
		FILE* pf = fopen(szFile, "wt");
		if(NULL == pf)
		{
			return false;
		}
		if(!thread.start())
		{
			fclose(pf);
			return false;
		}
		file = pf;
		prepared = true;
		this->printScreen = printScreen;
		return true;
	}

	void unInit()
	{
		if(!prepared)
		{
			return;
		}

		while(!msgQueue.enqueue(NULL))
		{
		}
		thread.join();
		msgQueue.reset();
		fclose(file);
		prepared = false;
	}

	bool write(LogMsg* msg)
	{
		if(!prepared)
		{
			return false;
		}
		if(!msgQueue.enqueue(msg))
		{
			return false;
		}
		return true;
	}


private:
	Logger()
		:task(*this), thread(task),
		 file(NULL),prepared(false)
	{
	}

	~Logger()
	{
	}

	void writeQueuedLog()
	{
		LogMsg* msg = NULL;
		time_t t = 0;
		struct tm tt = {0};
		while(true)
		{
			msg = msgQueue.dequeue();
			if(NULL == msg)
			{
				break;
			}
			t = msg->getTime();
			localtime_r(&t, &tt);

			if(printScreen)
			{
				printf("[%02d/%02d/%02d %02d:%02d:%02d]",
						tt.tm_year+1900, tt.tm_mon+1, tt.tm_mday,
						tt.tm_hour, tt.tm_min, tt.tm_sec);
				printf("%s\r\n", msg->getMsg());
			}
			fprintf(file, "[%02d/%02d/%02d %02d:%02d:%02d]",
						tt.tm_year+1900, tt.tm_mon+1, tt.tm_mday,
						tt.tm_hour, tt.tm_min, tt.tm_sec);
			fprintf(file, "%s\r\n", msg->getMsg());
			fflush(file);

			delete msg;

		}
	}

	class LogTask : public Task
	{
	public:
		LogTask(Logger& l)
			:logger(l)
		{
		}

		void run()
		{
			logger.writeQueuedLog();
		}

	private:
		Logger& logger;
	};

	LogTask task;
	Thread thread;
	BlockableMsgQueue<LogMsg> msgQueue;
	FILE* file;
	volatile bool prepared;
	bool printScreen;
};

void log_init(const char* szFile)
{
	Logger::getLogger().init(szFile);
}

void log_unInit()
{
	Logger::getLogger().unInit();
}

void log_write(const char* format, ...)
{
	LogMsg* msg = new LogMsg;
	va_list v;
	va_start(v, format);
	vsnprintf(msg->getMsgBuff(), LogMsg::BUFF_LEN, format, v);
	va_end(v);
	if(!Logger::getLogger().write(msg))
	{
		delete msg;
	}
}
