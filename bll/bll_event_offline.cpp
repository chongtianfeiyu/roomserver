/*
 * bll_event_offline.cpp
 *
 *  Created on: 2012-1-17
 *      Author: jimm
 */

#include "bll_event_offline.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "hallserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t COfflineMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t COfflineMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t COfflineMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	COfflineNoti *pOfflineNoti = dynamic_cast<COfflineNoti *>(pMsgBody);
	if(NULL == pOfflineNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pOfflineNoti=0x%08x}\n",pOfflineNoti);
		return E_NULLPOINTER;
	}

	WRITE_NOTICE_LOG("offline:recv roleid offline noti{nRoleID=%d}",pMsgHead->nRoleID);

	CConnInfoMgt::iterator it = g_ConnInfoMgt.find(pOfflineNoti->nRoleID);
	if(it != g_ConnInfoMgt.end())
	{
		FREE((uint8_t *)(it->second));
		g_ConnInfoMgt.erase(it);
	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_DEBUG_LOG("get player object failed{nRoleID=%d, ret=0x%08x}\n",pMsgHead->nRoleID, ret);
		return E_NULLPOINTER;
	}

	RoomID arrRoomID[MaxEnterRoomCount];
	int32_t nRoomCount = 0;
	pPlayer->GetAllEnterRoom(arrRoomID, MaxEnterRoomCount, nRoomCount);
	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		CRoom *pRoom = NULL;
		RoomIndex nRoomIndex = enmInvalidRoomIndex;
		ret = g_RoomMgt.GetRoom(arrRoomID[i], pRoom, nRoomIndex);
		if(ret < 0 || pRoom == NULL)
		{
			continue;
		}
        //ÍË·¿
		ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,0x00018002,true);
		if(ret<0)
		{
			WRITE_ERROR_LOG(" player exit room filed{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,arrRoomID[i]);
		}
	}

	g_PlayerMgt.DestroyPlayer(pMsgHead->nRoleID);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

