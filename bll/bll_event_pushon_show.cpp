/*
 *  bll_event_pushon_show.cpp
 *
 *  To do：
 *  Created on: 	2012-2-7
 *  Author: 		luocj
 */

#include "bll_event_pushon_show.h"
#include "requestsong/bll_timer_clearroomorderinfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CPushOnShowEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CPushOnShowEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CPushOnShowEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_PUSH_ON_SHOW_REQ:
		OnEventPushOnShow(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		case MSGID_RSCL_ANSWER_PULL_SHOW_REQ:
		OnEventAnswerPushOnShow(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CPushOnShowEvent::OnEventPushOnShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("push on show:player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("push on show:get room error!{nRoomID=%d, ret=0x%08x}\n",ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("push on show:player does not in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	CPushOnShowReq *pPushOnShowReq = dynamic_cast<CPushOnShowReq *>(pMsgBody);
	if(pPushOnShowReq == NULL)
	{
		WRITE_ERROR_LOG("push on show:pPushOnShowReq null{nRoleID=%d, nRoomId=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("push on show:rcev req{nRoleID=%d, nRoomID=%d, nDestRoleID=%d, nMicIndex=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pPushOnShowReq->nDestID,pPushOnShowReq->nIndex);
	//判断被封玩家情况
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pPushOnShowReq->nDestID, pDestPlayer, nDestPlayerIndex);
	//没有此玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("push on show:player does no exist{nDestRoleID=%d}",pPushOnShowReq->nDestID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pPushOnShowReq->nDestID)))
	{
		WRITE_ERROR_LOG("push on show:player does not in room{nDestRoleID=%d, nRoomID=%d}",pPushOnShowReq->nDestID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//管理员以下不能邀请上麦
	if(nSrcRoleRank < enmRoleRank_Admin)
	{
		// todo 发送权限不够回应
		WRITE_NOTICE_LOG("push on show:player has no permission push on show{nRoelID=%d, nSrcRoleRank=%d}\n",pMsgHead->nRoleID,nSrcRoleRank);
		SendResponseToUser(pMsgHead,enmPushResult_No_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
	RoleRank nDestRoleRank = pRoom->GetRoleRank(pPushOnShowReq->nDestID);
	VipLevel nVipLevel = pDestPlayer->GetVipLevel();
	//下对上(平级也不可以操作)，游客不能被邀请
	if(((nDestRoleRank >= nSrcRoleRank) && (pPushOnShowReq->nDestID != pMsgHead->nRoleID)) || nVipLevel <= enmVipLevel_NONE)
	{
		// todo 发送权限不够回应
		WRITE_NOTICE_LOG("push on show:player has no permission push on show{nRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d, nVipLevel=%d}\n",pMsgHead->nRoleID,nSrcRoleRank,nDestRoleRank,nVipLevel);
		SendResponseToUser(pMsgHead,enmPushResult_No_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
	//如果被邀请的玩家是机器人，有麦，则提示应经在麦上
	if(pDestPlayer->IsReboot() && pDestPlayer->IsHasCamera())
	{
		WRITE_NOTICE_LOG("push on show:player is robot and has Camera!{RoleID=%d}\n",pPushOnShowReq->nDestID);
		SendResponseToUser(pMsgHead,enmPushResult_IS_ONMIC,nOptionLen,pOptionData);
		return E_RS_ISONMIC;
	}
	//判断玩家是否在麦上
	if(pDestPlayer->IsOnMic())
	{
		//发送在麦上通知
		WRITE_NOTICE_LOG("push on show:player is on mic{nDestRoleID=%d}\n",pPushOnShowReq->nDestID);
		SendResponseToUser(pMsgHead,enmPushResult_IS_ONMIC,nOptionLen,pOptionData);
		return E_RS_ISONMIC;
	}
	//判断是否需要同意的
	if((pDestPlayer->GetVipLevel() >= enmVipLevel_Earl || pDestPlayer->IsOfficialPlayer() || (pDestPlayer->GetIdentityType() & enmIdentityType_AGENT)) && (pPushOnShowReq->nDestID != pMsgHead->nRoleID))
	{
		WRITE_NOTICE_LOG("push on show:player is need answer{nDestRoleID=%d}\n",pPushOnShowReq->nDestID);
		SendResponseToUser(pMsgHead,enmPushResult_Need_Answer,nOptionLen,pOptionData);
		//通知被邀请者
		SendNotifyToUser(pMsgHead,pMsgHead->nRoomID,pMsgHead->nRoleID,pDestPlayer->GetRoleID(),pPushOnShowReq->nIndex,enmNotifyType_NeedAnswer);
		return ret;
	}
	ret = PushPlayerOnShow(pMsgHead,pRoom,pMsgHead->nRoleID,pDestPlayer,pPushOnShowReq->nIndex);
	return ret;
}

int32_t CPushOnShowEvent::OnEventAnswerPushOnShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	//在此函数中DestPlayer为被邀请的玩家，srcPlayer为邀请者
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("push on show:get room error!{nRoomID=%d, ret=0x%08x}\n",ret);
		return ret;
	}

	CAnswerPullOnShowReq *pAnswerPullOnShowReq = dynamic_cast<CAnswerPullOnShowReq *>(pMsgBody);
	if(pAnswerPullOnShowReq == NULL)
	{
		WRITE_ERROR_LOG("push on show:pAnswerPullOnShowReq null{nRoleID=%d, nRoomId=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG(" answer push on show:rcev req{nDestRoleID=%d, nRoomID=%d, nSrcRoleID=%d, nMicIndex=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pAnswerPullOnShowReq->nInviteRoleID,pAnswerPullOnShowReq->nMicIndex);
	//判断被封玩家情况
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pDestPlayer, nDestPlayerIndex);
	//没有此玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("push on show:player does no exist{nDestRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)))
	{
		WRITE_ERROR_LOG("push on show:player does not in room{nDestRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//如果是拒绝直接通知邀请者
	if(pAnswerPullOnShowReq->nAnswerType == enmAnswerType_REJECT)
	{
		WRITE_NOTICE_LOG("answer push on show:player is reject invite{nInviteRoleID=%d,nDestRoleID=%d}\n",pAnswerPullOnShowReq->nInviteRoleID,pMsgHead->nRoleID);
		SendResponseToUser(pMsgHead,enmAnswerPullResult_OK,nOptionLen,pOptionData);
		SendNotifyToUser(pMsgHead,pMsgHead->nRoomID,pAnswerPullOnShowReq->nInviteRoleID,pDestPlayer->GetRoleID(),pAnswerPullOnShowReq->nMicIndex,enmNotifyType_Reject);
		return ret;
	}
	//判断玩家是否在麦上
	if(pDestPlayer->IsOnMic())
	{
		//发送在麦上通知
		WRITE_NOTICE_LOG("push on show:player is on mic{nDestRoleID=%d}\n",pMsgHead->nRoleID);
		SendResponseToUser(pMsgHead,enmAnswerPullResult_IS_ONMIC,nOptionLen,pOptionData);
		return E_RS_ISONMIC;
	}
	ret = PushPlayerOnShow(pMsgHead,pRoom,pAnswerPullOnShowReq->nInviteRoleID,pDestPlayer,pAnswerPullOnShowReq->nMicIndex);
	return ret;
}

int32_t CPushOnShowEvent::PushPlayerOnShow(MessageHeadSS *pMsgHead,CRoom *pRoom, RoleID nSrcRoleID,CPlayer* pDestPlayer, int32_t nMicIndex)
{
	int32_t ret = S_OK;
	//加到公卖上
	ret = pRoom->AddOnPublicMicInIndex(pDestPlayer->GetRoleID(),nMicIndex);
	if(ret<0)
	{
		//此卖被关闭
		if(ret == (int32_t)E_RS_PUBLICISLOCKED)
		{
			WRITE_NOTICE_LOG("push on show:mic is locked{nRoomID=%d, nMicIndex=%d}\n",pRoom->GetRoomID(),nMicIndex);
			if(pMsgHead->nMessageID == MSGID_CLRS_PUSH_ON_SHOW_REQ)
			{
				SendResponseToUser(pMsgHead,enmPushResult_Mic_IsLocked);
			}
			else
			{
				SendResponseToUser(pMsgHead,enmAnswerPullResult_MIC_ISLOCKED);
			}

			return ret;
		}
		if((ret == (int32_t)E_RS_PUBLICISFULL || ret == (int32_t)E_RS_PUBLICINDEXISEMPTY)
				&& pMsgHead->nMessageID == MSGID_RSCL_ANSWER_PULL_SHOW_REQ)
		{
			WRITE_NOTICE_LOG("answer push on show:mic has player{nRoomID=%d, nMicIndex=%d}\n",pRoom->GetRoomID(),nMicIndex);
			SendResponseToUser(pMsgHead,enmAnswerPullResult_MIC_HASPLAYER);
			return ret;
		}
		WRITE_ERROR_LOG("push on show:mic full or index is not empty {nRoomID=%d, nMicIndex=%d}\n",pMsgHead->nRoomID,nMicIndex);
		SendResponseToUser(pMsgHead,enmPushResult_UNKNOWN);
		return ret;
	}
	//如果在本房间的麦序里面，则删除
	PlayerState nPlayerState = pDestPlayer->GetStateInRoom(pMsgHead->nRoomID);
	if(nPlayerState == enmPlayerState_PublicMicQueue)
	{
		pRoom->RemoveFromWaittingMic(pDestPlayer->GetRoleID());
	}

	ret = pDestPlayer->SetStateInRoom(enmPlayerState_PublicMic,pRoom->GetRoomID());
	if(ret<0)
	{
		uint8_t nMicIndex = 0;
		pRoom->RemoveOnPublicMic(pDestPlayer->GetRoleID(),nMicIndex);
		WRITE_ERROR_LOG("push on show:player does not in room{nDestRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pRoom->GetRoomID());
		SendResponseToUser(pMsgHead,enmPushResult_UNKNOWN);
		return ret;
	}
	ret = pDestPlayer->SetPublicMicIndexInRoom(nMicIndex,pRoom->GetRoomID());
	if(ret<0)
	{
		uint8_t nMicIndex = 0;
		pRoom->RemoveOnPublicMic(pDestPlayer->GetRoleID(),nMicIndex);
		WRITE_ERROR_LOG("push on show:player does not in room{nRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pRoom->GetRoomID());
		SendResponseToUser(pMsgHead,enmPushResult_UNKNOWN);
		return ret;
	}
	//发送成功回应给客户端
	SendResponseToUser(pMsgHead,enmPushResult_OK);
	WRITE_NOTICE_LOG("push on show:player push On PublicMic{nRoleID=%d, nRoomID=%d, nDestRoleID=%d, nMicIndex=%d}",nSrcRoleID,pMsgHead->nRoomID,pDestPlayer->GetRoleID(),nMicIndex);
	if(!(pDestPlayer->IsReboot()))
	{
//		SendPlayerOnMicToMedia(pRoom,pDestPlayer);
		SendPlayerMicStatusToDB(pRoom,pDestPlayer,enmMicOperateType_Public_Up,pMsgHead->nMessageID,0);
		SendNotifyToMediaServer(pMsgHead,pDestPlayer->GetRoleID(),pRoom->GetMediaServerIDByType(enmNetType_Tel),pRoom->GetMediaServerIDByType(enmNetType_Cnc));
	}
	SendNotifyToUser(pMsgHead,pMsgHead->nRoomID,nSrcRoleID,pDestPlayer->GetRoleID(),nMicIndex,enmNotifyType_Connect);

	//发送上麦通知到hallserver
	pDestPlayer->SetPlayerGlobalState(enmPlayerState_PublicMic);

	//删除歌手的订单计时器
	g_ClearRoomOrderInfo.DeleteTimer(pDestPlayer->GetRoleID());

	COnMicNoti stOnMicNoti;
	stOnMicNoti.nOnMicRoleID = pDestPlayer->GetRoleID();
	stOnMicNoti.nMicType = enmMicType_PublicMic;
	SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pDestPlayer->GetRoleID(), enmEntityType_Hall);
	return ret;
}
/*
 * 发送通知给房间里的玩家
 */
int32_t CPushOnShowEvent::SendNotifyToUser(MessageHeadSS *pMsgHead,RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,uint8_t nIndex,NotifyType nNotifyType,const uint16_t nOptionLen , const void *pOptionData )
{
	CPushOnShowNoti stPushOnShowNoti;
	stPushOnShowNoti.nSrcRoleID = nSrcRoleID;
	stPushOnShowNoti.nDestRoleID = nDestRoleID;
	stPushOnShowNoti.nNotifyType = nNotifyType;
	stPushOnShowNoti.nIndex = nIndex;

	RoleID nNotiRoleID = nSrcRoleID;
	BroadcastType nBroadcastType = enmBroadcastType_ExceptPlayr;

	if(nNotifyType == enmNotifyType_NeedAnswer)
	{
		//给被邀请者发送需要确认的通知
		nNotiRoleID = nDestRoleID;
		nBroadcastType = enmBroadcastType_ExpectPlayr;
	}
	else if(nNotifyType == enmNotifyType_Reject)
	{
		//给邀请者发送对方拒绝邀请的通知
		nNotiRoleID = nSrcRoleID;
		nBroadcastType = enmBroadcastType_ExpectPlayr;
	}
	else if(nNotifyType == enmNotifyType_Connect && pMsgHead->nMessageID == MSGID_RSCL_ANSWER_PULL_SHOW_REQ)
	{
		nNotiRoleID = nDestRoleID;
		nBroadcastType = enmBroadcastType_All;
	}

	//发送给除发起的其他人
	SendMessageNotifyToClient(MSGID_RSCL_PUSH_ON_SHOW_NOTI, &stPushOnShowNoti, nRoomID, nBroadcastType,
			nNotiRoleID,nOptionLen,(char*)pOptionData, "send push on show user notify");
	return S_OK;
}
/*
 * 发送响应到客户端
 */
int32_t CPushOnShowEvent::SendResponseToUser(MessageHeadSS *pMsgHead,PushResult nPushResult,const uint16_t nOptionLen , const void *pOptionData)
{
	if(pMsgHead->nMessageID == MSGID_CLRS_PUSH_ON_SHOW_REQ)
	{
		CPushOnShowResp stPushOnShowResp;
		stPushOnShowResp.nPushResult = nPushResult;
		SendMessageResponse(MSGID_RSCL_PUSH_ON_SHOW_RESP, pMsgHead, &stPushOnShowResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	}
	else
	{
		CAnswerPullOnShowResp stAnswerPullOnShowResp;
		stAnswerPullOnShowResp.nAnswerPullResult = nPushResult;
		SendMessageResponse(MSGID_RSCL_ANSWER_PULL_SHOW_RESP, pMsgHead, &stAnswerPullOnShowResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	}
	return S_OK;
}

int32_t CPushOnShowEvent::SendNotifyToMediaServer(MessageHeadSS *pMsgHead,RoleID nPushRoleID,int32_t nDestTelMediaID,int32_t nDestCncMediaID)
{
	int32_t ret = S_OK;
	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	int32_t nPlayerCount = 0;
	ret = g_DataCenter.GetAllPlayers(pMsgHead->nRoomID, arrPlayer, MaxUserCountPerRoom, nPlayerCount);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("get room players object error{RoomID=%d, ret=0x%08x}", pMsgHead->nRoomID, ret);
		return ret;
	}

	//发送通知消息到mediaServer
	CPlayer *pSpeaker = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(nPushRoleID, pSpeaker, nPlayerIndex);
	if(nRet != S_OK || pSpeaker == NULL)
	{
		return S_FALSE;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room object error!{nRoomID=%d, ret=0x%08x}\n", pMsgHead->nRoomID, ret);
		return ret;
	}

	g_BllBase.SendAddTransmitUserNotifyToMediaServer(pSpeaker, arrPlayer, nPlayerCount, pRoom, true);
	return S_OK;

//	CAddTransmitUserNotice stAddTransmitUserNotice;
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nRoleID = nPushRoleID;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.nRoomID = pMsgHead->nRoomID;
//	stAddTransmitUserNotice.arrAddRoleID[0] = nPushRoleID;
//	for(int32_t i = 0;i<nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//           continue;
//		}
//		if(nPushRoleID!=arrPlayer[i]->GetRoleID()&&arrPlayer[i]->GetPlayerNetType()==enmNetType_Tel)
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_Broadcast,pMsgHead->nRoleID,enmEntityType_Media,nDestTelMediaID);
//	stAddTransmitUserNotice.nAddCount = 0;
//	for(int32_t i = 0;i<nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//			continue;
//		}
//		if(nPushRoleID!=arrPlayer[i]->GetRoleID()&&arrPlayer[i]->GetPlayerNetType()==enmNetType_Cnc)
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_Broadcast,nPushRoleID,enmEntityType_Media,nDestCncMediaID);
//	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END

