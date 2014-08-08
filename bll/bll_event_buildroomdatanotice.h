/*
 * bll_event_buildroomdatanotice.h
 *
 *  Created on: 2012-4-25
 *      Author: jimm
 */

#ifndef BLL_EVENT_BUILDROOMDATANOTICE_H_
#define BLL_EVENT_BUILDROOMDATANOTICE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CBuildRoomDataNoticeMessageEvent : public CBllBase
{
public:
	CBuildRoomDataNoticeMessageEvent()
	{
	}

	virtual ~CBuildRoomDataNoticeMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

private:

};

#define	CREATE_BUILDROOMDATANOTICEEVENT_INSTANCE	CSingleton<CBuildRoomDataNoticeMessageEvent>::CreateInstance
#define	GET_BUILDROOMDATANOTICEEVENT_INSTANCE		CSingleton<CBuildRoomDataNoticeMessageEvent>::GetInstance
#define	DESTROY_BUILDROOMDATANOTICEEVENT_INSTANCE	CSingleton<CBuildRoomDataNoticeMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_BUILDROOMDATANOTICE_H_ */
