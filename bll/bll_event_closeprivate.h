/*
 *  bll_event_closeprivate.h
 *
 *  To do：
 *  Created on: 	2011-12-17
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_CLOSEPRIVATE_H_
#define BLL_EVENT_CLOSEPRIVATE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CClosePrivateEvent:public CBllBase
{
public:
	CClosePrivateEvent()
	{
	}

	virtual ~CClosePrivateEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventClosePrivate(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody);
};

#define	CREATE_CLOSEPRIVATEEVENT_INSTANCE	CSingleton<CClosePrivateEvent>::CreateInstance
#define	GET_CLOSEPRIVATEEVENT_INSTANCE		CSingleton<CClosePrivateEvent>::GetInstance
#define	DESTROY_CLOSEPRIVATEEVENT_INSTANCE	CSingleton<CClosePrivateEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_CLOSEPRIVATE_H_ */
