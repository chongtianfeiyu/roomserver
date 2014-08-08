/*
 *  bll_event_redispatch_media.h
 *
 *  To do：
 *  Created on: 	2012-5-21
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_REDISPATCH_MEDIA_H_
#define BLL_EVENT_REDISPATCH_MEDIA_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CReDispatchMediaEvent : public CBllBase
{
public:
	CReDispatchMediaEvent()
	{
	}

	virtual ~CReDispatchMediaEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	int32_t StartTimer(RoomID nRoomID);

	int32_t TimerStartGetMediaTimeout(CFrameSession *pSession);

	int32_t GetMediaInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	class CTimerStartGetMediaData
	{
	public:
		RoomID			m_nRoomID;

		CTimerStartGetMediaData()
		{
			m_nRoomID = enmInvalidRoomID;
		}
	};

	class CGetMediaData
	{
	public:
		RoomID m_nRoomID;

		CGetMediaData()
		{
			m_nRoomID = enmInvalidRoomID;
		}
	};

};

#define	CREATE_REDISPATCHMEDIAEVENT_INSTANCE		CSingleton<CReDispatchMediaEvent>::CreateInstance
#define	GET_REDISPATCHMEDIAEVENT_INSTANCE			CSingleton<CReDispatchMediaEvent>::GetInstance
#define	DESTROY_REDISPATCHMEDIAEVENT_INSTANCE		CSingleton<CReDispatchMediaEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* BLL_EVENT_REDISPATCH_MEDIA_H_ */
