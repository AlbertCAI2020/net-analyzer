/*
 * messageheader.h
 *
 *  Created on: 2015-11-23
 *      Author: Administrator
 */

#ifndef MESSAGEHEADER_H_
#define MESSAGEHEADER_H_

#define HDRBUFF_LEN  8

 struct MsgHeader
{
	int length;
	int type;

	MsgHeader()
		:length(0),
		 type(0)
	{
	}
};



#endif /* MESSAGEHEADER_H_ */
