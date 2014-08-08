/*
 * bll_event_songvote.h
 *
 *  Created on: 2013-3-26
 *      Author: jimm
 */

#ifndef BLL_EVENT_SONGVOTE_H_
#define BLL_EVENT_SONGVOTE_H_

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

class CSongVoteMessageEvent : public CBllBase
{
public:
	CSongVoteMessageEvent()
	{
	}

	virtual ~CSongVoteMessageEvent()
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

#define	CREATE_SONGVOTEEVENT_INSTANCE			CSingleton<CSongVoteMessageEvent>::CreateInstance
#define	GET_SONGVOTEEVENT_INSTANCE				CSingleton<CSongVoteMessageEvent>::GetInstance
#define	DESTROY_SONGVOTEEVENT_INSTANCE			CSingleton<CSongVoteMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_SONGVOTE_H_ */
