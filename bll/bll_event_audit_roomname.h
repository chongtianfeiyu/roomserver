/*
 *  bll_event_audit_roomname.h
 *
 *  To do：
 *  Created on: 	2012-4-13
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_AUDIT_ROOMNAME_H_
#define BLL_EVENT_AUDIT_ROOMNAME_H_


#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CAuditRoomNameMessageEvent : public CBllBase
{
public:
	CAuditRoomNameMessageEvent()
	{
	}

	virtual ~CAuditRoomNameMessageEvent()
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

#define	CREATE_AUDITROOMNAMEEVENT_INSTANCE	CSingleton<CAuditRoomNameMessageEvent>::CreateInstance
#define	GET_AUDITROOMNAMEEVENT_INSTANCE		CSingleton<CAuditRoomNameMessageEvent>::GetInstance
#define	DESTROY_AUDITROOMNAMEEVENT_INSTANCE	CSingleton<CAuditRoomNameMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_AUDIT_ROOMNAME_H_ */
