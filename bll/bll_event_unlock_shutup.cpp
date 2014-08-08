/*
 *  bll_event_unlock_shutup.cpp
 *
 *  To do��
 *  Created on: 	2012-1-11
 *  Author: 		luocj
 */

#include "bll_event_unlock_shutup.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUnLockShutUpEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CUnLockShutUpEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CUnLockShutUpEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_UNLOCK_SHUT_UP_REQ:
			OnEventUnLockShutUp(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CUnLockShutUpEvent::OnEventUnLockShutUp(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
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
	//û�д����
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoom=%d, ret=0x%08x}\n",ret);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	CUnLockShutUPReq *pUnLOckShutUPReq = dynamic_cast<CUnLockShutUPReq *>(pMsgBody);
	if(pUnLOckShutUPReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pUnLOckShutUPReq=0x%08x, nRoleID=%d}\n",pUnLOckShutUPReq,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("unlock shut up:rcev player req{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pUnLOckShutUPReq->nDestRoleID);
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//��ʱ����Ա���²����ý����ֹ���ԵĹ���
	if(nSrcRoleRank < enmRoleRank_TempAdmin)
	{
		// todo ����Ȩ�޲�����Ӧ
		WRITE_NOTICE_LOG("unlock shut up:player no permission{nRoleID=%d, nSrcRoleRank=%d}",pMsgHead->nRoleID,nSrcRoleRank);
		SendResponseToUser(pMsgHead,enmUnLockShutUpResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}

	//�����ֹ���ԵĲ���
	ret = UnLockShutUpPlayer(pRoom,nSrcRoleRank,pUnLOckShutUPReq->nDestRoleID);
	if(ret == (int32_t)E_RS_NOPERMISSIONS)
	{
		// todo ����Ȩ�޲�����Ӧ
		SendResponseToUser(pMsgHead,enmUnLockShutUpResult_NO_Permission,nOptionLen,pOptionData);
		return ret;
	}
	else if(ret < 0)
	{
		//todo ���ʹ����Ӧ
		SendResponseToUser(pMsgHead,enmUnLockShutUpResult_Umknown,nOptionLen,pOptionData);
		return ret;
	}
	//����֪ͨ�ͻ�Ӧ
	SendResponseToUser(pMsgHead,enmUnLockShutUpResult_OK,nOptionLen,pOptionData);
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pUnLOckShutUPReq->nDestRoleID,nOptionLen,pOptionData);

	return ret;
}
/*
 *  ����Ҵӽ���listɾ��
 */
int32_t CUnLockShutUpEvent::UnLockShutUpPlayer(CRoom *pRoom,RoleRank nSrcRoleRank,RoleID nUnLockShutUpRoleID)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nUnLockShutUpRoleID, pPlayer, nPlayerIndex);
	//û�б���� �������
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nUnLockShutUpRoleID=%d}",nUnLockShutUpRoleID);
		return ret;
	}
	//�жϱ���� ������ҵ�RoleRank
	RoleRank nRoleRank = pRoom->GetRoleRank(nUnLockShutUpRoleID);
	if(nRoleRank >= nSrcRoleRank)
	{
		WRITE_ERROR_LOG("unlock shut up:filed because no permissions{nSrcRoleRank=%d, nDestRoleRank=%d}",nUnLockShutUpRoleID);
		return E_RS_NOPERMISSIONS;
	}
	if(!(pRoom->IsInProhibitList(nUnLockShutUpRoleID)))
	{
		WRITE_ERROR_LOG("unlock shut up:filed because because did not shut up{nUnLockShutUpRoleID=%d}",nUnLockShutUpRoleID);
		return E_RS_NOTINSHUTUPLIST;
	}
    //�ӽ����б�ɾ��
	ret = pRoom->DelProhibitList(nUnLockShutUpRoleID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("unlock shut up:filed because  delete from  ProhibitList error{nUnLockShutUpRoleID=%d}",nUnLockShutUpRoleID);
		return ret;
	}
	return ret;
}

/*
 * ����֪ͨ������������
 */
int32_t CUnLockShutUpEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen , const void *pOptionData )
{
	CUnLockShutUpNoti stUnLockShutUpNoti;
	stUnLockShutUpNoti.nSrcRoleID = nSrcRoleID;
	stUnLockShutUpNoti.nDestRoleID = nDestRoleID;

	//���͸���������Ե�������
	SendMessageNotifyToClient(MSGID_RSCL_UNLOCK_SHUT_UP_NOTI, &stUnLockShutUpNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send shut up user  notify");
	return S_OK;
}
/*
 * ������Ӧ���ͻ���
 */
int32_t CUnLockShutUpEvent::SendResponseToUser(MessageHeadSS *pMsgHead,UnLockShutUpResult nUnLockShutUpResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CUnLockShutUpResp stUnLockShutUpResp;
	stUnLockShutUpResp.nUnLockShutUpResult = nUnLockShutUpResult;
	SendMessageResponse(MSGID_RSCL_UNLOCK_SHUT_UP_RESP, pMsgHead, &stUnLockShutUpResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
