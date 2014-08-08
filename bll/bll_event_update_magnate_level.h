/*
 *  bll_event_update_magnate_level.h
 *
 *  To do：
 *  Created on: 	2012-11-13
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_UPDATE_MAGNATE_LEVEL_H_
#define BLL_EVENT_UPDATE_MAGNATE_LEVEL_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CUpdateMagnateLevelMessageEvent : public CBllBase
{
public:
	CUpdateMagnateLevelMessageEvent()
	{
	}

	virtual ~CUpdateMagnateLevelMessageEvent()
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

#define	CREATE_UPDATEMAGNATELEVELEVENT_INSTANCE		CSingleton<CUpdateMagnateLevelMessageEvent>::CreateInstance
#define	GET_UPDATEMAGNATELEVELEVENT_INSTANCE		CSingleton<CUpdateMagnateLevelMessageEvent>::GetInstance
#define	DESTROY_UPDATEMAGNATELEVELEVENT_INSTANCE	CSingleton<CUpdateMagnateLevelMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_UPDATE_MAGNATE_LEVEL_H_ */
