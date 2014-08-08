/*
 * bll_timer_clearroomorderinfo.h
 *
 *  Created on: 2013-3-12
 *      Author: jimm
 */

#ifndef BLL_TIMER_CLEARROOMORDERINFO_H_
#define BLL_TIMER_CLEARROOMORDERINFO_H_

#include "def/server_namespace.h"
#include "bll/bll_base.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "ctl/song_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define MaxSongerTimerCount		1024

class CClearRoomOrderInfoTimer : public CBllBase
{
public:
	CClearRoomOrderInfoTimer()
	{
		m_nTimerCount = 0;
	}

	virtual ~CClearRoomOrderInfoTimer()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	int32_t CreateTimer(RoomID nRoomID, RoleID nSongerRoleID);

	int32_t DeleteTimer(RoleID nSongerRoleID);

private:

	int32_t DeleteTimerInfo(RoleID nSongerRoleID);

	int32_t ClearRoomOrderInfo(CFrameSession *pSession);

	int32_t UpdateUserAssetResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession *pSession);

	int32_t UpdateUserAssetTimeout(CFrameSession *pSession);

	class CClearRoomOrderInfoSessionData
	{
	public:
		RoomID			nRoomID;
		RoleID			nSongerRoleID;

		CClearRoomOrderInfoSessionData()
		{
			nRoomID = enmInvalidRoomID;
			nSongerRoleID = enmInvalidRoleID;
		}
	};

	class CUpdateUserAssetSessionData
	{
	public:
		SongOrderInfo	stSongOrderInfo;
	};


private:

	class SongerTimerInfo
	{
	public:
		SongerTimerInfo()
		{
			nSessionIndex = enmInvalidSessionIndex;
			nRoomID = enmInvalidRoomID;
			nSongerRoleID = enmInvalidRoleID;
		}

		SessionIndex 	nSessionIndex;
		RoomID			nRoomID;
		RoleID			nSongerRoleID;
	};

	int32_t				m_nTimerCount;
	SongerTimerInfo		m_arrSongerTimerInfo[MaxSongerTimerCount];
};

#define	CREATE_CLEARROOMORDERINFO_TIMER_INSTANCE		CSingleton<CClearRoomOrderInfoTimer>::CreateInstance
#define	GET_CLEARROOMORDERINFO_TIMER_INSTANCE			CSingleton<CClearRoomOrderInfoTimer>::GetInstance
#define	DESTROY_CLEARROOMORDERINFO_TIMER_INSTANCE		CSingleton<CClearRoomOrderInfoTimer>::DestroyInstance

#define g_ClearRoomOrderInfo			GET_CLEARROOMORDERINFO_TIMER_INSTANCE()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_CLEARROOMORDERINFO_H_ */
