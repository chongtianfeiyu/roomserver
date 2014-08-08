/*
 * bll_event_buildplayerdatanotice.cpp
 *
 *  Created on: 2012-4-25
 *      Author: jimm
 */

#include "bll_event_buildplayerdatanotice.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "roomdispatcher_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CBuildPlayerDataNoticeMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CBuildPlayerDataNoticeMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CBuildPlayerDataNoticeMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CBuildPlayerDataNoti *pBuildPlayerDataNotice = dynamic_cast<CBuildPlayerDataNoti *>(pMsgBody);
	if(NULL == pBuildPlayerDataNotice)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}

	WRITE_DEBUG_LOG("recv BuildPlayerDataNotice{nPlayerCount=%d}\n",pBuildPlayerDataNotice->nPlayerCount);

	for(int32_t i = 0; i < pBuildPlayerDataNotice->nPlayerCount; ++i)
	{
		CPlayer *pLocalPlayer = NULL;
		PlayerIndex nLocalPlayerIndex = enmInvalidPlayerIndex;
		int32_t ret = g_PlayerMgt.GetPlayer(pBuildPlayerDataNotice->arrRoleID[i], pLocalPlayer, nLocalPlayerIndex);
		//本地roomserver上没有这个玩家
		if(ret < 0 || pLocalPlayer == NULL)
		{
			ret = g_PlayerMgt.CreatePlayer(pBuildPlayerDataNotice->arrRoleID[i], pLocalPlayer, nLocalPlayerIndex);
			if(ret < 0 || pLocalPlayer == NULL)
			{
				WRITE_ERROR_LOG("null pointer:{pLocalPlayer=0x%08x, pMsgHead=0x%08x}\n",pLocalPlayer);
				continue;
			}

			uint32_t nStartPos = pLocalPlayer->GetStartPos();
			uint32_t nEndPos = pLocalPlayer->GetEndPos();

			uint32_t nPlayerDataSize = nEndPos - nStartPos;
			if(nPlayerDataSize >= pBuildPlayerDataNotice->arrPlayerDataSize[i])
			{
				memcpy(((uint8_t *)pLocalPlayer) + nStartPos, pBuildPlayerDataNotice->arrPlayerData[i], pBuildPlayerDataNotice->arrPlayerDataSize[i]);
			}

			RoomID arrRoomID[MaxEnterRoomCount];
			int32_t nEnterRoomCount = 0;
			pLocalPlayer->GetAllEnterRoom(arrRoomID, MaxEnterRoomCount, nEnterRoomCount);
			WRITE_DEBUG_LOG("building player data:{nEnterCount=%d}\n",nEnterRoomCount);
			for(int32_t j = 0; j < nEnterRoomCount; ++j)
			{
				CRoom *pRoom = NULL;
				RoomIndex nRoomIndex = enmInvalidRoomIndex;
				ret = g_RoomMgt.GetRoom(arrRoomID[j], pRoom, nRoomIndex);
				if(ret < 0 || nRoomIndex == enmInvalidRoomIndex)
				{
					WRITE_WARNING_LOG("building player data:but it's not found room object,or room is not rebulid!{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i],arrRoomID[j]);
					continue;
				}

				if(pRoom->IsPlayerInRoom(nLocalPlayerIndex)||pRoom->IsRebotPlayerInRoom(pBuildPlayerDataNotice->arrRoleID[i]))
				{
					WRITE_NOTICE_LOG("building player data:player in this room{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i], arrRoomID[j]);
					continue;
				}

				//将玩家添加到房间对象中去
				if(pLocalPlayer->IsReboot())
				{
					pRoom->AddRebotPlayer(pBuildPlayerDataNotice->arrRoleID[i]);
				}
				else
				{
					pRoom->AddPlayer(nLocalPlayerIndex, pLocalPlayer->GetVipLevel(), pLocalPlayer->IsHideEnter());
				}
				WRITE_NOTICE_LOG("building player data:player rebulid in room{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i],arrRoomID[j]);
			}
		}
		else
		{
			CPlayer stPlayer;
			CPlayer *pRemotePlayer = &stPlayer;

			uint32_t nStartPos = pRemotePlayer->GetStartPos();
			uint32_t nEndPos = pRemotePlayer->GetEndPos();

			uint32_t nPlayerDataSize = nEndPos - nStartPos;
			if(nPlayerDataSize >= pBuildPlayerDataNotice->arrPlayerDataSize[i])
			{
				memcpy(((uint8_t *)pRemotePlayer) + nStartPos, pBuildPlayerDataNotice->arrPlayerData[i], pBuildPlayerDataNotice->arrPlayerDataSize[i]);
			}

			PlayerRoomInfo arrPlayerRoomInfo[MaxEnterRoomCount];
			int32_t nRoomInfoCount = 0;
			pRemotePlayer->GetPlayerRoomInfo(arrPlayerRoomInfo, MaxEnterRoomCount, nRoomInfoCount);

			//只合并目前本地服务器上已有的房间数据
			int32_t nEnterRoomCount = nRoomInfoCount;
			nRoomInfoCount = 0;
			for(int32_t j = 0; j < nEnterRoomCount; ++j)
			{
				CRoom *pRoom = NULL;
				RoomIndex nRoomIndex = enmInvalidRoomIndex;
				ret = g_RoomMgt.GetRoom(arrPlayerRoomInfo[j].nRoomID, pRoom, nRoomIndex);
				if(ret < 0 || pRoom == NULL)
				{
					WRITE_WARNING_LOG("building player data:but it's not found room object,or room is not rebulid!{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i],arrPlayerRoomInfo[j].nRoomID);
					continue;
				}
				//将玩家添加到房间对象中去(修改于2012.6.5)
				if(pRoom->IsPlayerInRoom(nLocalPlayerIndex)||pRoom->IsRebotPlayerInRoom(pBuildPlayerDataNotice->arrRoleID[i]))
				{
					WRITE_NOTICE_LOG("building player data:player in this room{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i], arrPlayerRoomInfo[j].nRoomID);
					continue;
				}
				if(pRemotePlayer->IsReboot())
				{
					pRoom->AddRebotPlayer(pBuildPlayerDataNotice->arrRoleID[i]);
				}
				else
				{
					pRoom->AddPlayer(nLocalPlayerIndex, pRemotePlayer->GetVipLevel(), pRemotePlayer->IsHideEnter());
				}

				arrPlayerRoomInfo[nRoomInfoCount] = arrPlayerRoomInfo[j];
				++nRoomInfoCount;
				WRITE_NOTICE_LOG("building player data:player rebulid in room{nRoleID=%d, nRoomID=%d}\n", pBuildPlayerDataNotice->arrRoleID[i],arrPlayerRoomInfo[j].nRoomID);
			}
			//合并player
			pLocalPlayer->MergePlayerRoomInfo(arrPlayerRoomInfo, nRoomInfoCount);
		}
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
