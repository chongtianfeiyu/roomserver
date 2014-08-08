/*
 *  bll_event_openprivate.h
 *
 *  To do：
 *  Created on: 	2011-12-17
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_OPENPRIVATE_H_
#define BLL_EVENT_OPENPRIVATE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class COpenPrivateEvent:public CBllBase
{
public:
	COpenPrivateEvent()
	{
	}

	virtual ~COpenPrivateEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventOpenPrivate(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
};

#define	CREATE_OPENPRIVATEEVENT_INSTANCE	CSingleton<COpenPrivateEvent>::CreateInstance
#define	GET_OPENPRIVATEEVENT_INSTANCE		CSingleton<COpenPrivateEvent>::GetInstance
#define	DESTROY_OPENPRIVATEEVENT_INSTANCE	CSingleton<COpenPrivateEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_OPENPRIVATE_H_ */
