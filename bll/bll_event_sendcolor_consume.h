/*
 *  bll_event_sendcolor_consume.h
 *
 *  To do：
 *  Created on: 	2012-5-29
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_SENDCOLOR_CONSUME_H_
#define BLL_EVENT_SENDCOLOR_CONSUME_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "ctl/item_rebulid_ctl.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CSendColorEvent : public CBllBase
{
public:
	CSendColorEvent()
	{
	}

	virtual ~CSendColorEvent()
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

#define	CREATE_SENDCOLOREVENT_INSTANCE		CSingleton<CSendColorEvent>::CreateInstance
#define	GET_SENDCOLOREVENT_INSTANCE			CSingleton<CSendColorEvent>::GetInstance
#define	DESTROY_SENDCOLOREVENT_INSTANCE		CSingleton<CSendColorEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_SENDCOLOR_CONSUME_H_ */
