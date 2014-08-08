/*
 *  bll_event_lockip.cpp
 *
 *  To do��
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
	//û�д����
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nSrcRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{ret=0x%08x}\n",ret);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nSrcRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	//�жϱ���������
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pLockIpReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//û�д����
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("add black player does no exist{nDestRoleID=%d}",pLockIpReq->nDestRoleID);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pLockIpReq->nDestRoleID)))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pLockIpReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//����������û�з�IP��Ȩ�ޣ����ܲ��У�
	if(nSrcRoleRank <= enmRoleRank_Host)
	{
		// todo ����Ȩ�޲�����Ӧ
		SendResponseToUser(pMsgHead,enmLockIpResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}

	uint32_t nEndLockTime = (uint32_t)CTimeValue::CurrentTime().Seconds()+pLockIpReq->nTime;
	LockIpResult nLockIpResult = enmLockIpResult_Umknown;
	ret = LockPlayerIP(pRoom,nEndLockTime,nSrcRoleRank,pLockIpReq->nDestRoleID,nLockIpResult);
	//����֪ͨ�ͻ�Ӧ
	SendResponseToUser(pMsgHead,nLockIpResult,nOptionLen,pOptionData);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("lock player ip error{nDestRoleID=%d, ret=0x%08x}",pLockIpReq->nDestRoleID,ret);
		return ret;
	}
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pLockIpReq->nDestRoleID,pLockIpReq->strReason,pLockIpReq->nTime,nOptionLen,pOptionData);
	//�˳�����
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
 *  �����IP
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
	//û�б� �����
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
 * ����֪ͨ������������
 */
int32_t CLockIPEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen , const void *pOptionData )
{
	CLockIpNoti stLockIpNoti;
	stLockIpNoti.nSrcRoleID = nSrcRoleID;
	stLockIpNoti.nDestRoleID = nDestRoleID;
	stLockIpNoti.strReason = strReason;
	stLockIpNoti.nTime = nTime;

	//���͸���������Ե�������
	SendMessageNotifyToClient(MSGID_RSCL_LOCK_IP_NOTI, &stLockIpNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send lockIP  notify");
	return S_OK;
}
/*
 * ������Ӧ���ͻ���
 */
int32_t CLockIPEvent::SendResponseToUser(MessageHeadSS *pMsgHead,LockIpResult nLockIpResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CLockIpResp stLockIpResp;
	stLockIpResp.nLockIpResult = nLockIpResult;
	SendMessageResponse(MSGID_RSCL_LOCK_IP_RESP, pMsgHead, &stLockIpResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
