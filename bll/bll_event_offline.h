/*
 * bll_event_offline.h
 *
 *  Created on: 2012-1-17
 *      Author: jimm
 */

#ifndef BLL_EVENT_OFFLINE_H_
#define BLL_EVENT_OFFLINE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class COfflineMessageEvent : public CBllBase
{
public:
	COfflineMessageEvent()
	{
	}

	virtual ~COfflineMessageEvent()
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

#define	CREATE_OFFLINEEVENT_INSTANCE		CSingleton<COfflineMessageEvent>::CreateInstance
#define	GET_OFFLINEEVENT_INSTANCE		CSingleton<COfflineMessageEvent>::GetInstance
#define	DESTROY_OFFLINEEVENT_INSTANCE	CSingleton<COfflineMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* BLL_EVENT_OFFLINE_H_ */
