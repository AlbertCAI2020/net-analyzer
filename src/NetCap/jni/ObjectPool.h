#ifndef _OBJECT_POOL__H
#define _OBJECT_POOL__H
#include <typeinfo>
#include <new>
#include "commdef.h"
#include "Log.h"
#include "Thread.h"

template<typename T>
class Placement
{
public:
	T* Get()
	{
		return (T*)data;
	}
private:
	char data[sizeof(T)];
};

template<typename T, 
	size_t BLOCK_SIZE = 1000, 
	size_t MAX_BLOCK_NUM = 0xFFFFFFFF>
class ObjectPool
{
public:
	static ObjectPool& Instance()
	{
		static ObjectPool inst;
		return inst;
	}

	ObjectPool()
	{
		m_pBlockLinkHeader = NULL;
		m_nBlockNum = 0;
		m_pFreeLinkHeader = NULL;
	}
	~ObjectPool()
	{
		Block* pTemp = m_pBlockLinkHeader;
		while (pTemp)
		{
			m_pBlockLinkHeader = pTemp->pNext;
			delete pTemp;
			pTemp = m_pBlockLinkHeader;
		}
		m_pBlockLinkHeader = NULL;
		m_nBlockNum = 0;
		m_pFreeLinkHeader = NULL;
	}

	T* New()
	{
		T* p = Alloc();
		if (p)
		{
			new(p) T();
		}
		return p;
	}

	void Delete(T* p)
	{
		if (p)
		{
			p->~T();
			Free(p);
		}		
	}

	T* Alloc()
	{
		Element* p = NULL;
		if (m_pFreeLinkHeader)
		{
			p = m_pFreeLinkHeader;
			m_pFreeLinkHeader = m_pFreeLinkHeader->pNext;
			return p->data.Get();
		}

		if (m_pBlockLinkHeader)
		{
			size_t& nUsed = m_pBlockLinkHeader->nUsed;
			if (nUsed < BLOCK_SIZE)
			{
				p = &m_pBlockLinkHeader->elementArray[nUsed];
				nUsed++;
			}
			else if(m_nBlockNum < MAX_BLOCK_NUM)
			{
				Block* pTmpBlock = new Block;
				if (pTmpBlock)
				{
					p = &pTmpBlock->elementArray[0];
					pTmpBlock->nUsed = 1;
					pTmpBlock->pNext = m_pBlockLinkHeader;
					m_pBlockLinkHeader = pTmpBlock;
					m_nBlockNum++;
				}				
			}			
		}
		else
		{
			Block* pTmpBlock = new Block;
			if (pTmpBlock)
			{
				p = &pTmpBlock->elementArray[0];
				pTmpBlock->nUsed = 1;
				pTmpBlock->pNext = m_pBlockLinkHeader;
				m_pBlockLinkHeader = pTmpBlock;
				m_nBlockNum++;
			}			
		}
		if(p)
		{
			return p->data.Get();
		}
		return NULL;
	}
	void Free(T* p)
	{
		if (!p)
		{
			return;
		}
		Element* pTemp = (Element*)p;
		pTemp->pNext  = m_pFreeLinkHeader;
		m_pFreeLinkHeader = pTemp;
	}

private:
	struct Element
	{
		Placement<T> data;
		Element* pNext;
	};
	struct Block
	{
		Element elementArray[BLOCK_SIZE];
		size_t nUsed;
		Block* pNext;
	};
	Block* m_pBlockLinkHeader;
	size_t m_nBlockNum;
	Element* m_pFreeLinkHeader;
};

template<typename T,
	size_t BLOCK_SIZE = 1000,
	size_t MAX_BLOCK_NUM = 0xFFFFFFFF>
class PooledObject
{
public:
	void* operator new(size_t size)
	{
		if(size > sizeof(T)) throw std::bad_alloc();
		return ObjectPool<T,BLOCK_SIZE,MAX_BLOCK_NUM>::Instance().Alloc();
	}

	void operator delete(void* p)
	{
		ObjectPool<T,BLOCK_SIZE,MAX_BLOCK_NUM>::Instance().Free((T*)p);
	}
};

template<typename T,
	size_t BLOCK_SIZE = 1000,
	size_t MAX_BLOCK_NUM = 0xFFFFFFFF>
class ThreadSafePooledObject
{
public:
	void* operator new(size_t size)
	{
		if(size > sizeof(T)) throw std::bad_alloc();
		ScopedLock lock(*getThreadSafeLock());
		return ObjectPool<T,BLOCK_SIZE,MAX_BLOCK_NUM>::Instance().Alloc();
	}

	void operator delete(void* p)
	{
		ScopedLock lock(*getThreadSafeLock());
		ObjectPool<T,BLOCK_SIZE,MAX_BLOCK_NUM>::Instance().Free((T*)p);
	}

private:
	static MutexLock lock;
	static Lock* getThreadSafeLock()
	{
		return &lock;
	}
};

template<typename T,
	size_t BLOCK_SIZE ,
	size_t MAX_BLOCK_NUM>
MutexLock ThreadSafePooledObject<T,BLOCK_SIZE,MAX_BLOCK_NUM>::lock;

#endif // !_OBJECT_POOL__H
