/*
 *  bll_event_kickuser.cpp
 *
 *  To do��
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
 * ����������Ϣ
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
		WRITE_ERROR_LOG("get room error!{ret=0x%08x}\n",ret);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}


	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//�ж��Ƿ������˵�Ȩ��
	if(nSrcRoleRank<enmRoleRank_Admin&&pPlayer->GetVipLevel()<enmVipLevel_Baron)
	{
		WRITE_ERROR_LOG("kick player filed because no permissions{nSrcRoleID=%d, nSrcRoleRank=0x%04x, nVipLevel=0x%04x}",pCKickUserReq->nKickRoleID,nSrcRoleRank,pPlayer->GetVipLevel());
		SendResponseToClient(pMsgHead,enmKickResult_NO_Permission,nOptionLen,pOptionData);
		return ret;
	}
	if(pCKickUserReq->nKickType == enmKickType_KickVisitor)
	{
		//�����߳������ο�
		SendNotifyToUser(pRoom->GetRoomID(),pMsgHead->nRoleID,enmKickType_KickVisitor,enmInvalidRoleID,"visitor",0,0,NULL);
		ret = KickVisitor(pRoom);
		if(ret<0)
		{
			WRITE_ERROR_LOG("kick visitor filed {ret=0x%08x}\n",ret);
			SendResponseToClient(pMsgHead,enmKickResult_Umknown,nOptionLen,pOptionData);
			return ret;
		}
		//���ͳɹ���Ӧ
		SendResponseToClient(pMsgHead,enmKickResult_OK,nOptionLen,pOptionData);
		KickRobot(pRoom, pRoom->GetNeedRobotCount());
		return ret;
	}
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pCKickUserReq->nKickRoleID, pDestPlayer, nDestPlayerIndex);
	//û�б������
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nKickRoleID=%d}",pCKickUserReq->nKickRoleID);
		return ret;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pCKickUserReq->nKickRoleID)))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pCKickUserReq->nKickRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//�߷��ο�
	KickResult nKickResult=enmKickResult_Umknown;
	ret = KickUser(pRoom,nSrcRoleRank,pPlayer->GetVipLevel(),pCKickUserReq->nKickRoleID,pCKickUserReq->strReason,pCKickUserReq->nTime,nKickResult);
	//���ͳɹ���Ӧ
	SendResponseToClient(pMsgHead,nKickResult,nOptionLen,pOptionData);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("kick palyer error{nKickRoleID=%d, ret=0x%08x}",pCKickUserReq->nKickRoleID,ret);
		return ret;
	}
	//��������֪ͨ�������е����
	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,enmKickType_Other,pCKickUserReq->nKickRoleID,pCKickUserReq->strReason,pCKickUserReq->nTime,nOptionLen,pOptionData);
	//�˷�
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
 * �߳�����������ο�
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
		//���οͲ���
		if(pPlayer->GetVipLevel() > enmVipLevel_NONE)
		{
			continue;
		}
		//��������:�ӷ������ɾ�������뱻�߶���
		uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
		uint32_t nEndTime = pRoom->GetEndKickTime(nKickRoleID);
		if(nEndTime!=0)
		{
			WRITE_ERROR_LOG("kick player filed because is did in kicklist{nKickRoleID=%d}",nKickRoleID);
			continue;
		}
		//���ο͵�ʱ��ʱ��Ĭ��Ϊ5����
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
		//��������server��֪ͨ
		SendNotifyToServer(pRoom,pRoom->GetRoomID(),nKickRoleID,pPlayer);
		//������Ҷ���
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
 * �ߵ����
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
	//û�б������
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nKickRoleID=%d}",nKickRoleID);
		return ret;
	}
	RoleRank nRoleRank = pRoom->GetRoleRank(nKickRoleID);
	VipLevel nDestVipLevel = pPlayer->GetVipLevel();
	//���ߵ����й���Ȩ��
	if(nRoleRank >= enmRoleRank_TempAdmin)
	{

		if(nRoleRank>=nSrcRoleRank)
		{
			WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestRoleRank=%d}",nKickRoleID,nSrcRoleRank,nRoleRank);
			nKickResult = enmKickResult_NO_Permission;
			return E_RS_NOPERMISSIONS;
		}
		//�й�
		if(nDestVipLevel >= enmVipLevel_Baron)
		{
			//todo �ж�VIP �ʹڵȼ���������ڡ�ˮ��ֻ���������ߣ������ڲ������ߣ�
			if((pPlayer->GetVipLevel() >= enmVipLevel_Marquis && nSrcRoleRank < enmRoleRank_Super)
					|| (pPlayer->GetVipLevel() <= enmVipLevel_Earl && pPlayer->GetVipLevel() >= enmVipLevel_Baron && nSrcRoleRank < enmRoleRank_Host))
			{
				WRITE_ERROR_LOG("kick player filed because no permissions{nKickRoleID=%d, nSrcRoleRank=%d, nDestVipLevel=%d}",nKickRoleID,nSrcRoleRank,pPlayer->GetVipLevel());
				nKickResult = enmKickResult_NO_Permission_King;
				return E_RS_NOPERMISSIONS_KING;
			}
		}
	}
	//���ߵ���û�й���Ȩ�ޣ�Ȩ�޺�vip�϶��߲��˵�ʱ�򷵻أ�
	else
	{
		if(nDestVipLevel >= enmVipLevel_Baron)
		{
			if(nDestVipLevel >= nSrcVipLevel)
			{
				//todo �ж�VIP �ʹڵȼ���������ڡ�ˮ��ֻ���������ߣ������ڲ������ߣ�
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
	//��������:�ӷ������ɾ�������뱻�߶���
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
 * ����֪ͨ�����server
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
 * ����֪ͨ��user
 */
int32_t CKickUserEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,KickType nKickType,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen , const void *pOptionData )
{
	CKickUserNoti stKickUserNoti;
	stKickUserNoti.nSrcRoleID = nSrcRoleID;
	stKickUserNoti.nKickAllVisitor = nKickType;
	stKickUserNoti.nKickRoleID = nDestRoleID;
	stKickUserNoti.strReason = strReason;
	stKickUserNoti.nTime = nTime;
	//���͸���������������
	SendMessageNotifyToClient(MSGID_RSCL_KICK_USER_NOTI, &stKickUserNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send kick user notify");
	return S_OK;
}

/*
 * ������Ӧ���ͻ���
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
