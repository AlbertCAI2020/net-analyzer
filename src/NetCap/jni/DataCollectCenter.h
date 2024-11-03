/*
 * DataCollectCenter.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

#ifndef DATACOLLECTCENTER_H_
#define DATACOLLECTCENTER_H_
#include "DataCollector.h"
#include "DataReporter.h"
#include "DataType.h"
#include <map>

class DataCollectCenter
{
public:
	static DataCollectCenter* getInstance()
	{
		static DataCollectCenter inst;
		return &inst;
	}

	void registerCollector(DataType type , DataCollector* pCollector)
	{
		if(NULL == pCollector)
		{
			return;
		}
		_collectors[type] = pCollector;
		pCollector->setReporter(_reporter);
	}

	void unregisterCollector(DataType type)
	{
		_collectors.erase(type);
	}

	DataCollector* getCollector(DataType type)
	{
		std::map<DataType, DataCollector*>::iterator it
			= _collectors.find(type);
		if(it != _collectors.end())
		{
			return it->second;
		}
		return NULL;
	}

	void registerReporter(DataReporter* pReporter)
	{
		_reporter = pReporter;

		std::map<DataType, DataCollector*>::iterator it
					= _collectors.begin();
		for(; it != _collectors.end(); it++)
		{
			it->second->setReporter(_reporter);
		}
	}

	DataReporter* getReporter()
	{
		return _reporter;
	}


private:
	DataCollectCenter()
		:_reporter(NULL)
	{
	}

	~DataCollectCenter(){}

	std::map<DataType, DataCollector*> _collectors;
	DataReporter* _reporter;
};



#endif /* DATACOLLECTCENTER_H_ */
