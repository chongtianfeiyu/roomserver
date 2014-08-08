/*
 *  bll_event_setmic.h
 *
 *  To do：
 *  Created on: 	2012-5-7
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_SETMIC_H_
#define BLL_EVENT_SETMIC_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "dal/to_server_message.h"
#include "../dal/to_client_message.h"
#include "ctl/server_datacenter.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CSetMicStatusMessageEvent : public CBllBase
{
public:
	CSetMicStatusMessageEvent()
	{
	}

	virtual ~CSetMicStatusMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,SetMICResult nSetMICResult,const uint16_t nOptionLen , const void *pOptionData);
};

#define	CREATE_SETMICSTATUSEVENT_INSTANCE		CSingleton<CSetMicStatusMessageEvent>::CreateInstance
#define	GET_SETMICSTATUSEVENT_INSTANCE		CSingleton<CSetMicStatusMessageEvent>::GetInstance
#define	DESTROY_SETMICSTATUSEVENT_INSTANCE	CSingleton<CSetMicStatusMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_SETMIC_H_ */
