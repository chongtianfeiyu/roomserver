/*
 * bll_event_buildplayerdatanotice.h
 *
 *  Created on: 2012-4-25
 *      Author: jimm
 */

#ifndef BLL_EVENT_BUILDPLAYERDATANOTICE_H_
#define BLL_EVENT_BUILDPLAYERDATANOTICE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CBuildPlayerDataNoticeMessageEvent : public CBllBase
{
public:
	CBuildPlayerDataNoticeMessageEvent()
	{
	}

	virtual ~CBuildPlayerDataNoticeMessageEvent()
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

#define	CREATE_BUILDPLAYERDATANOTICEEVENT_INSTANCE	CSingleton<CBuildPlayerDataNoticeMessageEvent>::CreateInstance
#define	GET_BUILDPLAYERDATANOTICEEVENT_INSTANCE		CSingleton<CBuildPlayerDataNoticeMessageEvent>::GetInstance
#define	DESTROY_BUILDPLAYERDATANOTICEEVENT_INSTANCE	CSingleton<CBuildPlayerDataNoticeMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_BUILDPLAYERDATANOTICE_H_ */
