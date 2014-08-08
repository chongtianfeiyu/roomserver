/*
 * bll_event_fightforticket.cpp
 *
 *  Created on: 2013-3-23
 *      Author: jimm
 */

#include "bll_event_fightforticket.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "requestsong/bll_timer_program.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CFightForTicketMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CFightForTicketMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CFightForTicketMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	return S_OK;

	CFightForTicketResp stResp;

	CFightForTicketReq *pFightForTicketReq = dynamic_cast<CFightForTicketReq *>(pMsgBody);
	if(NULL == pFightForTicketReq)
	{
		WRITE_ERROR_LOG("null pointer:{pFightForTicketReq=0x%08x}\n",pFightForTicketReq);
		return E_NULLPOINTER;
	}

	SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(pMsgHead->nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found songprograminfo!nRoomID=%d,nRoleID=%d,nOrderID=%u}\n",
				pMsgHead->nRoomID, pMsgHead->nRoleID, pFightForTicketReq->nOrderID);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	//��Ŀ״̬��������Ʊ�׶�
	if(pSongProgramInfo->nProgramStatus != enmProgramStatus_FightingForTicket)
	{
		WRITE_ERROR_LOG("fight for ticket is error, because programe status is not correct!"
				"{ProgramStatus=%d}\n", pSongProgramInfo->nProgramStatus);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pMsgHead->nRoomID, pFightForTicketReq->nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found songorderinfo!nRoomID=%d,nRoleID=%d,nOrderID=%u}\n",
				pMsgHead->nRoomID, pMsgHead->nRoleID, pFightForTicketReq->nOrderID);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	//������Ϣ���ǽ���״̬
	if(pSongOrderInfo->nOrderStatus != enmOrderStatus_Accepted)
	{
		WRITE_ERROR_LOG("fight for ticket is error, because order status is not correct!"
				"{OrderStatus=%d}\n", pSongOrderInfo->nOrderStatus);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found player!nRoleID=%d}\n", pMsgHead->nRoleID);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	nRet = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found room!nRoomID=%d}\n", pMsgHead->nRoomID);

		stResp.nResult = enmFightForTicketResult_Unknown;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	//Ʊ������
	if((pSongProgramInfo->nTotalTicketCount <= 0) ||
			(pSongProgramInfo->nHaveTicketPlayerCount >= pSongProgramInfo->nTotalTicketCount))
	{
		WRITE_INFO_LOG("it's have no left ticket!{nTotalTicketCount=%d, nHaveTicketPlayerCount=%d}\n",
				pSongProgramInfo->nTotalTicketCount, pSongProgramInfo->nHaveTicketPlayerCount);

		stResp.nResult = enmFightForTicketResult_NoLeftTicket;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);

		FightForTicketFinish(pMsgHead->nRoomID, pFightForTicketReq->nOrderID, pSongProgramInfo, arrPlayer, nPlayerCount);

		return S_OK;
	}

	//����������ߣ�����������������Ʊ
	if((pRoom->GetHostID() == pMsgHead->nRoleID) ||
			(pSongOrderInfo->nRequestRoleID == pMsgHead->nRoleID) ||
			(pSongOrderInfo->nSongerRoleID == pMsgHead->nRoleID))
	{
		WRITE_INFO_LOG("it's have no permission fight for ticket!{HostID=%d, RequesterRoleID=%d, "
				"SongerRoleID=%d, FighterRoleID=%d}\n", pRoom->GetHostID(), pSongOrderInfo->nRequestRoleID,
				pSongOrderInfo->nSongerRoleID, pMsgHead->nRoleID);

		stResp.nResult = enmFightForTicketResult_NoPermission;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//����Ѿ�����Ʊ��
	if(pSongProgramInfo->IsHaveTicket(pMsgHead->nRoleID))
	{
		WRITE_INFO_LOG("player had one ticket, so he cann't fight for ticket agagin!{RoleID=%d}\n", pMsgHead->nRoleID);

		stResp.nResult = enmFightForTicketResult_NoPermission;
		SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//��ӵ���������
	uint16_t nLeftTicketCount = pSongProgramInfo->AddVoter(pMsgHead->nRoleID);

	stResp.nResult = enmFightForTicketResult_OK;
	SendMessageResponse(MSGID_RSCL_FIGHTFORTICKET_RESP, pMsgHead, &stResp, enmTransType_P2P);

	CTicketCountUpdateNoti stTicketCountUpdateNoti;
	stTicketCountUpdateNoti.nOrderID = pFightForTicketReq->nOrderID;
	stTicketCountUpdateNoti.nTotalTicketCount = pSongProgramInfo->nTotalTicketCount;
	stTicketCountUpdateNoti.nLeftTicketCount = nLeftTicketCount;
	stTicketCountUpdateNoti.nEndLeftTime = pSongProgramInfo->nStatusEndTime - CDateTime::CurrentDateTime().Seconds();

	//��ʼ����֪ͨ
	for(int32_t i = 0; i < nPlayerCount; ++i)
	{
		RoleID nRoleID = arrPlayer[i]->GetRoleID();
		stTicketCountUpdateNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pMsgHead->nRoomID, nRoleID);

		//���������,����������,��û��ͶƱȨ��
		if((nRoleID == pSongOrderInfo->nSongerRoleID) ||
				(nRoleID == pRoom->GetHostID()))
		{
			//stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		//����ǵ����,�Ѿ�����ͶƱȨ,���ǲ�����Ʊ
		else if(nRoleID == pSongOrderInfo->nRequestRoleID)
		{
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		//��������Ʊ����Һ��Ѿ�����Ʊ�����
		else if((nRoleID == pMsgHead->nRoleID) ||
				(pSongProgramInfo->IsHaveTicket(nRoleID)))
		{
			//stTicketCountUpdateNoti.nHaveTicket = 1;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		else
		{
			//stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = 1;
		}

		SendMessageNotifyToClient(MSGID_RSCL_TICKETCOUNTUPDATE_NOTI, &stTicketCountUpdateNoti,
				pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, nRoleID);
	}

	if(nLeftTicketCount <= 0)
	{
		FightForTicketFinish(pMsgHead->nRoomID, pFightForTicketReq->nOrderID, pSongProgramInfo, arrPlayer, nPlayerCount);
	}

	return S_OK;
}

int32_t CFightForTicketMessageEvent::FightForTicketFinish(RoomID nRoomID, uint32_t nOrderID,
		SongProgramInfo *pSongProgramInfo, CPlayer *arrPlayer[], int32_t nPlayerCount)
{
	//ɾ����Ʊ��ʱ��
	g_ProgramTimer.DeleteProgramTimer(nRoomID, nOrderID);

	//��ʼ����֪ͨ
	CEnjoyingNoti stEnjoyingNoti;
	stEnjoyingNoti.nOrderID = nOrderID;

	for(int32_t i = 0; i < nPlayerCount; ++i)
	{
		RoleID nRoleID = arrPlayer[i]->GetRoleID();
		if(pSongProgramInfo->IsHaveTicket(nRoleID))
		{
			stEnjoyingNoti.nHaveTicket = 1;
		}
		else
		{
			stEnjoyingNoti.nHaveTicket = 0;
		}
		SendMessageNotifyToClient(MSGID_RSCL_ENJOYING_NOTI, &stEnjoyingNoti, nRoomID, enmBroadcastType_ExpectPlayr,
				nRoleID);
	}

	if(pSongProgramInfo->nProgramStatus == enmProgramStatus_FightingForTicket)
	{
		//�������ͽ׶�
		g_ProgramTimer.CreateProgramTimer(nRoomID, nOrderID, enmProgramStatus_Enjoying);
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END


