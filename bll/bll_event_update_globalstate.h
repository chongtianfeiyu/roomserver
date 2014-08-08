/*
 *  bll_event_update_globalstate.h
 *
 *  To do��
 *  Created on: 	2012-5-8
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_UPDATE_GLOBALSTATE_H_
#define BLL_EVENT_UPDATE_GLOBALSTATE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CUpdateStateMessageEvent : public CBllBase
{
public:
	CUpdateStateMessageEvent()
	{
	}

	virtual ~CUpdateStateMessageEvent()
	{
	}

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//�Ự�¼�����
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

};

#define	CREATE_UPDATESTATEEVENT_INSTANCE		CSingleton<CUpdateStateMessageEvent>::CreateInstance
#define	GET_UPDATESTATEEVENT_INSTANCE		CSingleton<CUpdateStateMessageEvent>::GetInstance
#define	DESTROY_UPDATESTATEEVENT_INSTANCE	CSingleton<CUpdateStateMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_UPDATE_GLOBALSTATE_H_ */
