/*
 *  bll_event_modify_capacity.cpp
 *
 *  To do：
 *  Created on: 	2012-4-13
 *  Author: 		luocj
 */

#include "bll_event_modify_capacity.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "public_message_define.h"
#include "dal/to_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CModifyCapacityMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CModifyCapacityMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CModifyCapacityMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CModifyRoomCapacityNotify *pModifyRoomCapacityNotify = dynamic_cast<CModifyRoomCapacityNotify *>(pMsgBody);
	if(NULL == pModifyRoomCapacityNotify)
	{
		WRITE_ERROR_LOG("null pointer:{pModifyRoomCapacityNotify=0x%08x}\n",pModifyRoomCapacityNotify);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("modify room capacity:recv from web {nRoomID=%d}",pModifyRoomCapacityNotify->nRoomID);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pModifyRoomCapacityNotify->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("modify room capacity:room is not exit in this server!{nRoomID=%d}",pModifyRoomCapacityNotify->nRoomID);
		return E_NULLPOINTER;
	}
	//后台设置房间容量的时候的一个保护
	if(pModifyRoomCapacityNotify->nRoomShowCapacity > pModifyRoomCapacityNotify->nRoomRealCapacity
			|| pModifyRoomCapacityNotify->nRoomRealCapacity > MaxUserCountPerRoom)
	{
		WRITE_WARNING_LOG("modify room capacity: value is error {nRoomRealCapacity=%d,nRoomShowCapacity=%d,MaxUserCountPerRoom=%d}",pModifyRoomCapacityNotify->nRoomRealCapacity,pModifyRoomCapacityNotify->nRoomShowCapacity,MaxUserCountPerRoom);
		return S_OK;
	}

	//判断是否需要踢出玩家
	pRoom->SetMaxRealUserLimit(pModifyRoomCapacityNotify->nRoomRealCapacity);
	pRoom->SetMaxShowUserLimit(pModifyRoomCapacityNotify->nRoomShowCapacity);

	int32_t nNowUserCount = pRoom->GetCurUserCount() + pRoom->GetRobotCount();
	if(nNowUserCount <= pRoom->GetMaxRealUserLimit())
	{
		WRITE_NOTICE_LOG("modify room capacity:room not need kick player {nRoomID=%d, nNowRealCapacity=%d, nRoomRealCapacity=%d, nNowUserCount=%d}!",pModifyRoomCapacityNotify->nRoomID,pRoom->GetMaxRealUserLimit(),pModifyRoomCapacityNotify->nRoomRealCapacity,nNowUserCount);
		return S_OK;
	}

	//踢出玩家
	KickPlayerForModifyCapacity(pRoom,pModifyRoomCapacityNotify->nRoomRealCapacity);
	return S_OK;
}

int32_t CModifyCapacityMessageEvent::KickPlayerForModifyCapacity(CRoom *pRoom,int32_t nModifyCapacity)
{
	int32_t ret = S_OK;
	if(pRoom == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return ret;
	}
	PlayerIndex arrPlayerIndex[MaxUserCountPerRoom] = {enmInvalidPlayerIndex};
	int32_t nPlayerCount = 0;
	pRoom->GetAllPlayers(arrPlayerIndex, MaxUserCountPerRoom, nPlayerCount);

	RoleID arrRobotRoleID[MaxUserCountPerRoom];
	int32_t nRobotCount = 0;
	pRoom->GetAllRebotPlayers(arrRobotRoleID,MaxUserCountPerRoom,nRobotCount);
	int32_t nTotalPlayerCount = nPlayerCount+nRobotCount;

	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	for(int32_t i = 0; i < nTotalPlayerCount && i < MaxUserCountPerRoom; ++i)
	{
		if(i<nPlayerCount)
		{
			g_PlayerMgt.GetPlayerByIndex(arrPlayerIndex[i],arrPlayer[i]);
		}
		else
		{
			g_PlayerMgt.GetPlayer(arrRobotRoleID[i-nPlayerCount],arrPlayer[i],arrPlayerIndex[i]);
		}
	}
	//需要踢得人
	uint32_t nNeedKickPlayer = nTotalPlayerCount-nModifyCapacity;
	WRITE_NOTICE_LOG("modify room capacity:room need kick player{nNeedKickPlayer=%d, nRoomRealCapacity=%d, nNowUserCount=%d}!",nNeedKickPlayer,nModifyCapacity,nTotalPlayerCount);
	//保存第二遍踢得人
	CPlayer *arrSecondPlayer[MaxUserCountPerRoom] = {NULL};
	uint32_t nSecondKickCount = 0;
    //第一遍踢人，根据vipLevel
	SortPlayerByVipLevel(arrPlayer,nTotalPlayerCount);
	uint32_t nKickPlayer = 0;
    for(int32_t i = nTotalPlayerCount-1;i>=0;i--)
    {
    	if(arrPlayer[i] == NULL)
    	{
    		continue;
    	}
    	if(nKickPlayer >= nNeedKickPlayer)
    	{
    		WRITE_NOTICE_LOG("modify room capacity:kick player{nKickPlayer=%d, nNeedKickPlayer=%d}!!!",nKickPlayer,nNeedKickPlayer);
    		return S_OK;
    	}
       //提出非皇冠，和非管理的人
    	if(arrPlayer[i]->GetVipLevel() >= enmVipLevel_King)
    	{
    		arrSecondPlayer[nSecondKickCount++] = arrPlayer[i];
    		break;
    	}
    	if(pRoom->GetRoleRank(arrPlayer[i]->GetRoleID()) >= enmRoleRank_TempAdmin)
    	{
    		arrSecondPlayer[nSecondKickCount++] = arrPlayer[i];
    		continue;
    	}
    	//通知房间的所有玩家
    	WRITE_NOTICE_LOG("modify room capacity:kick player{nRoleID=%d, nVipLevel=%d}",arrPlayer[i]->GetRoleID(),arrPlayer[i]->GetVipLevel());
    	SendNotifyToUser(pRoom->GetRoomID(),arrPlayer[i]->GetRoleID());
    	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
    	ret = g_PlayerMgt.GetPlayerIndex(arrPlayer[i]->GetRoleID(),nPlayerIndex);
    	if(ret < 0)
    	{
    		arrSecondPlayer[nSecondKickCount++] = arrPlayer[i];
    		WRITE_ERROR_LOG("modify room capacity:kick player filed because get playerIndex filed{nRoleID=%d}",arrPlayer[i]->GetRoleID());
    		continue;
    	}
    	//退房
		ret = ExitRoom(arrPlayer[i],nPlayerIndex,pRoom,MSGID_WEBOT_MODIFY_ROOMCAPACITY_NOTIFY,false,false);
		if(ret < 0)
		{
			arrSecondPlayer[nSecondKickCount++] = arrPlayer[i];
			WRITE_ERROR_LOG("modify room capacity:kick player filed because exit room filed{nRoleID=%d}",arrPlayer[i]->GetRoleID());
			continue;
		}
		nKickPlayer++;
    }
    nTotalPlayerCount = nTotalPlayerCount-nKickPlayer;
    if(nTotalPlayerCount <= 0)
    {
    	//容错再次销毁房间
    	g_RoomMgt.DestroyRoom(pRoom->GetRoomID());
    	return S_OK;
    }

    //开始第二次踢人，根据管理权限
    SortPlayerByAdminLevel(arrSecondPlayer,nSecondKickCount,pRoom->GetRoomID());
    for(int32_t i = nSecondKickCount-1;i >= 0;i--)
	{
    	if(arrSecondPlayer[i] == NULL)
		{
			continue;
		}
		if(nKickPlayer >= nNeedKickPlayer)
		{
			WRITE_NOTICE_LOG("modify room capacity:kick player{nKickPlayer=%d, nNeedKickPlayer=%d}!!!",nKickPlayer,nNeedKickPlayer);
			return S_OK;
		}
		if(pRoom->GetRoleRank(arrSecondPlayer[i]->GetRoleID())>=enmRoleRank_Host)
		{
			continue;
		}
		//通知房间的所有玩家
		WRITE_NOTICE_LOG("modify room capacity:kick player{nRoleID=%d, nRoleRank=%d}",arrSecondPlayer[i]->GetRoleID(),pRoom->GetRoleRank(arrSecondPlayer[i]->GetRoleID()));
		SendNotifyToUser(pRoom->GetRoomID(),arrSecondPlayer[i]->GetRoleID());
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayerIndex(arrSecondPlayer[i]->GetRoleID(),nPlayerIndex);
		if(ret<0)
		{
			WRITE_ERROR_LOG("modify room capacity:kick player filed because get playerIndex filed{nRoleID=%d}",arrSecondPlayer[i]->GetRoleID());
			continue;
		}
		//退房
		ret = ExitRoom(arrSecondPlayer[i],nPlayerIndex,pRoom,MSGID_WEBOT_MODIFY_ROOMCAPACITY_NOTIFY,false,false);
		if(ret<0)
		{
			WRITE_ERROR_LOG("modify room capacity:kick player filed because exit room filed{nRoleID=%d}",arrSecondPlayer[i]->GetRoleID());
			continue;
		}
		nKickPlayer++;
	}
    WRITE_NOTICE_LOG("modify room capacity:final kick player!{nKickPlayer=%d, nNeedKickPlayer=%d}!!!",nKickPlayer,nNeedKickPlayer);
	return ret;
}

/*
 * 发送通知给user
 */
int32_t CModifyCapacityMessageEvent::SendNotifyToUser(RoomID nRoomID,RoleID nDestRoleID )
{
	CKickUserNoti stKickUserNoti;
	stKickUserNoti.nSrcRoleID = nDestRoleID;
	stKickUserNoti.nKickAllVisitor = enmKickType_KickedForModifyCapacity;
	stKickUserNoti.nKickRoleID = nDestRoleID;
	stKickUserNoti.strReason = "modify capacity";
	stKickUserNoti.nTime = 0;
	//发送给除发起请求的玩家
	SendMessageNotifyToClient(MSGID_RSCL_KICK_USER_NOTI, &stKickUserNoti, nRoomID, enmBroadcastType_All,
			nDestRoleID,0,NULL, "send kick user notify for modify capacity");
	return S_OK;
}
FRAME_ROOMSERVER_NAMESPACE_END
