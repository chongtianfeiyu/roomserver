/*
 *  bll_event_lockip.cpp
 *
 *  To do：
 *  Created on: 	2012-1-12
 *  Author: 		luocj
 */

#include "bll_event_lockip.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CLockIPEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CLockIPEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CLockIPEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_LOCK_IP_REQ:
		OnEventLockIP(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CLockIPEvent::OnEventLockIP(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CLockIpReq *pLockIpReq = dynamic_cast<CLockIpReq *>(pMsgBody);
	if(pLockIpReq == NULL)
	{
		WRITE_ERROR_LOG("pLockIpReq null  ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player lock other ip req{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pLockIpReq->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nSrcRoleID=%d}",pMsgHead->nRoleID);
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
		WRITE_ERROR_LOG("player does no in room{nSrcRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	//判断被封玩家情况
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pLockIpReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//没有此玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("add black player does no exist{nDestRoleID=%d}",pLockIpReq->nDestRoleID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pLockIpReq->nDestRoleID)))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pLockIpReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//室主及以下没有封IP的权限（超管才有）
	if(nSrcRoleRank <= enmRoleRank_Host)
	{
		// todo 发送权限不够回应
		SendResponseToUser(pMsgHead,enmLockIpResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}

	uint32_t nEndLockTime = (uint32_t)CTimeValue::CurrentTime().Seconds()+pLockIpReq->nTime;
	LockIpResult nLockIpResult = enmLockIpResult_Umknown;
	ret = LockPlayerIP(pRoom,nEndLockTime,nSrcRoleRank,pLockIpReq->nDestRoleID,nLockIpResult);
	//发送通知和回应
	SendResponseToUser(pMsgHead,nLockIpResult,nOptionLen,pOptionData);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("lock player ip error{nDestRoleID=%d, ret=0x%08x}",pLockIpReq->nDestRoleID,ret);
		return ret;
	}
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pLockIpReq->nDestRoleID,pLockIpReq->strReason,pLockIpReq->nTime,nOptionLen,pOptionData);
	//退出房间
	ret = ExitRoom(pDestPlayer,nDestPlayerIndex,pRoom,pMsgHead->nMessageID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("add player to lockIP because exit room filed{nDestRoleID=%d, ret=0x%08x}",pLockIpReq->nDestRoleID,ret);
		return ret;
	}
	WRITE_NOTICE_LOG("player lock other ip {nSrcRoleID=%d, nRoomID=%d, nDestRoleID=%d, nTime=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pLockIpReq->nDestRoleID,pLockIpReq->nTime);
	return ret;
}
/*
 *  封玩家IP
 */
int32_t CLockIPEvent::LockPlayerIP(CRoom *pRoom,const uint32_t nEndLockTime,RoleRank nSrcRoleRank,RoleID nLockIPRoleID,LockIpResult &nLockIpResult)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("pRoom null ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nLockIPRoleID, pPlayer, nPlayerIndex);
	//没有被 封玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nLockIPRoleID=%d}",nLockIPRoleID);
		return ret;
	}
	if(!(pPlayer->IsReboot()))
	{
		ret = pRoom->AddLockIPList(pPlayer->GetConnInfo().nAddress,nEndLockTime);
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to lockIP filed because  add to lockIpList error{nLockIPRoleID=%d}",nLockIPRoleID);
			return ret;
		}
	}
	nLockIpResult = enmLockIpResult_OK;
	return ret;
}

/*
 * 发送通知给房间里的玩家
 */
int32_t CLockIPEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen , const void *pOptionData )
{
	CLockIpNoti stLockIpNoti;
	stLockIpNoti.nSrcRoleID = nSrcRoleID;
	stLockIpNoti.nDestRoleID = nDestRoleID;
	stLockIpNoti.strReason = strReason;
	stLockIpNoti.nTime = nTime;

	//发送给除发起禁言的其他人
	SendMessageNotifyToClient(MSGID_RSCL_LOCK_IP_NOTI, &stLockIpNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send lockIP  notify");
	return S_OK;
}
/*
 * 发送响应到客户端
 */
int32_t CLockIPEvent::SendResponseToUser(MessageHeadSS *pMsgHead,LockIpResult nLockIpResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CLockIpResp stLockIpResp;
	stLockIpResp.nLockIpResult = nLockIpResult;
	SendMessageResponse(MSGID_RSCL_LOCK_IP_RESP, pMsgHead, &stLockIpResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
