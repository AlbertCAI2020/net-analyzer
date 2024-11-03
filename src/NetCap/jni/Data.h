/*
 * Data.h
 *
 *  Created on: 2015-11-19
 *      Author: Administrator
 */

#ifndef DATA_H_
#define DATA_H_
#include "commdef.h"

class Data
{
public:

	Data(){}
	virtual ~Data(){}

	virtual bool serializeToBuffer(BYTE* buff, size_t len) const = 0;

	virtual size_t serializedBytes() const = 0;

	virtual const char* toString() const
	{
		return "";
	}
};

#endif /* DATA_H_ */
