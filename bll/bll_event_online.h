/*
 * bll_event_online.h
 *
 *  Created on: 2012-1-17
 *      Author: jimm
 */

#ifndef BLL_EVENT_ONLINE_H_
#define BLL_EVENT_ONLINE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class COnlineMessageEvent : public CBllBase
{
public:
	COnlineMessageEvent()
	{
	}

	virtual ~COnlineMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

};

#define	CREATE_ONLINEEVENT_INSTANCE		CSingleton<COnlineMessageEvent>::CreateInstance
#define	GET_ONLINEEVENT_INSTANCE		CSingleton<COnlineMessageEvent>::GetInstance
#define	DESTROY_ONLINEEVENT_INSTANCE	CSingleton<COnlineMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END



#endif /* BLL_EVENT_ONLINE_H_ */
