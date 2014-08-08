/*
 * bll_event_getuserlist.h
 *
 *  Created on: 2011-12-24
 *      Author: jimm
 */

#ifndef BLL_EVENT_GETUSERLIST_H_
#define BLL_EVENT_GETUSERLIST_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"
#include "dal/dal_room.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_server_message.h"
#include "dal/to_server_message.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CGetUsetListMessageEvent : public CBllBase
{
public:
	CGetUsetListMessageEvent()
	{
	}

	virtual ~CGetUsetListMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
	int32_t GetItemInfoFromItem(MessageHeadSS *pMsgHead, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
						const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
private:
	int32_t GetItemInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetItemInfoTimeout(CFrameSession* pSession);

	class CGetItemInfoSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;

		CGetItemInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
		}
	};
};

#define	CREATE_GETUSERLISTEVENT_INSTANCE	CSingleton<CGetUsetListMessageEvent>::CreateInstance
#define	GET_GETUSERLISTEVENT_INSTANCE		CSingleton<CGetUsetListMessageEvent>::GetInstance
#define	DESTROY_GETUSERLISTEVENT_INSTANCE	CSingleton<CGetUsetListMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_GETUSERLIST_H_ */
