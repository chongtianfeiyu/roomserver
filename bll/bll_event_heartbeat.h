/*
 *  bll_event_heartbeat.h
 *
 *  To do：
 *  Created on: 	2012-5-10
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_HEARTBEAT_H_
#define BLL_EVENT_HEARTBEAT_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CHeartBeatEvent:public CBllBase
{
public:
	CHeartBeatEvent()
	{
	}

	virtual ~CHeartBeatEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
};

#define	CREATE_HEARTBEATEVENT_INSTANCE		CSingleton<CHeartBeatEvent>::CreateInstance
#define	GET_HEARTBEATEVENT_INSTANCE			CSingleton<CHeartBeatEvent>::GetInstance
#define	DESTROY_HEARTBEATEVENT_INSTANCE		CSingleton<CHeartBeatEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_HEARTBEAT_H_ */
