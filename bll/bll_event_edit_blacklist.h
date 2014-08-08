/*
 *  bll_event_edit_blacklist.h
 *
 *  To do：
 *  Created on: 	2012-6-14
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_EDIT_BLACKLIST_H_
#define BLL_EVENT_EDIT_BLACKLIST_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "global.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CDelBlackListEvent : public CBllBase
{
public:
	CDelBlackListEvent()
	{
	}

	virtual ~CDelBlackListEvent()
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

#define	CREATE_EDITBLACKLISTEVENT_INSTANCE	    CSingleton<CDelBlackListEvent>::CreateInstance
#define	GET_EDITBLACKLISTEVENT_INSTANCE			CSingleton<CDelBlackListEvent>::GetInstance
#define	DESTROY_EDITBLACKLISTEVENT_INSTANCE		CSingleton<CDelBlackListEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_DEL_BLACKLIST_H_ */
