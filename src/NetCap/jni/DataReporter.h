/*
 * DataReporter.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

#ifndef DATAREPORTER_H_
#define DATAREPORTER_H_
#include "Data.h"

class DataReporter
{
public:
	virtual ~DataReporter(){}
	virtual void reportData(const Data& data) = 0;
};


#endif /* DATAREPORTER_H_ */
