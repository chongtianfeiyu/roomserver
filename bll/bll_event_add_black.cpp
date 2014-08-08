/*
 *  bll_event_add_black.cpp
 *
 *  To do��
 *  Created on: 	2012-1-10
 *  Author: 		luocj
 */

#include "bll_event_add_black.h"
#include "dal/to_server_message.h"
#include "dal/from_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CAddBlockEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	int32_t ret = S_OK;
	CAddBlackSessionData * pData =(CAddBlackSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("recv add black to DB response!{nRoleID=%d, nRoomID=%d} ",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	//��ȡ��Ҷ���
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->nDestRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("get player filed!!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CAddPlayerToBlackResp *pAddPlayerToBlackResp = dynamic_cast<CAddPlayerToBlackResp *>(pMsgBody);
	if(NULL == pAddPlayerToBlackResp)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
		return E_NULLPOINTER;
	}

	if(pAddPlayerToBlackResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("get item info failed ret = 0x%8X!\n",pAddPlayerToBlackResp->nResult);
		SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
		return pAddPlayerToBlackResp->nResult;
	}
	//��������:�ӷ������ɾ�������뱻�߶���
	if(!(pPlayer->IsReboot()))
	{
		ret = pRoom->AddBlackList(pPlayer->GetRoleID());
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because  add to blacklist error!{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
			return ret;
		}
	}
	//����֪ͨ�ͻ�Ӧ
	SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_OK,pData->m_nOptionLen,pData->m_arrOptionData);
	SendNotifyToUser(pData->m_sMsgHead.nRoomID,pData->m_sMsgHead.nRoleID,pPlayer->GetRoleID(),pData->m_nOptionLen,pData->m_arrOptionData);
	//����
	ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,pData->m_sMsgHead.nMessageID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("add player to black filed because exit room filed!{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
		return ret;
	}
	return S_OK;
}
int32_t CAddBlockEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}
	CAddBlackSessionData * pData =(CAddBlackSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("add black time out {nRoleID=%d}",pData->m_sMsgHead.nRoleID);
	SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
	return S_OK;
}
int32_t CAddBlockEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_ADD_BLACK_REQ:
		OnEventAddBlock(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CAddBlockEvent::OnEventAddBlock(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CAddBlackReq *pAddBlackReq = dynamic_cast<CAddBlackReq *>(pMsgBody);
	if(pAddBlackReq==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pAddBlackReq=0x%08x}\n",pAddBlackReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player add user to black req!{nSrcRoleID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pAddBlackReq->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//û�д����
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist!{nSrcRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{ret=0x%08x} \n",ret);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does not in room{nSrcRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	//�жϱ������˵�״̬
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pAddBlackReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//û�д����
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("dest player does no exist{nDestRoleID=%d}",pAddBlackReq->nDestRoleID);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex))&&!(pRoom->IsRebotPlayerInRoom(pAddBlackReq->nDestRoleID)))
	{
		WRITE_ERROR_LOG("player does not in room{nDestRoleID=%d, nRoomID=%d}",pAddBlackReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//�������Ͽ�������
	if(nSrcRoleRank<enmRoleRank_Host)
	{
		// todo ����Ȩ�޲�����Ӧ
		SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
    if(pRoom->IsInBlackList(pAddBlackReq->nDestRoleID))
    {
    	WRITE_DEBUG_LOG("has error!");
    	SendResponseToUser(pMsgHead,enmAddBlackResult_OK,nOptionLen,pOptionData);
    	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pAddBlackReq->nDestRoleID,nOptionLen,pOptionData);
    	return S_OK;
    }
    bool isOffice = pPlayer->IsOfficialPlayer();
    if(!isOffice)
    {
    	//����������Ĳ���
		ret = AddPlayerToBlack(pRoom,pAddBlackReq->nDestRoleID);
		if(ret == (int32_t)E_RS_NOPERMISSIONS)
		{
			// todo ����Ȩ�޲�����Ӧ
			SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission,nOptionLen,pOptionData);
			return ret;
		}
		else if(ret == (int32_t)E_RS_NOPERMISSIONS_KING)
		{
			// todo ����Ȩ�޲�����Ӧ
			SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission_King,nOptionLen,pOptionData);
			return ret;
		}
		else if(ret<0)
		{
			//todo ���ʹ����Ӧ
			SendResponseToUser(pMsgHead,enmAddBlackResult_Umknown,nOptionLen,pOptionData);
			return ret;
		}
    }
	//����ǻ����ˣ����������ݿ�
	if(pDestPlayer->IsReboot())
	{
		//����֪ͨ�ͻ�Ӧ
		SendResponseToUser(pMsgHead,enmAddBlackResult_OK,nOptionLen,pOptionData);
		SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pAddBlackReq->nDestRoleID,nOptionLen,pOptionData);
		//�˷�
		ret = ExitRoom(pDestPlayer,nDestPlayerIndex,pRoom,pMsgHead->nMessageID);
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because exit room filed{nDestRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pMsgHead->nRoomID);
			return ret;
		}
		return S_OK;
	}
	//������ӵ����ݿ�����
	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_DBRS_ADD_BLACK_RESP, enmSessionType_AddToBlack, this, enmAddBlackTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("create add black session is failed!{RoleID=%d}\n", pMsgHead->nRoleID);
		//todo ���ʹ����Ӧ
		SendResponseToUser(pMsgHead,enmAddBlackResult_Umknown,nOptionLen,pOptionData);

		return ret;
	}

	CAddBlackSessionData * pData = new (pSession->GetSessionData())CAddBlackSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	//��ʱ�Ŀ�ѡ�ֶ������������ҵ�������ϢConnUin
	if(nOptionLen > 0)
	{
		pData->m_nOptionLen = nOptionLen;
		memcpy(pData->m_arrOptionData, pOptionData, nOptionLen);
	}
	pData->nDestRoleID = pAddBlackReq->nDestRoleID;
	//��������
	CAddPlayerToBlackReq stAddPlayerToBlackReq;
	//Ϊ0�Ǳ�ʾ��ȡ�����������˵�
	stAddPlayerToBlackReq.nRoleID = pAddBlackReq->nDestRoleID;
	stAddPlayerToBlackReq.nRoomID = pMsgHead->nRoomID;

	SendMessageRequest(MSGID_RSDB_ADD_BLACK_REQ, &stAddPlayerToBlackReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send add black request");
	WRITE_NOTICE_LOG("player add user to balck!{nSrcRoleID=%d, nDestRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pAddBlackReq->nDestRoleID);
	return ret;
}
/*
 *  ����Ҽ��������
 */
int32_t CAddBlockEvent::AddPlayerToBlack(CRoom *pRoom,RoleID nAddBlockRoleID)
{
	int32_t ret = S_OK;
	if(NULL==pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pMsgHead=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nAddBlockRoleID, pPlayer, nPlayerIndex);
	//û�б� �Ӻ��������
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("dest player does no exist{nAddBlockRoleID=%d}",nAddBlockRoleID);
		return ret;
	}
	//�жϱ��Ӻ�������ҵ�RoleRank
	RoleRank nRoleRank = pRoom->GetRoleRank(nAddBlockRoleID);
	if(nRoleRank>=enmRoleRank_Admin)
	{
		WRITE_ERROR_LOG("add player to black filed because no permissions{nAddBlockRoleID=%d, nRoleRank=%d}",nAddBlockRoleID,nRoleRank);
		return E_RS_NOPERMISSIONS;
	}
	//��ɫ�ʹ������û���������
	if(pPlayer->GetVipLevel()>=enmVipLevel_Silver_KING)
	{
		WRITE_ERROR_LOG("add player to black filed because no permissions to king!{nAddBlockRoleID=%d, nVipLevel=0x%02X}",nAddBlockRoleID,pPlayer->GetVipLevel());
		return E_RS_NOPERMISSIONS_KING;
	}
	return ret;
}

/*
 * ����֪ͨ������������
 */
int32_t CAddBlockEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen , const void *pOptionData )
{
	CAddBlackNoti stAddBlackNoti;
	stAddBlackNoti.nSrcRoleID = nSrcRoleID;
	stAddBlackNoti.nDestRoleID = nDestRoleID;

	//���͸����������˵�������
	SendMessageNotifyToClient(MSGID_RSCL_ADD_BLACK_NOTI, &stAddBlackNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send add user to black notify");
	return S_OK;
}
/*
 * ������Ӧ���ͻ���
 */
int32_t CAddBlockEvent::SendResponseToUser(MessageHeadSS *pMsgHead,AddBlackResult nAddBlackResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CAddBlackResp stAddBlackResp;
	stAddBlackResp.nAddBlackResult = nAddBlackResult;
	SendMessageResponse(MSGID_RSCL_ADD_BLACK_RESP, pMsgHead, &stAddBlackResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
