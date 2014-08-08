/*
 * bll_event_getorderlist.h
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#ifndef BLL_EVENT_GETORDERLIST_H_
#define BLL_EVENT_GETORDERLIST_H_

#include "def/server_namespace.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"
#include "dal/from_server_message.h"
#include "dal/dal_room.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "main_frame.h"
#include "bll/bll_base.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CGetOrderListMessageEvent : public CBllBase
{
public:
	CGetOrderListMessageEvent()
	{
	}

	virtual ~CGetOrderListMessageEvent()
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

#define	CREATE_GETORDERLISTEVENT_INSTANCE		CSingleton<CGetOrderListMessageEvent>::CreateInstance
#define	GET_GETORDERLISTEVENT_INSTANCE			CSingleton<CGetOrderListMessageEvent>::GetInstance
#define	DESTROY_GETORDERLISTEVENT_INSTANCE		CSingleton<CGetOrderListMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_GETORDERLIST_H_ */
