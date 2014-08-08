/*
 *  bll_event_p2p_response.cpp
 *
 *  To do��
 *  Created on: 	2011-12-19
 *  Author: 		luocj
 */

#include "bll_event_p2p_response.h"
#include "../dal/to_server_message.h"
#include "ctl/server_datacenter.h"
#include "dal/to_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CP2PResponseEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CP2PResponseEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CP2PResponseEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_RECVP2P_REQ:
		OnEventP2PResponse(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * ����һ��һ��Ƶ�Ľ�����Ӧ�����Կͻ��ˣ�
 */
int32_t CP2PResponseEvent::OnEventP2PResponse(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CP2PAnswerReq *pP2PAnswerReq = dynamic_cast<CP2PAnswerReq *>(pMsgBody);
	if(pP2PAnswerReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pP2PAnswerReq=0x%08x}\n",pP2PAnswerReq);
		return E_NULLPOINTER;
	}
	if(pMsgHead->nRoleID == pP2PAnswerReq->nDestRoleID)
	{
		WRITE_ERROR_LOG("recv room answer p2pReq Dest is Src{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return ret;
	}
	WRITE_NOTICE_LOG("recv roleID in room answerp2pReq{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);

	if(ret < 0 || pPlayer == NULL)
	{
		//û�д����
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}

	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x}\n",ret);
		return S_OK;
	}
	//�ж�����Ƿ��ڷ�����
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	//��ȡ���𸴵����
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nSrcPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pP2PAnswerReq->nDestRoleID, pDestPlayer, nSrcPlayerIndex);
	if(ret < 0 || pDestPlayer == NULL)
	{
		//û�д����
		WRITE_ERROR_LOG("player does no exist{nDestRoleID=%d}",pP2PAnswerReq->nDestRoleID);
		return ret;
	}
	if(!(pRoom->IsPlayerInRoom(nSrcPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pP2PAnswerReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	ConnectP2PResult nConnectP2PResult = enmConnectP2PResult_OK;
	if(pP2PAnswerReq->nResponseP2PType == enmRESPONSEP2PTYPE_REFUSE)
	{
		//��������p2p����ҷ��;ܾ�֪ͨ
		SendNotifyToClient(pP2PAnswerReq->nResponseP2PType,pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID,pMsgHead->nRoleID,nOptionLen,pOptionData);
		WRITE_NOTICE_LOG("player in room REFUSE p2p mic to player success{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID);
		return ret;
	}
	//Ϊ����ʱ�Ĳ���
	GenerateConnectP2PResult(pPlayer->GetPlayerGlobalState(),nConnectP2PResult,true);
	if(nConnectP2PResult != enmConnectP2PResult_OK)
	{
		//��Ӧ��
		SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
		return ret;
	}
	GenerateConnectP2PResult(pDestPlayer->GetPlayerGlobalState(),nConnectP2PResult,false);
	if(nConnectP2PResult != enmConnectP2PResult_OK)
	{
		//��Ӧ��
		SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
		return ret;
	}
	//�޸���ҵ�״̬Ϊ��һ��һ����
	pPlayer->SetStateInRoom(enmPlayerState_P2PMic,pMsgHead->nRoomID);
	pDestPlayer->SetStateInRoom(enmPlayerState_P2PMic,pMsgHead->nRoomID);
	pPlayer->SetPlayerGlobalState(enmPlayerState_P2PMic);
	pDestPlayer->SetPlayerGlobalState(enmPlayerState_P2PMic);
	//�໥�������ںͶԷ�һ��һ��Ƶ
	pPlayer->SetP2PRoleIDInRoom(pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID);
	pDestPlayer->SetP2PRoleIDInRoom(pMsgHead->nRoomID,pMsgHead->nRoleID);
	//�����߷��ͽ����ɹ���Ӧ
	SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
	//�������߷��ͶԷ�����֪ͨ
	SendNotifyToClient(pP2PAnswerReq->nResponseP2PType,pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID,pMsgHead->nRoleID,nOptionLen,pOptionData);
	//����notify������mediaserver
	SendNotifyToMediaServer(pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID,pMsgHead->nRoleID);
	WRITE_NOTICE_LOG("player in room ACCEPT p2p mic to player success{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pP2PAnswerReq->nDestRoleID);

	COnMicNoti stOnMicNoti;
	stOnMicNoti.nOnMicRoleID = pPlayer->GetRoleID();
	stOnMicNoti.nMicType = enmMicType_P2PMic;
	SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);
	stOnMicNoti.nOnMicRoleID = pP2PAnswerReq->nDestRoleID;
	SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pP2PAnswerReq->nDestRoleID, enmEntityType_Hall);

	return ret;
}

int32_t CP2PResponseEvent::GenerateConnectP2PResult(PlayerState nPlayerState,ConnectP2PResult &nConnectP2PResult,bool isSelf)
{
	int32_t ret = S_OK;
	switch(nPlayerState)
	{
		case enmPlayerState_PublicMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_PUBLIC;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_PUBLIC;
			}
		}

		break;
		case enmPlayerState_PrivateMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_PRIVATE;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_PRIVATE;
			}
		}
		break;
		case enmPlayerState_P2PMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_P2P;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_P2P;
			}
		}
		break;
		case enmInvalidPlayerState:
		{
			nConnectP2PResult = enmConnectP2PResult_UNKNOWN;
		}
		break;
		default:
		break;
	}
	return ret;
}

int32_t CP2PResponseEvent::SendRespToClient(MessageHeadSS *pMsgHead,ConnectP2PResult nConnectP2PResult,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	CP2PResp stP2PResp;
	stP2PResp.nConnectP2PResult = nConnectP2PResult;
	SendMessageResponse(MSGID_RSCL_RECVP2P_RESP, pMsgHead, &stP2PResp,enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return ret;
}

int32_t CP2PResponseEvent::SendNotifyToClient(ResponseP2PType nResponseP2PType,RoomID nRoomID,RoleID nDestRoleID,RoleID nAnswerRoleID,const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	CP2PAnswerNoti stP2PAnswerNoti;
	stP2PAnswerNoti.nResponseP2PType = nResponseP2PType;
	stP2PAnswerNoti.nSrcRoleID = nAnswerRoleID;
	stP2PAnswerNoti.nDestRoleID = nDestRoleID;
	BroadcastType nBroadcastType = enmBroadcastType_ExpectPlayr;
	RoleID nRoleID = nDestRoleID;    //������Ҫ֪ͨ�����߲���Ҫ֪ͨ�����
	//���ܵ�ʱ��֪ͨ���˴���������
	if(nResponseP2PType == enmRESPONSEP2PTYPE_ACCEPT)
	{
		nRoleID = nAnswerRoleID;
		nBroadcastType = enmBroadcastType_ExceptPlayr;
	}
	SendMessageNotifyToClient(MSGID_RSCL_RECVP2P_NOTI, &stP2PAnswerNoti, nRoomID, nBroadcastType,
			nRoleID, nOptionLen, (char*)pOptionData, "send answer p2p notify");
	return ret;
}

int32_t CP2PResponseEvent::SendNotifyToMediaServer(RoomID nRoomID,RoleID nSrcRoleID,RoleID nAnswerRoleID)
{
	int32_t ret = S_OK;
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x}\n",nRoomID,ret);
		return S_OK;
	}
	//��ȡ���
	CPlayer *pSrcPlayer = NULL;
	PlayerIndex nSrcPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nSrcRoleID, pSrcPlayer, nSrcPlayerIndex);
	if(ret < 0 || pSrcPlayer == NULL)
	{
		//û�д����
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",nSrcRoleID);
		return ret;
	}
	//��ȡ���𸴵����
	CPlayer *pDestPlayer = NULL;
	ret = g_PlayerMgt.GetPlayer(nAnswerRoleID, pDestPlayer, nSrcPlayerIndex);
	if(ret < 0 || pDestPlayer == NULL)
	{
		//û�д����
		WRITE_ERROR_LOG("player does no exist{nAnswerRoleID=%d}",nAnswerRoleID);
		return ret;
	}

//	SendPlayerOnMicToMedia(pRoom,pSrcPlayer);
//	SendPlayerOnMicToMedia(pRoom,pDestPlayer);

	//��������֪ͨ��Ϣ��mediaServer
	RoleID arrReciverID[1] = {nAnswerRoleID};
	g_BllBase.SendAddTransmitUserNotifyToMediaServer(nSrcRoleID, arrReciverID, 1, pRoom, false);
	arrReciverID[0] = nSrcRoleID;
	g_BllBase.SendAddTransmitUserNotifyToMediaServer(nAnswerRoleID, arrReciverID, 1, pRoom, false);

//	//���ͷ�������֪ͨ��Ϣ��mediaServer
//	CAddTransmitUserNotice stAddTransmitUserNotice;
//
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nRoleID = nSrcRoleID;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.arrAddRoleID[0]=nAnswerRoleID;
//	stAddTransmitUserNotice.nRoomID = nRoomID;
//	//���������ߵ�ת�����֪ͨ
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,nRoomID,enmTransType_Broadcast,nSrcRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(pDestPlayer->GetPlayerNetType()));
//	//���ͽ����ߵ�ת�����֪ͨ
//	stAddTransmitUserNotice.nRoleID = nAnswerRoleID;
//	stAddTransmitUserNotice.arrAddRoleID[0]=nSrcRoleID;
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,nRoomID,enmTransType_Broadcast,nAnswerRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(pSrcPlayer->GetPlayerNetType()));
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
