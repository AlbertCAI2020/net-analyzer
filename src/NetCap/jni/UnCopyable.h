/*
 * ObjectUtilities.h
 *
 *  Created on: 2015-11-12
 *      Author: Administrator
 */

#ifndef UNCOPYABLE_H_
#define UNCOPYABLE_H_

class UnCopyable
{
public:
	UnCopyable()
	{
	}

private:
	UnCopyable(const UnCopyable&)
	{
	}
	const UnCopyable& operator=(const UnCopyable& )
	{
		return *this;
	}
};

#endif /* OBJECTUTILITIES_H_ */
