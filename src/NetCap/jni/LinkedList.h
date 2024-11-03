/*
 * LinkedItem.h
 *
 *  Created on: 2015-11-9
 *      Author: Administrator
 */

#ifndef LINKEDITEM_H_
#define LINKEDITEM_H_
#include "commdef.h"

template<typename T>
class LinkedItem
{
public:
	LinkedItem()
		:_prev(NULL),_next(NULL)
	{

	}

	T* getItem()
	{
		return &_item;
	}

	T* getItemBackward(int indexFromHere)
	{
		LinkedItem<T>* pTmp =  this;
		while(NULL != pTmp)
		{
			if(0 >= indexFromHere)
			{
				break;
			}
			pTmp = pTmp->next();
			indexFromHere--;
		}
		if(NULL != pTmp)
		{
			return pTmp->getItem();
		}
		return NULL;
	}

	T* getItemForward(int indexFromHere)
	{
		LinkedItem<T>* pTmp =  this;
		while(NULL != pTmp)
		{
			if(0 >= indexFromHere)
			{
				break;
			}
			pTmp = pTmp->prev();
			indexFromHere--;
		}
		if(NULL != pTmp)
		{
			return pTmp->getItem();
		}
		return NULL;
	}

	LinkedItem<T>* prev()
	{
		return _prev;
	}

	LinkedItem<T>* next()
	{
		return _next;
	}

	void linkAfter(LinkedItem<T>* node)
	{
		node->_next = _next;
		node->_prev = this;
		_next = node;
	}

	void linkBefore(LinkedItem<T>* node)
	{
		node->_prev =_prev;
		node->_next = this;
		_prev = node;
	}

	LinkedItem<T>* eraseThis()
	{
		if(_prev){
			_prev->_next = _next;
		}
		if(_next){
			_next->_prev = _prev;
		}
		return _next;
	}

private:
	T _item;
	LinkedItem<T>* _prev;
	LinkedItem<T>* _next;
};

template<typename T>
class LinkedList
{
public:
	LinkedList()
		:_length(0),_header(NULL),_tailer(NULL)
	{
	}

	int length()
	{
		return _length;
	}
	T* get(int index)
	{
		if(_header)
		{
			return _header->getItemBackward(index);
		}else
		{
			return NULL;
		}
	}

private:
	int _length;
	LinkedItem<T>* _header;
	LinkedItem<T>* _tailer;
};


#endif /* LINKEDITEM_H_ */
