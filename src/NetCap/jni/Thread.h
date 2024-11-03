/*
 * Thread.h
 *
 *  Created on: 2015-11-11
 *      Author: Administrator
 */

#ifndef THREAD_H_
#define THREAD_H_
#include <pthread.h>
#include "UnCopyable.h"

class Task
{
public:
	virtual ~Task(){}
	virtual void run() = 0;
};

class Thread : UnCopyable
{
public:
	Thread(Task& task)
		:m_tid(-1), m_task(task)
	{
	}

	bool start()
	{
		if(-1 != m_tid)
		{
			return false;
		}

		if(0 != pthread_create(&m_tid, NULL, posix_thread_entry, (void*)this))
		{
			return false;
		}

		return true;
	}

	void join()
	{
		if(-1 != m_tid)
		{
			pthread_join(m_tid, NULL);
		}
		m_tid = -1;
	}

private:

	static void* posix_thread_entry(void* arg)
	{
		Thread* t = (Thread*)arg;
		t->m_task.run();
		return NULL;
	}

	pthread_t m_tid;
	Task& m_task;
};

class Lock : UnCopyable
{
public:
	virtual ~Lock(){}
	virtual void lock() = 0;
	virtual bool trylock() = 0;
	virtual void unlock() = 0;
};

class ScopedLock
{
public:
	ScopedLock(Lock& lock)
		:m_lock(lock)
	{
		m_lock.lock();
	}
	~ScopedLock()
	{
		m_lock.unlock();
	}

private:
	Lock& m_lock;
};

class MutexLock : public Lock
{
public:
	MutexLock()
	{
		pthread_mutex_init(&m_mutex, NULL);
	}
	~MutexLock()
	{
		pthread_mutex_destroy(&m_mutex);
	}
	void lock()
	{
		pthread_mutex_lock(&m_mutex);
	}
	bool trylock()
	{
		return 0 == pthread_mutex_trylock(&m_mutex);
	}
	void unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}

private:
	friend class Condition;
	pthread_mutex_t m_mutex;
};

class Condition : UnCopyable
{
public:
	Condition(MutexLock& mutex)
		:m_pMutex(&mutex.m_mutex)
	{
		pthread_cond_init(&m_cond, NULL);
	}

	void wait()
	{
		pthread_cond_wait(&m_cond, m_pMutex);
	}

	bool wait(int milis)
	{
		timeval begin = {0};
		timespec end = {0};
		gettimeofday(&begin, NULL);
		end.tv_sec = begin.tv_sec + milis/1000;
		end.tv_nsec = begin.tv_usec*1000 + (milis%1000)*1000*1000;
		int ret = pthread_cond_timedwait(&m_cond,m_pMutex, &end);
		return 0 == ret;
	}

	void notify()
	{
		pthread_cond_signal(&m_cond);
	}

	void notifyAll()
	{
		pthread_cond_broadcast(&m_cond);
	}

private:
	pthread_mutex_t* m_pMutex;
	pthread_cond_t m_cond;
};

#endif /* THREAD_H_ */
