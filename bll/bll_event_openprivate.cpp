/*
 *  bll_event_openprivate.cpp
 *
 *  To do��
 *  Created on: 	2011-12-17
 *  Author: 		luocj
 */

#include "bll_event_openprivate.h"
#include "../dal/to_server_message.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t COpenPrivateEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t COpenPrivateEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t COpenPrivateEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_OPEN_PEIVATE_REQ:
		OnEventOpenPrivate(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * ����ĳ�˵�˽������client��    δ�����  ^ ^
 */
int32_t COpenPrivateEvent::OnEventOpenPrivate(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen , const void *pOptionData )
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
		WRITE_ERROR_LOG("null pointer:{pPlayer=0x%08x}\n",pPlayer);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error{ret=0x%08x, nRoomID=%d}\n",ret,pMsgHead->nRoomID);
		return S_OK;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	COpenPrivateMicReq *pOpenPrivateMicReqMsg = dynamic_cast<COpenPrivateMicReq *>(pMsgBody);

	if(pOpenPrivateMicReqMsg == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pOpenPrivateMicReqMsg=0x%08x}\n",pOpenPrivateMicReqMsg);
		return E_NULLPOINTER;
	}

	WRITE_NOTICE_LOG("open private:recv req{nRoleID=%d, nDestRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pOpenPrivateMicReqMsg->nDestRoleID,pMsgHead->nRoomID);
	//��ȡ����˽����ҵĶ���
	CPlayer * pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pOpenPrivateMicReqMsg->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//û�д����
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nDestRoleID=%d}",pOpenPrivateMicReqMsg->nDestRoleID);
		return ret;
	}
	COpenPeivateResp stOpenPeivateResp;
	//�Լ����Լ�ֱ�ӷ���
	if(pOpenPrivateMicReqMsg->nDestRoleID == pMsgHead->nRoleID)
	{
		stOpenPeivateResp.nOpenResult = enmOpenResult_Is_Self;
		SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
		return ret;
	}
	//�鿴��ҵ�״̬�Ƿ���˽����
	if(!(pDestPlayer->IsStateInRoom(enmPlayerState_PrivateMic,pMsgHead->nRoomID)))
	{
		WRITE_ERROR_LOG("player does not on privateMic{nDestRoleID=%d}",pOpenPrivateMicReqMsg->nDestRoleID);
		//todo  ����û����˽���ϵ���Ӧ
		stOpenPeivateResp.nOpenResult = enmOpenResult_NOT_OPEN;
		SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
		return ret;
	}
	//�жϱ��򿪵����Ƿ񱻴򿪹���
	if(pPlayer->PrivateMicIsOpened(pOpenPrivateMicReqMsg->nDestRoleID,pMsgHead->nRoomID))
	{
		WRITE_WARNING_LOG("player is opened{nRoleID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pOpenPrivateMicReqMsg->nDestRoleID);
		stOpenPeivateResp.nOpenResult = enmOpenResult_Is_Opend;
		SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
		return ret;
	}
	bool isOfficePlayer = pPlayer->IsOfficialPlayer();

	//todo (ͨ���ȼ��鿴�����жϿ��Դ򿪵�˽��ĸ�����
	ret = pPlayer->AddWatchPrivateMic(pOpenPrivateMicReqMsg->nDestRoleID,pMsgHead->nRoomID,isOfficePlayer);
	//�������Դ򿪵ĸ���
	if(ret== (int32_t)E_RS_WATCHPRIVATEMICISTOBIG)
	{
		stOpenPeivateResp.nOpenResult = enmOpenResult_Is_Full;
		SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
		return ret;
	}
	if(ret < 0)
	{
		stOpenPeivateResp.nOpenResult = enmOpenResult_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
		return ret;
	}

	//���ͳɹ���
	stOpenPeivateResp.nOpenResult = enmOpenResult_OK;
	SendMessageResponse(MSGID_RSCL_OPEN_PEIVATE_RESP, pMsgHead, &stOpenPeivateResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);

	//����֪ͨ��mediaserver
	RoleID arrReciverID[1] = {pMsgHead->nRoleID};
	g_BllBase.SendAddTransmitUserNotifyToMediaServer(pOpenPrivateMicReqMsg->nDestRoleID, arrReciverID, 1, pRoom, false);


//	//����֪ͨ��mediaserver
//	CAddTransmitUserNotice stAddTransmitUserNotice;
//
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nRoleID = pOpenPrivateMicReqMsg->nDestRoleID;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.arrAddRoleID[0] = pMsgHead->nRoleID;
//	stAddTransmitUserNotice.nRoomID = pMsgHead->nRoomID;
//
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_P2P,pOpenPrivateMicReqMsg->nDestRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(pPlayer->GetPlayerNetType()));
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_Broadcast,pOpenPrivateMicReqMsg->nDestRoleID,enmEntityType_Media);
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
