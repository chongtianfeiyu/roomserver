/*
 * global.cpp
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#include "global.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

//int32_t GetSerNumberSequence()
//{
//	static int32_t seq = 1;
//	if ( ++seq >= 10000)
//	{
//		seq = 1;
//	}
//	return seq;
//}
uint8_t g_nRobotTimeoutTime = 0;
bool g_bTest = false;
bool g_bHasCommnad = false;
uint8_t g_messageHead[4096];
uint8_t g_messageBody[65535*10];

FRAME_ROOMSERVER_NAMESPACE_END

