/*
 * bll_event_login.h
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#ifndef BLL_EVENT_LOGIN_H_
#define BLL_EVENT_LOGIN_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
//#include "def/def_message_body.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"
#include "dal/from_server_message.h"
#include "dal/dal_room.h"
#include "ctl/rebot_ctl.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CLoginMessageEvent : public CBllBase
{
public:
	CLoginMessageEvent()
	{
	}

	virtual ~CLoginMessageEvent()
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
	int32_t GetRoleInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetRoleInfoTimeout(CFrameSession* pSession);

	int32_t GetRoomInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetRoomInfoTimeout(CFrameSession* pSession);

	int32_t GetMediaInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetMediaInfoTimeout(CFrameSession* pSession);

	int32_t GetEnterRoomCountResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetEnterRoomCountTimeout(CFrameSession* pSession);

	int32_t GetItemInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession);

	int32_t GetItemInfoTimeout(CFrameSession* pSession);

	int32_t EnterRoom(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, CRoom *pRoom,
				const uint16_t nOptionLen = 0, const void *pOptionData = NULL,bool isNeedGetMediaInfo = false);

	int32_t VerifyPlayer(CRoom *pRoom, CPlayer *pPlayer, PlayerIndex nPlayerIndex,uint32_t	nAddress,CLoginReq *stLoginReq,
				CLoginResp &stLoginResp, CLoginNoti &stLoginNoti);

	int32_t WatchShow(CRoom *pRoom, CPlayer *pPlayer,CLoginResp &stLoginResp,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t GetEnterRoomCountFromHall(MessageHeadSS *pMsgHead, CLoginReq* pMsgBody, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
				const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t GetItemInfoFromItem(MessageHeadSS *pMsgHead, CLoginReq* pMsgBody, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
					const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t DistributionRobot(CRoom *pRoom);

	int32_t DestroyPlayerAndRoom(CRoom *pRoom,CPlayer *pPlayer);



	//保存请求数据到会话中
	class CGetRoomInfoSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		uint16_t m_nOptionLen;
		char m_arrOptionData[MaxOptionDataSize];
		CLoginReq m_pMsgBody;

		CGetRoomInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			m_nOptionLen = 0;
			memset(&m_arrOptionData, 0, sizeof(m_arrOptionData));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};

	class CGetMediaInfoSessionData
	{
	public:
		MessageHeadSS			m_sMsgHead;
		uint16_t				m_nOptionLen;
		char					m_arrOptionData[MaxOptionDataSize];
		CLoginReq				m_pMsgBody;

		CGetMediaInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			m_nOptionLen = 0;
			memset(&m_arrOptionData, 0, sizeof(m_arrOptionData));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};
	//保存请求数据到会话中
	class CGetRoleInfoSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		CLoginReq m_pMsgBody;

		CGetRoleInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};
	class CGetEnterRoomSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		CLoginReq m_pMsgBody;

		CGetEnterRoomSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};
	class CGetItemInfoSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		CLoginReq m_pMsgBody;

		CGetItemInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};
};

#define	CREATE_LOGINEVENT_INSTANCE	CSingleton<CLoginMessageEvent>::CreateInstance
#define	GET_LOGINEVENT_INSTANCE		CSingleton<CLoginMessageEvent>::GetInstance
#define	DESTROY_LOGINEVENT_INSTANCE	CSingleton<CLoginMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_LOGIN_H_ */
