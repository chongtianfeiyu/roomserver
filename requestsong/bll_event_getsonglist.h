/*
 * bll_event_getsonglist.h
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#ifndef BLL_EVENT_GETSONGLIST_H_
#define BLL_EVENT_GETSONGLIST_H_

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

class CGetSongListMessageEvent : public CBllBase
{
public:
	CGetSongListMessageEvent()
	{
	}

	virtual ~CGetSongListMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

private:
	int32_t GetSongListResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetSongListTimeout(CFrameSession *pSession);

	//保存请求数据到会话中
	class CGetSongListSessionData
	{
	public:
		MessageHeadSS m_stMsgHead;
		uint16_t m_nOptionLen;
		char m_arrOptionData[MaxOptionDataSize];
		CGetSongListReq m_stMsgBody;

		CGetSongListSessionData()
		{
			memset(&m_stMsgHead, 0, sizeof(m_stMsgHead));
			m_nOptionLen = 0;
			memset(&m_arrOptionData, 0, sizeof(m_arrOptionData));
			memset(&m_stMsgBody, 0, sizeof(m_stMsgBody));
		}
	};
};

#define	CREATE_GETSONGLISTEVENT_INSTANCE		CSingleton<CGetSongListMessageEvent>::CreateInstance
#define	GET_GETSONGLISTEVENT_INSTANCE			CSingleton<CGetSongListMessageEvent>::GetInstance
#define	DESTROY_GETSONGLISTEVENT_INSTANCE		CSingleton<CGetSongListMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_GETSONGLIST_H_ */
