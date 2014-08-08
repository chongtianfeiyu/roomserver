/*
 * global.h
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "def/server_namespace.h"
#include "def/dal_def.h"
#include "common/common_typedef.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


#define LOGPREFIX			"log/"SERVER_NAME_STRING

//int32_t GetSerNumberSequence();

extern uint8_t g_nRobotTimeoutTime;
extern bool g_bTest;
extern bool g_bHasCommnad;
extern uint8_t g_messageHead[4096];
extern uint8_t g_messageBody[65535*10];

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* GLOBAL_H_ */
