/*
 * bll_event_songvote.cpp
 *
 *  Created on: 2013-3-26
 *      Author: jimm
 */

#include "bll_event_songvote.h"
#include "def/server_errordef.h"
#include "ctl/song_mgt.h"
#include "ctl/program_mgt.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "requestsong/bll_timer_program.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CSongVoteMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CSongVoteMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CSongVoteMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CSongVoteReq *pSongVoteReq = dynamic_cast<CSongVoteReq *>(pMsgBody);
	if(NULL == pSongVoteReq)
	{
		WRITE_ERROR_LOG("null pointer:{pSongVoteReq=0x%08x}\n",pSongVoteReq);
		return E_NULLPOINTER;
	}

	CSongVoteResp stResp;

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pMsgHead->nRoomID, pSongVoteReq->nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found songorderinfo!nRoomID=%d,nRoleID=%d,nOrderID=%u}\n",
				pMsgHead->nRoomID, pMsgHead->nRoleID, pSongVoteReq->nOrderID);

		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_SONGVOTE_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	//订单信息不是接受状态
	if(pSongOrderInfo->nOrderStatus != enmOrderStatus_Accepted)
	{
		WRITE_ERROR_LOG("fight for ticket is error, because order status is not correct!"
				"{OrderStatus=%d}\n", pSongOrderInfo->nOrderStatus);

		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_SONGVOTE_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	if(pSongVoteReq->nSongVoteResult == enmSongVoteResult_Good)
	{
		pSongOrderInfo->nOrderStatus = enmOrderStatus_Good;
	}
	else if(pSongVoteReq->nSongVoteResult == enmSongVoteResult_Bad)
	{
		pSongOrderInfo->nOrderStatus = enmOrderStatus_Bad;
	}
	else
	{
		pSongOrderInfo->nOrderStatus = enmOrderStatus_NotVote;
	}

	pSongOrderInfo->nSongVoteResult = pSongVoteReq->nSongVoteResult;

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found player!nRoleID=%d}\n", pMsgHead->nRoleID);

		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_SONGVOTE_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	nRet = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{it's not found room!nRoomID=%d}\n", pMsgHead->nRoomID);

		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_SONGVOTE_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	SendMessageResponse(MSGID_RSCL_SONGVOTE_RESP, pMsgHead, &stResp, enmTransType_P2P);

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	//更新数据库记录
	CUpdateSongOrderInfoNoti stUpdateSongOrderInfoNoti;
	stUpdateSongOrderInfoNoti.strTransID = pSongOrderInfo->strTransID;
	stUpdateSongOrderInfoNoti.nRoomID = pSongOrderInfo->nRoomID;
	stUpdateSongOrderInfoNoti.nOrderStartTime = pSongOrderInfo->nOrderStartTime;
	stUpdateSongOrderInfoNoti.nOrderEndTime = pSongOrderInfo->nOrderEndTime;
	stUpdateSongOrderInfoNoti.nOrderStatus = pSongOrderInfo->nOrderStatus;
	stUpdateSongOrderInfoNoti.strSongName = pSongOrderInfo->strSongName;
	stUpdateSongOrderInfoNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
	stUpdateSongOrderInfoNoti.strSongerName = pSongOrderInfo->strSongerName;
	stUpdateSongOrderInfoNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
	stUpdateSongOrderInfoNoti.strRequestName = pSongOrderInfo->strRequestName;
	stUpdateSongOrderInfoNoti.strWishWords = pSongOrderInfo->strWishWords;
	stUpdateSongOrderInfoNoti.nOrderCost = pSongOrderInfo->nOrderCost;
	stUpdateSongOrderInfoNoti.nSongCount = pSongOrderInfo->nSongCount;
	stUpdateSongOrderInfoNoti.nMarkResult = pSongOrderInfo->nSongVoteResult;

	g_BllBase.SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pMsgHead->nRoomID,
			enmTransType_ByKey, pSongOrderInfo->nSongerRoleID, enmEntityType_DBProxy, pSongOrderInfo->nSongerRoleID);

	//投票结果更新通知
	CSongVoteUpdateNoti stSongVoteUpdateNoti;
	stSongVoteUpdateNoti.nOrderID = pSongVoteReq->nOrderID;
	stSongVoteUpdateNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
	stSongVoteUpdateNoti.strRequestName = pSongOrderInfo->strRequestName;
	stSongVoteUpdateNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
	stSongVoteUpdateNoti.strSongerName = pSongOrderInfo->strSongerName;
	stSongVoteUpdateNoti.strSongName = pSongOrderInfo->strSongName;
	stSongVoteUpdateNoti.nSongVoteResult = pSongOrderInfo->nSongVoteResult;

	if(pSongOrderInfo->nSongVoteResult == enmSongVoteResult_Good)
	{
		SendMessageNotifyToClient(MSGID_RSCL_SONGVOTE_NOTI, &stSongVoteUpdateNoti, pMsgHead->nRoomID, enmBroadcastType_All,
				enmInvalidRoleID);
	}
	else
	{
		SendMessageNotifyToClient(MSGID_RSCL_SONGVOTE_NOTI, &stSongVoteUpdateNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr,
				pSongOrderInfo->nSongerRoleID);
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

