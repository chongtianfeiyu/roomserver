/*
 *  bll_event_shutup.cpp
 *
 *  To do：
 *  Created on: 	2012-1-11
 *  Author: 		luocj
 */

#include "bll_event_shutup.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CShutUpEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CShutUpEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CShutUpEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_SHUT_UP_REQ:
		OnEventShutUp(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CShutUpEvent::OnEventShutUp(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("shut up:player does not exist{nRoleiD=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("shut up:get room error!{nRoomID=%d, ret=0x%08x}\n",ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("shut up:player does not in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	CShutUPReq *pShutUPReq = dynamic_cast<CShutUPReq *>(pMsgBody);
	if(pShutUPReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pShutUPReq=0x%08x}\n",pShutUPReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("shut up:rcev player req{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pShutUPReq->nDestRoleID);
	//判断被封玩家情况
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pShutUPReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//没有此玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("shut up:player does not exist{nDestRoleID=%d}",pShutUPReq->nDestRoleID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex))&&!(pRoom->IsRebotPlayerInRoom(pShutUPReq->nDestRoleID)))
	{
		WRITE_ERROR_LOG("shut up:player does not in room{nDestRoleID=%d, nRoomID=%d}",pShutUPReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//临时管理员以下不能用禁止发言的功能
	if(nSrcRoleRank < enmRoleRank_TempAdmin)
	{
		// todo 发送权限不够回应
		WRITE_ERROR_LOG("shut up:player no permission{nRoleID=%d, nRoleRank=%d}",pMsgHead->nRoleID,nSrcRoleRank);
		SendResponseToUser(pMsgHead,enmShutUpResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}

	//禁止发言的操作
	ret = ShutUpPlayer(pRoom,nSrcRoleRank,pShutUPReq->nDestRoleID);
	if(ret == (int32_t)E_RS_NOPERMISSIONS)
	{
		// todo 发送权限不够回应
		SendResponseToUser(pMsgHead,enmShutUpResult_NO_Permission,nOptionLen,pOptionData);
		return ret;
	}
	else if(ret<0)
	{
		//todo 发送错误回应
		SendResponseToUser(pMsgHead,enmShutUpResult_Umknown,nOptionLen,pOptionData);
		return ret;
	}
	//发送通知和回应
	SendResponseToUser(pMsgHead,enmShutUpResult_OK,nOptionLen,pOptionData);
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pShutUPReq->nDestRoleID,nOptionLen,pOptionData);

	WRITE_NOTICE_LOG("shut up:shut up ok{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pShutUPReq->nDestRoleID);
	return ret;
}
/*
 *  把玩家加入禁言list
 */
int32_t CShutUpEvent::ShutUpPlayer(CRoom *pRoom,RoleRank nSrcRoleRank,RoleID nShutUpRoleID)
{
	int32_t ret = S_OK;
	if(NULL==pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nShutUpRoleID, pPlayer, nPlayerIndex);
	//没有被 加禁止发言的玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("shut up:player does no exist{nDestRoleID=%d}",nShutUpRoleID);
		return ret;
	}
	//判断被禁止发言玩家的RoleRank
	RoleRank nRoleRank = pRoom->GetRoleRank(nShutUpRoleID);
	if(nRoleRank >= nSrcRoleRank)
	{
		WRITE_ERROR_LOG("shut up:player no permission{nDestRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d}",nShutUpRoleID,nSrcRoleRank,nRoleRank);
		return E_RS_NOPERMISSIONS;
	}
	//加入禁言列表
	RoleID nDelRoleID = enmInvalidRoleID;
	ret = pRoom->AddProhibitList(nShutUpRoleID,nDelRoleID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("shut up:filed because add to list error{nDestRoleID=%d}",nShutUpRoleID);
		return ret;
	}
	//当达到禁言列表最大值的时候，会顶掉第一个
	if(nDelRoleID != enmInvalidRoleID)
	{
		CUnLockShutUpNoti stUnLockShutUpNoti;
		stUnLockShutUpNoti.nSrcRoleID = enmInvalidRoleID;
		stUnLockShutUpNoti.nDestRoleID = nDelRoleID;
		SendMessageNotifyToClient(MSGID_RSCL_UNLOCK_SHUT_UP_NOTI, &stUnLockShutUpNoti, pRoom->GetRoomID(), enmBroadcastType_All,
				nDelRoleID,0,NULL, "send unshut up user  notify");
	}
	return ret;
}

/*
 * 发送通知给房间里的玩家
 */
int32_t CShutUpEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen , const void *pOptionData )
{
	CShutUpNoti stShutUpNoti;
	stShutUpNoti.nSrcRoleID = nSrcRoleID;
	stShutUpNoti.nDestRoleID = nDestRoleID;
	stShutUpNoti.nLeftTime = g_SomeConfig.GetShutUpTime();

	//发送给除发起禁言的其他人
	SendMessageNotifyToClient(MSGID_RSCL_SHUT_UP_NOTI, &stShutUpNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send shut up user  notify");
	return S_OK;
}
/*
 * 发送响应到客户端
 */
int32_t CShutUpEvent::SendResponseToUser(MessageHeadSS *pMsgHead,ShutUpResult nShutUpResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CShutUpResp stShutUpResp;
	stShutUpResp.nShutUpResult = nShutUpResult;
	SendMessageResponse(MSGID_RSCL_SHUT_UP_RESP, pMsgHead, &stShutUpResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

