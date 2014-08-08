/*
 *  bll_event_lockroom.cpp
 *
 *  To do：
 *  Created on: 	2012-4-12
 *  Author: 		luocj
 */

#include "bll_event_lockroom.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "public_message_define.h"
#include "dal/to_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CLockRoomMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CLockRoomMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CLockRoomMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CLockRoomNoitfy *pLockRoomNoitfy = dynamic_cast<CLockRoomNoitfy *>(pMsgBody);
	if(NULL == pLockRoomNoitfy)
	{
		WRITE_ERROR_LOG("null pointer:{pLockRoomNoitfy=0x%08x}\n",pLockRoomNoitfy);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("recv room is locked by web{nRoomID=%d}",pLockRoomNoitfy->nRoomID);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pLockRoomNoitfy->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not exit!{nRoomID=%d}",pLockRoomNoitfy->nRoomID);
		return E_NULLPOINTER;
	}
	//todo 通知房间里的人房间被封了
	CLockRoomNotifyToClient stLockRoomNotifyToClient;
	stLockRoomNotifyToClient.strReason = pLockRoomNoitfy->strLockRoomReason.GetString();
	stLockRoomNotifyToClient.nEndTime = pLockRoomNoitfy->nTimeUnlock;
	stLockRoomNotifyToClient.nLockDays = (pLockRoomNoitfy->nTimeUnlock-pLockRoomNoitfy->nTimeLock)/SECOND_PER_DAY;
	SendMessageNotifyToClient(MSGID_RSCL_SRV_ROOM_LOCKED_NOTI, &stLockRoomNotifyToClient, pLockRoomNoitfy->nRoomID, enmBroadcastType_All,
			enmInvalidRoleID,0,NULL, "send lock room notify");

	//踢出房间里的所有人
	PlayerIndex arrPlayerIndex[MaxUserCountPerRoom] = {enmInvalidPlayerIndex};
	int32_t nPlayerCount = 0;
	pRoom->GetAllPlayers(arrPlayerIndex, MaxUserCountPerRoom, nPlayerCount);
	for(int32_t i = 0; i < nPlayerCount; ++i)
	{
		CPlayer *pPlayer = NULL;
		g_PlayerMgt.GetPlayerByIndex(arrPlayerIndex[i], pPlayer);
		if(pPlayer == NULL)
		{
			WRITE_ERROR_LOG("player is not in this roomserver{nRoleID=%d}",pPlayer->GetRoleID());
			continue;
		}
		//玩家退房的相关操作
		//退房
		ret = ExitRoom(pPlayer,arrPlayerIndex[i],pRoom,pMsgHead->nMessageID);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("kick player filed because exit room filed{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pLockRoomNoitfy->nRoomID);
			continue;
		}
	}
	//为了容错再次删除房间（exitroom 中有删除房间的操作）
	g_RoomMgt.RealDestroyRoom(pLockRoomNoitfy->nRoomID);
	WRITE_NOTICE_LOG("lock room ok !{nRoomID=%d}",pLockRoomNoitfy->nRoomID);

	CDestoryRoomNotice stDestoryRoomNotice;
	stDestoryRoomNotice.nRoomID = pLockRoomNoitfy->nRoomID;
	stDestoryRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_Item);
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_Hall);
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_Media);
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_Tunnel);
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_RoomDispatcher);
	SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, pLockRoomNoitfy->nRoomID, enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_HallDataCenter);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

