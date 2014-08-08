/*
 *  bll_event_pulloff_show.cpp
 *
 *  To do��
 *  Created on: 	2012-2-7
 *  Author: 		luocj
 */

#include "bll_event_pulloff_show.h"
#include "requestsong/bll_timer_clearroomorderinfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CPullOffShowEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CPullOffShowEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CPullOffShowEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_PULL_OFF_SHOW_REQ:
		OnEventPullOffShow(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CPullOffShowEvent::OnEventPullOffShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//û�д����
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("push off show:player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("push off show:get room error{nRoomID=%d, ret=0x%08x}\n",ret);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("push off show:player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	CPullOffShowReq *pPullOffShowReq = dynamic_cast<CPullOffShowReq *>(pMsgBody);
	if(pPullOffShowReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pPullOffShowReq=0x%08x, nMsgID=%d, nRoleID=%d}\n",E_NULLPOINTER,MSGID_CLRS_PULL_OFF_SHOW_REQ,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("push off show:rcev player req{nRoleID=%d, nDestRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pPullOffShowReq->nDestID,pMsgHead->nRoomID);
	//�жϱ���������
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pPullOffShowReq->nDestID, pDestPlayer, nDestPlayerIndex);
	//û�д����
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("push off show:player is pushed does no exist{nDestRoleID=%d}",pPullOffShowReq->nDestID);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex))&&!(pRoom->IsRebotPlayerInRoom(pPullOffShowReq->nDestID)))
	{
		WRITE_ERROR_LOG("push off show:player does no in room{nDestRoleID=%d, nRoomID=%d}",pPullOffShowReq->nDestID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//��ʱ����Ա���²����±��˵���
	if(nSrcRoleRank < enmRoleRank_TempAdmin)
	{
		// todo ����Ȩ�޲�����Ӧ
		WRITE_ERROR_LOG("push off show:player has no permission push off show{nRoleID=%d, nSrcRoleRank=%d}\n",pMsgHead->nRoleID,nSrcRoleRank);
		SendResponseToUser(pMsgHead,enmPullResult_No_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
	RoleRank nDestRoleRank = pRoom->GetRoleRank(pPullOffShowReq->nDestID);
	//�¶��ϣ�ƽ��Ҳ�����Բ��������οͲ��ܲ���
	if(nDestRoleRank >= nSrcRoleRank && pPullOffShowReq->nDestID != pMsgHead->nRoleID)
	{
		// todo ����Ȩ�޲�����Ӧ
		WRITE_ERROR_LOG("push off show:player has no permission push off show{nRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d}\n",pMsgHead->nRoleID,nSrcRoleRank,nDestRoleRank);
		SendResponseToUser(pMsgHead,enmPullResult_No_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
	//�ж�����Ƿ�������
	if(!(pDestPlayer->IsOnMicInRoom(pMsgHead->nRoomID)))
	{
		//����������֪ͨ
		WRITE_ERROR_LOG("push off show:dest player is not on mic{nDestRoleID=%d}\n",pPullOffShowReq->nDestID);
		SendResponseToUser(pMsgHead,enmPullResult_IS_NOT_ONMIC,nOptionLen,pOptionData);
		return E_RS_ISONMIC;
	}
	uint8_t nMicIndex = 0;
	if(!pDestPlayer->IsReboot())
	{
		//ͳ�������micʱ��
		StaticOnMIcTime(pDestPlayer,pRoom);
	}
	//�ӹ���ɾ��
	ret = pRoom->RemoveOnPublicMic(pPullOffShowReq->nDestID,nMicIndex);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("push off show:player is not on public mic{nDestRoleID=%d, nRoomID=%d, ret=0x%8X}\n",pPullOffShowReq->nDestID,pMsgHead->nRoomID,ret);
		SendResponseToUser(pMsgHead,enmPushResult_IS_ONMIC,nOptionLen,pOptionData);
		return ret;
	}
	ret = pDestPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
	if(ret < 0)
	{
		WRITE_ERROR_LOG("push off show:dest player does no in room{nDestRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pRoom->GetRoomID());
		SendResponseToUser(pMsgHead,enmPullResult_UNKNOWN,nOptionLen,pOptionData);
		return ret;
	}
	ret = pDestPlayer->SetPublicMicIndexInRoom(0,pRoom->GetRoomID());
	if(ret < 0)
	{
		WRITE_ERROR_LOG("push off show:dest player does no in room{nDestRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pRoom->GetRoomID());
		SendResponseToUser(pMsgHead,enmPullResult_UNKNOWN,nOptionLen,pOptionData);
		return ret;
	}
	//���ͳɹ���Ӧ���ͻ���
	SendResponseToUser(pMsgHead,enmPullResult_OK,nOptionLen,pOptionData);
	WRITE_NOTICE_LOG("push off show:player pull off PublicMic{nRoleID=%d, nRoomID=%d, nDestRoleID=%d, nMicIndex=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pDestPlayer->GetRoleID(),nMicIndex);
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pPullOffShowReq->nDestID,nOptionLen,pOptionData);
	if(!pDestPlayer->IsReboot())
	{
		SendNotifyToMediaServer(pMsgHead,pPullOffShowReq->nDestID,pRoom->GetMediaServerIDByType(enmNetType_Tel),pRoom->GetMediaServerIDByType(enmNetType_Cnc));
		SendPlayerMicStatusToDB(pRoom,pDestPlayer,enmMicOperateType_Public_Dowm,pMsgHead->nMessageID,0);
	}

	pDestPlayer->SetPlayerGlobalState(enmPlayerState_InRoom);
	COffMicNoti stOffMicNoti;
	stOffMicNoti.nOffMicRoleID = pDestPlayer->GetRoleID();
	stOffMicNoti.nMicType = enmMicType_PublicMic;
	SendMessageNotifyToServer(MSGID_RSHS_OFFMIC_NOTIFY, &stOffMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pDestPlayer->GetRoleID(), enmEntityType_Hall);
	//������������
	WaittingToPublic(pRoom);

	//���������
	IdentityType nIdentityType = pDestPlayer->GetIdentityType();
	if((nIdentityType & enmIdentityType_SONGER) ||
			(nIdentityType & enmIdentityType_DANCER) ||
			(nIdentityType & enmIdentityType_HOST))
	{
		g_ClearRoomOrderInfo.CreateTimer(pMsgHead->nRoomID, pPullOffShowReq->nDestID);
	}

	return ret;
}

/*
 * ����֪ͨ������������
 */
int32_t CPullOffShowEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen , const void *pOptionData )
{
	CPullOffShowNoti stPullOffShowNoti;
	stPullOffShowNoti.nSrcRoleID = nSrcRoleID;
	stPullOffShowNoti.nDestRoleID = nDestRoleID;

	//���͸���������������
	SendMessageNotifyToClient(MSGID_RSCL_PULL_OFF_SHOW_NOTI, &stPullOffShowNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send pull off show user  notify");
	return S_OK;
}
/*
 * ������Ӧ���ͻ���
 */
int32_t CPullOffShowEvent::SendResponseToUser(MessageHeadSS *pMsgHead,PullResult nPullResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CPullOffShowResp stPullOffShowResp;
	stPullOffShowResp.nPullResult = nPullResult;
	SendMessageResponse(MSGID_RSCL_PULL_OFF_SHOW_RESP, pMsgHead, &stPullOffShowResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

int32_t CPullOffShowEvent::SendNotifyToMediaServer(MessageHeadSS *pMsgHead,RoleID nPullOffRoleID,int32_t nDestTelMediaID,int32_t nDestCncMediaID)
{
	if(pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("pMsgHead is null\n");
		return S_FALSE;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("send delete transmit user Error:get room error!{nRoomID=%d, ret=0x%08x}\n", pMsgHead->nRoomID, ret);
		return ret;
	}

    g_BllBase.SendDelTransmitUserNotifyToMediaServer(nPullOffRoleID, pRoom, NULL, 0);

//	CDelTransmitUserNotice stDelTransmitUserNotice;
//	//��Ϊ����ʱnRoleID=arrDelRoleID[0]��nDelCount = 1
//	stDelTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stDelTransmitUserNotice.nRoleID = nPullOffRoleID;
//	stDelTransmitUserNotice.nDelCount = 1;
//	stDelTransmitUserNotice.arrDelRoleID[0] = nPullOffRoleID;
//	stDelTransmitUserNotice.nRoomID = pMsgHead->nRoomID;
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,pMsgHead->nRoomID,enmTransType_P2P,nPullOffRoleID,enmEntityType_Media,nDestTelMediaID);
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,pMsgHead->nRoomID,enmTransType_P2P,nPullOffRoleID,enmEntityType_Media,nDestCncMediaID);
	return S_OK;
}
FRAME_ROOMSERVER_NAMESPACE_END
