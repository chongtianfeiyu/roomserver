/*
 *  bll_event_web_openprivatemic.h
 *
 *  To do：
 *  Created on: 	2012-7-5
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_WEB_OPENPRIVATEMIC_H_
#define BLL_EVENT_WEB_OPENPRIVATEMIC_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CWebOpenPrivateMicMessageEvent : public CBllBase
{
public:
	CWebOpenPrivateMicMessageEvent()
	{
	}

	virtual ~CWebOpenPrivateMicMessageEvent()
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

#define	CREATE_WEBOPENPRIVATEMICEVENT_INSTANCE		CSingleton<CWebOpenPrivateMicMessageEvent>::CreateInstance
#define	GET_WEBOPENPRIVATEMICEVENT_INSTANCE		CSingleton<CWebOpenPrivateMicMessageEvent>::GetInstance
#define	DESTROY_WEBOPENPRIVATEMICEVENT_INSTANCE	CSingleton<CWebOpenPrivateMicMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_WEB_OPENPRIVATEMIC_H_ */
