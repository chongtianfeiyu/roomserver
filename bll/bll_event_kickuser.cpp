/*
 *  bll_event_kickuser.cpp
 *
 *  To do：
 *  Created on: 	2012-1-10
 *  Author: 		luocj
 */

#include "bll_event_kickuser.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CKickUserEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CKickUserEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CKickUserEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_KICK_USER_REQ:
		OnEventKickUser(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

/*
 * 处理踢人消息
 */
int32_t CKickUserEvent::OnEventKickUser(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CKickUserReq *pCKickUserReq = dynamic_cast<CKickUserReq *>(pMsgBody);
	if(pCKickUserReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pCKickUserReq=0x%08x}\n",pCKickUserReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player kick user req{nSrcRoleID=%d, nRoomID=%d, nDestRoleID=%d, nKickType=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pCKickUserReq->nKickRoleID, pCKickUserReq->nKickType);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{ret=0x%08x}\n",ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}


	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//判断是否有踢人的权利
	if(nSrcRoleRank<enmRoleRank_Admin&&pPlayer->GetVipLevel()<enmVipLevel_Baron)
	{
		WRITE_ERROR_LOG("kick player filed because no permissions{nSrcRoleID=%d, nSrcRoleRank=0x%04x, nVipLevel=0x%04x}",pCKickUserReq->nKickRoleID,nSrcRoleRank,pPlayer->GetVipLevel());
		SendResponseToClient(pMsgHead,enmKickResult_NO_Permission,nOptionLen,pOptionData);
		return ret;
	}
	if(pCKickUserReq->nKickType == enmKickType_KickVisitor)
	{
		//发送踢出所有游客
		SendNotifyToUser(pRoom->GetRoomID(),pMsgHead->nRoleID,enmKickType_KickVisitor,enmInvalidRoleID,"visitor",0,0,NULL);
		ret = KickVisitor(pRoom);
		if(ret<0)
		{
			WRITE_ERROR_LOG("kick visitor filed {ret=0x%08x}\n",ret);
			SendResponseToClient(pMsgHead,enmKickResult_Umknown,nOptionLen,pOptionData);
			return ret;
		}
		//发送成功回应
		SendResponseToClient(pMsgHead,enmKickResult_OK,nOptionLen,pOptionData);
		KickRobot(pRoom, pRoom->GetNeedRobotCount());
		return ret;
	}
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pCKickUserReq->nKickRoleID, pDestPlayer, nDestPlayerIndex);
	//没有被踢玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nKickRoleID=%d}",pCKickUserReq->nKickRoleID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pCKickUserReq->nKickRoleID)))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pCKickUserReq->nKickRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//踢非游客
	KickResult nKickResult=enmKickResult_Umknown;
	ret = KickUser(pRoom,nSrcRoleRank,pPlayer->GetVipLevel(),pCKickUserReq->nKickRoleID,pCKickUserReq->strReason,pCKickUserReq->nTime,nKickResult);
	//发送成功回应
	SendResponseToClient(pMsgHead,nKickResult,nOptionLen,pOptionData);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("kick palyer error{nKickRoleID=%d, ret=0x%08x}",pCKickUserReq->nKickRoleID,ret);
		return ret;
	}
	//发送踢人通知到房间中的玩家
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,enmKickType_Other,pCKickUserReq->nKickRoleID,pCKickUserReq->strReason,pCKickUserReq->nTime,nOptionLen,pOptionData);
	//退房
	ret = ExitRoom(pDestPlayer,nDestPlayerIndex,pRoom,pMsgHead->nMessageID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("kick player filed because exit room filed{nKickRoleID=%d}",pCKickUserReq->nKickRoleID);
		return ret;
	}
	WRITE_NOTICE_LOG("player kick user success{nSrcRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pCKickUserReq->nKickRoleID);
	return ret;
}
/*
 * 踢出房间的所有游客
 */
int32_t CKickUserEvent::KickVisitor(CRoom *pRoom)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	PlayerIndex arrPlayerIndex[MaxUserCountPerRoom] = {enmInvalidPlayerIndex};
	int32_t nPlayerCount = 0;
	pRoom->GetAllPlayers(arrPlayerIndex, MaxUserCountPerRoom, nPlayerCount);

	for(int32_t i = 0;i < nPlayerCount;i++)
	{
		CPlayer *pPlayer = NULL;
		ret = g_PlayerMgt.GetPlayerByIndex(arrPlayerIndex[i],pPlayer);
		if(ret < 0 || NULL == pPlayer)
		{
			WRITE_DEBUG_LOG("get player filed {nRoleID=%d, ret=0x%08x} \n",arrPlayerIndex[i],ret);
			continue;
		}
		RoleID nKickRoleID = pPlayer->GetRoleID();
		//非游客不提
		if(pPlayer->GetVipLevel() > enmVipLevel_NONE)
		{
			continue;
		}
		//其他操作:从房间队列删除、加入被踢队列
		uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
		uint32_t nEndTime = pRoom->GetEndKickTime(nKickRoleID);
		if(nEndTime!=0)
		{
			WRITE_ERROR_LOG("kick player filed because is did in kicklist{nKickRoleID=%d}",nKickRoleID);
			continue;
		}
		//踢游客的时候时间默认为5分钟
		ret = pRoom->AddKickList(nKickRoleID,nCurTime+5*60,"visitor");
		if(ret<0)
		{
			WRITE_ERROR_LOG("kick player filed because add to kicklist error{nKickRoleID=%d}",nKickRoleID);
			continue;
		}
		pPlayer->ExitRoom(pRoom->GetRoomID());
		ret = pRoom->DeletePlayer(arrPlayerIndex[i],pPlayer->IsHideEnter());
		if(ret < 0)
		{
			WRITE_ERROR_LOG("kick player filed because deletePlayer error{nKickRoleID=%d}",nKickRoleID);
			continue;
		}
		//给其他的server发通知
		SendNotifyToServer(pRoom,pRoom->GetRoomID(),nKickRoleID,pPlayer);
		//销毁玩家对象
		if(pPlayer->GetCurEnterRoomCount() == 0)
		{
			WRITE_DEBUG_LOG("player is exit all room so destroy player!{nKickRoleID=%d}\n",nKickRoleID);
			g_PlayerMgt.DestroyPlayer(nKickRoleID);
		}

	}
	CStaPlayerCount stStaPlayerCount;
	stStaPlayerCount.nRoomID = pRoom->GetRoomID();
	stStaPlayerCount.nPlayerCount = pRoom->GetCurUserCount();
	stStaPlayerCount.nRobotCount = pRoom->GetRobotCount();

	SendMessageNotifyToServer(MSGID_RSDB_STA_PLAYER_COUNT_NOTIFY, &stStaPlayerCount, pRoom->GetRoomID(), enmTransType_ByKey, pRoom->GetRoomID(), enmEntityType_DBProxy);
	return ret;
}

/*
 * 踢掉玩家
 */
int32_t CKickUserEvent::KickUser(CRoom *pRoom,RoleRank nSrcRoleRank,VipLevel nSrcVipLevel,RoleID nKickRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,KickResult &nKickResult)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("pRoom null ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nKickRoleID, pPlayer, nPlayerIndex);
	//没有被踢玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nKickRoleID=%d}",nKickRoleID);
		return ret;
	}
	RoleRank nRoleRank = pRoom->GetRoleRank(nKickRoleID);
	VipLevel nDestVipLevel = pPlayer->GetVipLevel();
	//被踢的人有管理权限
	if(nRoleRank >= enmRoleRank_TempAdmin)
	{

		if(nRoleRank>=nSrcRoleRank)
		{
			WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d}",nKickRoleID,nSrcRoleRank,nRoleRank);
			nKickResult = enmKickResult_NO_Permission;
			return E_RS_NOPERMISSIONS;
		}
		//有冠
		if(nDestVipLevel >= enmVipLevel_Baron)
		{
			//todo 判断VIP 皇冠等级（银、金冠、水晶只有室主可踢，其他冠不可以踢）
			if((pPlayer->GetVipLevel() >= enmVipLevel_Marquis && nSrcRoleRank < enmRoleRank_Super)
					|| (pPlayer->GetVipLevel() <= enmVipLevel_Earl && pPlayer->GetVipLevel() >= enmVipLevel_Baron && nSrcRoleRank < enmRoleRank_Host))
			{
				WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestVipLevel=%d}",nKickRoleID,nSrcRoleRank,pPlayer->GetVipLevel());
				nKickResult = enmKickResult_NO_Permission_King;
				return E_RS_NOPERMISSIONS_KING;
			}
		}
	}
	//被踢的人没有管理权限（权限和vip上都踢不了的时候返回）
	else
	{
		if(nDestVipLevel >= enmVipLevel_Baron)
		{
			if(nDestVipLevel >= nSrcVipLevel)
			{
				//todo 判断VIP 皇冠等级（银、金冠、水晶只有室主可踢，其他冠不可以踢）
				if((pPlayer->GetVipLevel() >= enmVipLevel_Marquis && nSrcRoleRank<enmRoleRank_Super)
						|| (pPlayer->GetVipLevel() <= enmVipLevel_Earl&&pPlayer->GetVipLevel() >= enmVipLevel_Baron && nSrcRoleRank < enmRoleRank_Host))
				{
					WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestVipLevel=%d}",nKickRoleID,nSrcRoleRank,pPlayer->GetVipLevel());
					nKickResult = enmKickResult_NO_Permission_King;
					return E_RS_NOPERMISSIONS_KING;
				}
			}
		}
		else
		{
			if(nRoleRank >= nSrcRoleRank && nDestVipLevel >= nSrcVipLevel)
			{
				WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d, nSrcVipLevel=%d, nDestVipLevel=%d}",nKickRoleID,nSrcRoleRank,nRoleRank,nSrcVipLevel,nDestVipLevel);
				nKickResult = enmKickResult_NO_Permission;
				return E_RS_NOPERMISSIONS;
			}
		}
	}
	//其他操作:从房间队列删除、加入被踢队列
	uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
	uint32_t nEndTime = pRoom->GetEndKickTime(nKickRoleID);
	if(nEndTime!=0)
	{
		WRITE_ERROR_LOG("kick player filed because  is did in kicklist{nKickRoleID=%d}",nKickRoleID);
		return ret;
	}
	if(!(pPlayer->IsReboot()))
	{
		ret = pRoom->AddKickList(nKickRoleID,nCurTime+nTime,strReason.GetString());
		if(ret < 0)
		{
			WRITE_ERROR_LOG("kick player filed because add to kicklist error{nKickRoleID=%d}",nKickRoleID);
			return ret;
		}
	}
	nKickResult = enmKickResult_OK;
	return ret;
}

/*
 * 发送通知到相关server
 */

int32_t CKickUserEvent::SendNotifyToServer(CRoom *pRoom,RoomID nRoomID,RoleID nRoleID,CPlayer *pPlayer)
{
	CExitRoomNotice stExitRoomNoti;
	stExitRoomNoti.nRoleID = nRoleID;
	stExitRoomNoti.nRoomID = nRoomID;
	stExitRoomNoti.nRoomType = pRoom->GetRoomType();

	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Item);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Hall);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_RoomDispatcher);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Tunnel,pPlayer->GetConnInfo().nServerID);

	//realcode
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media, pPlayer->GetConnInfo().nServerID);

	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Task);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_HallDataCenter);

	return S_OK;
}
/*
 * 发送通知给user
 */
int32_t CKickUserEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,KickType nKickType,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen , const void *pOptionData )
{
	CKickUserNoti stKickUserNoti;
	stKickUserNoti.nSrcRoleID = nSrcRoleID;
	stKickUserNoti.nKickAllVisitor = nKickType;
	stKickUserNoti.nKickRoleID = nDestRoleID;
	stKickUserNoti.strReason = strReason;
	stKickUserNoti.nTime = nTime;
	//发送给除发起请求的玩家
	SendMessageNotifyToClient(MSGID_RSCL_KICK_USER_NOTI, &stKickUserNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send kick user notify");
	return S_OK;
}

/*
 * 发送响应到客户端
 */
int32_t CKickUserEvent::SendResponseToClient(MessageHeadSS *pMsgHead,KickResult nKickResult,const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	CKickUserResp stKickUserResp;
	stKickUserResp.nKickResult = nKickResult;
	SendMessageResponse(MSGID_RSCL_KICK_USER_RESP, pMsgHead, &stKickUserResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END
