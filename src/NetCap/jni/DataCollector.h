/*
 * DataCollector.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

#ifndef DATACOLLECTOR_H_
#define DATACOLLECTOR_H_
#include "commdef.h"
#include "DataReporter.h"

//time-driven, collecting and reporting periodically

class DataCollector
{
public:
	DataCollector()
		:_reporter(NULL),
		 _interval(0),
		 _lastTime(0),
		 _beginTime(0)
	{
	}

	virtual ~DataCollector(){}

	void setReporter(DataReporter* reporter)
	{
		_reporter = reporter;
	}

	DataReporter* getReporter() const
	{
		return _reporter;
	}

	void setInterval(WORD32 interval)
	{
		_interval = interval;
	}

	WORD32 getInterval() const
	{
		return _interval;
	}

	void setBeginTime(WORD64 time)
	{
		_beginTime = time;
		_lastTime = time;
	}

	WORD64 getBeginTime() const
	{
		return _beginTime;
	}

	void timeDriven(WORD64 curTime)
	{
		if(0 == _interval ||
				_lastTime > curTime)
		{
			return;
		}

		if(0 == _beginTime)
		{
			setBeginTime(curTime);
			return;
		}

		WORD32 count = (curTime - _lastTime)/_interval;
		if(count < 1)
		{
			return;
		}

		WORD64 startTime = _lastTime;
		WORD64 endTime = _lastTime + _interval;
		_lastTime += count*_interval;

		const Data& data = collectData(startTime, endTime);
		if(NULL != _reporter)
		{
			_reporter->reportData(data);
		}
		clearData();
		return;
	}

	/*template methods --GOF*/

	virtual const Data& collectData(WORD64 startTime, WORD64 endTime) = 0;

	virtual void clearData() = 0;

protected:
	DataReporter* _reporter;
	WORD32 _interval;/*milliseconds*/
	WORD64 _lastTime;/*milliseconds*/
	WORD64 _beginTime;/*milliseconds*/
};


#endif /* DATACOLLECTOR_H_ */
