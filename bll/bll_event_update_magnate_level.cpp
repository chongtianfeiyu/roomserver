/*
 *  bll_event_update_magnate_level.cpp
 *
 *  To do£º
 *  Created on: 	2012-11-13
 *  Author: 		luocj
 */

#include "bll_event_update_magnate_level.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "public_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUpdateMagnateLevelMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CUpdateMagnateLevelMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CUpdateMagnateLevelMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CUpdateMagnateLevelNotify *pUpdateMagnateLevelNotify = dynamic_cast<CUpdateMagnateLevelNotify *>(pMsgBody);
	if(NULL == pUpdateMagnateLevelNotify)
	{
		WRITE_ERROR_LOG("null pointer:{pUpdateVipLevelNotify=0x%08x, nRoleID=%d}\n",pUpdateMagnateLevelNotify,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pUpdateMagnateLevelNotify->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_WARNING_LOG("get player object failed or is not in this server{RoleID=%d, ret=0x%08x}\n",pUpdateMagnateLevelNotify->nRoleID, ret);
		return E_NULLPOINTER;
	}


	pPlayer->SetMagnateLevel(pUpdateMagnateLevelNotify->nNowMagnateLevel);

	CUpdateUserInfoNotify stUpdateUserInfoNotify;

	int32_t nEnterCount = 0;
	RoomID arrEnterRoomID[MaxEnterRoomCount];
	pPlayer->GetEnterRoomInfo(arrEnterRoomID,nEnterCount,MaxEnterRoomCount);

	for(int32_t j = 0;j < nEnterCount;j++)
	{
		CRoom *pRoom = NULL;
		RoomIndex nRoomIndex = enmInvalidRoomIndex;
		ret = g_RoomMgt.GetRoom(arrEnterRoomID[j], pRoom, nRoomIndex);
		if(ret < 0 || pRoom == NULL)
		{
			WRITE_ERROR_LOG("get room error!ret=0x%08x{roomid=%d}\n",ret,arrEnterRoomID[j]);
			continue;
		}
		stUpdateUserInfoNotify.nRoleID = pPlayer->GetRoleID();
		stUpdateUserInfoNotify.nUpdateCount = 1;
		stUpdateUserInfoNotify.nType[0] = enmUserInfoType_Magnate_Level;
		stUpdateUserInfoNotify.stRoomUserInfo.nMagnateLevel = pUpdateMagnateLevelNotify->nNowMagnateLevel;

		SendMessageNotifyToClient(MSGID_RSCL_USERINFO_CHANGE_NOTI, &stUpdateUserInfoNotify, arrEnterRoomID[j], enmBroadcastType_ExceptPlayr,
				pPlayer->GetRoleID(), 0, NULL, "send update magnate level notify message");
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
