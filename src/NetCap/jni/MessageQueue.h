/*
 * PacketQueue.h
 *
 *  Created on: 2015-11-11
 *      Author: Administrator
 */

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_
#include <deque>
#include "Thread.h"

template<typename T>
class BlockableMsgQueue
{
public:
	BlockableMsgQueue(size_t maxSize = 1000)
		:m_bQuit(false),
		 m_maxSize(maxSize),
		 m_failedCount(0),
		 m_cleanFunc(NULL),
		 m_cond(m_mutex)
	{
		if(0 == maxSize)
		{
			m_maxSize = 1000;
		}
	}

	//if msg is NULL, it means a quit message,
	//the thread blocking on the queue should break its loop when obtaining a NULL message.
	bool enqueue(T* msg)
	{
		ScopedLock lock(m_mutex);
		if(m_bQuit)
		{
			return false;
		}
		if(m_queue.size() > m_maxSize)
		{
			m_failedCount++;
			return false;
		}
		if(NULL == msg)
		{
			m_bQuit = true;
		}
		if(m_queue.empty())
		{
			m_queue.push_back(msg);
			m_cond.notify();
			return true;
		}
		m_queue.push_back(msg);
		return true;
	}

	//the thread blocking on the queue should break its loop when obtaining a NULL message.
	T* dequeue()
	{
		ScopedLock lock(m_mutex);
		while(m_queue.empty())
		{
			m_cond.wait();
		}
		T* msg = m_queue.front();
		m_queue.pop_front();
		return msg;
	}

	size_t size()
	{
		ScopedLock lock(m_mutex);
		return m_queue.size();
	}

	bool empty()
	{
		ScopedLock lock(m_mutex);
		return m_queue.empty();
	}

	typedef void (*CleanCallback)(T* msg);
	void setCleanCallback(CleanCallback pFunc)
	{
		m_cleanFunc = pFunc;
	}

	void reset()
	{
		ScopedLock lock(m_mutex);
		if(NULL != m_cleanFunc)
		{
			for(size_t i = 0; i < m_queue.size(); i++)
			{
				T* msg = m_queue[i];
				if(NULL!= msg)
				{
					m_cleanFunc(msg);
				}
			}
		}

		m_queue.clear();
		m_bQuit = false;
		m_failedCount = 0;
	}

	size_t failedCount()
	{
		return m_failedCount;
	}

private:
	std::deque<T*> m_queue;
	bool m_bQuit;
	size_t m_maxSize;
	size_t m_failedCount;
	CleanCallback m_cleanFunc;
	MutexLock m_mutex;
	Condition m_cond;
};


#endif /* PACKETQUEUE_H_ */
