/*
 * bll_event_processorder.cpp
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#include "bll_event_processorder.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "public_message_define.h"
#include "dal/billengine.h"
#include "ctl/program_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CProcessOrderMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_UpdateUserAsset:
			UpdateUserAssetResp(pMsgHead, pMsgBody, pSession);
		break;
		default:
		break;
	}

	return S_OK;
}

int32_t CProcessOrderMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmUpdateUserAssetTimeoutPeriod:
			UpdateUserAssetTimeout(pSession);
		break;
		default:
		break;
	}

	return S_OK;
}

//注意，只有拒绝时才进入本函数
int32_t CProcessOrderMessageEvent::UpdateUserAssetResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CProcessOrderSessionData * pData =(CProcessOrderSessionData *)(pSession->GetSessionData());

	CProcessOrderResp stResp;
	CUpdateUserAssetResp *pUpdateUserAssetResp = dynamic_cast<CUpdateUserAssetResp *>(pMsgBody);
	if(NULL == pUpdateUserAssetResp)
	{
		WRITE_ERROR_LOG("null pointer:{pUpdateUserAssetResp=0x%08x}\n", pUpdateUserAssetResp);
		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	if(pUpdateUserAssetResp->nResult < 0)
	{
		stResp.nResult = 1;
	}
	else
	{
		stResp.nResult = S_OK;
	}

	SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	CProcessOrderNoti stNoti;
	stNoti.nOrderID = pData->stSongOrderInfo.nOrderID;
	stNoti.nIsAccept = pData->m_stMsgBody.nIsAccept;
	stNoti.nSongerRoleID = pData->stSongOrderInfo.nSongerRoleID;
	stNoti.strSongerName = pData->stSongOrderInfo.strSongerName;
	stNoti.nRequestRoleID = pData->stSongOrderInfo.nRequestRoleID;
	stNoti.strRequestName = pData->stSongOrderInfo.strRequestName;
	stNoti.strSongName = pData->stSongOrderInfo.strSongName;
	stNoti.strWishWords = pData->stSongOrderInfo.strWishWords;
	stNoti.nSongCount = pData->stSongOrderInfo.nSongCount;

	//是同意点歌
	if((stResp.nResult == S_OK) && (pData->m_stMsgBody.nIsAccept == 1))
	{
		SendMessageNotifyToClient(MSGID_RSCL_PROCESSORDER_NOTI, &stNoti, pData->stSongOrderInfo.nRoomID,
				enmBroadcastType_All, enmInvalidRoleID);
	}
	else
	{
		SendMessageNotifyToClient(MSGID_RSCL_PROCESSORDER_NOTI, &stNoti, pData->stSongOrderInfo.nRoomID,
				enmBroadcastType_ExpectPlayr, pData->stSongOrderInfo.nRequestRoleID);
	}

	OrderStatus nOrderStatus = (pData->m_stMsgBody.nIsAccept == 1) ? enmOrderStatus_Accepted : enmOrderStatus_Refused;
	int64_t nCurTime = CDateTime::CurrentDateTime().Seconds();

	//更新数据库记录
	CUpdateSongOrderInfoNoti stUpdateSongOrderInfoNoti;
	stUpdateSongOrderInfoNoti.strTransID = pData->stSongOrderInfo.strTransID;
	stUpdateSongOrderInfoNoti.nRoomID = pData->stSongOrderInfo.nRoomID;
	stUpdateSongOrderInfoNoti.nOrderStartTime = pData->stSongOrderInfo.nOrderStartTime;
	stUpdateSongOrderInfoNoti.nOrderEndTime = nCurTime;
	stUpdateSongOrderInfoNoti.nOrderStatus = nOrderStatus;
	stUpdateSongOrderInfoNoti.strSongName = pData->stSongOrderInfo.strSongName;
	stUpdateSongOrderInfoNoti.nSongerRoleID = pData->stSongOrderInfo.nSongerRoleID;
	stUpdateSongOrderInfoNoti.strSongerName = pData->stSongOrderInfo.strSongerName;
	stUpdateSongOrderInfoNoti.nRequestRoleID = pData->stSongOrderInfo.nRequestRoleID;
	stUpdateSongOrderInfoNoti.strRequestName = pData->stSongOrderInfo.strRequestName;
	stUpdateSongOrderInfoNoti.strWishWords = pData->stSongOrderInfo.strWishWords;
	stUpdateSongOrderInfoNoti.nOrderCost = pData->stSongOrderInfo.nOrderCost;
	stUpdateSongOrderInfoNoti.nSongCount = pData->stSongOrderInfo.nSongCount;

	SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pData->m_stMsgBody.nRoomID,
			enmTransType_ByKey, pData->m_stMsgHead.nRoleID, enmEntityType_DBProxy, pData->m_stMsgHead.nRoleID);

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pData->stSongOrderInfo.nRoomID, pData->stSongOrderInfo.nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("update song order info, but it's not found song order info!"
				"{nRoleID=%d, nRoomID=%d, nOrderID=%u, nIsAccept=%d}\n", pData->m_stMsgHead.nRoleID,
				pData->stSongOrderInfo.nRoomID, pData->stSongOrderInfo.nOrderID, pData->m_stMsgBody.nIsAccept);
		return S_OK;
	}

	//更新内存记录
	pSongOrderInfo->nOrderEndTime = nCurTime;
	pSongOrderInfo->nOrderStatus = nOrderStatus;

	return S_OK;
}

int32_t CProcessOrderMessageEvent::UpdateUserAssetTimeout(CFrameSession *pSession)
{
	CProcessOrderSessionData * pData =(CProcessOrderSessionData *)(pSession->GetSessionData());

	WRITE_WARNING_LOG("process order time out{nRoleID=%d, nOrderID=%d, RoomID=%d, "
			"nIsAccept=%d, nSessionIndex=%d}",
			pData->m_stMsgHead.nRoleID, pData->m_stMsgBody.nOrderID, pData->m_stMsgBody.nRoomID,
			pData->m_stMsgBody.nIsAccept, pSession->GetSessionIndex());

	CProcessOrderResp stResp;
	stResp.nResult = 1;

	SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

int32_t CProcessOrderMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CProcessOrderReq *pProcessOrderReq = dynamic_cast<CProcessOrderReq *>(pMsgBody);
	if(NULL == pProcessOrderReq)
	{
		WRITE_ERROR_LOG("null pointer:{pProcessOrderReq=0x%08x}\n",pProcessOrderReq);
		return E_NULLPOINTER;
	}

	CProcessOrderResp stResp;

	CPlayer *pProcessPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pProcessPlayer, nPlayerIndex);
	if(nRet < 0 || pProcessPlayer == NULL)
	{
		WRITE_WARNING_LOG("it's not found process player!{nRoleID=%d, nRoomID=%d, nOrderID=%u, nIsAccept=%d}\n",
				pMsgHead->nRoleID, pProcessOrderReq->nRoomID, pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//不是艺人
	IdentityType nIdentityType = pProcessPlayer->GetIdentityType();
	if(((nIdentityType & enmIdentityType_SONGER) == 0) &&
			((nIdentityType & enmIdentityType_DANCER) == 0) &&
			((nIdentityType & enmIdentityType_HOST) == 0))
	{
		WRITE_WARNING_LOG("she is not a songer!{nRoleID=%d, nRoomID=%d, IdentityType=%d}!\n",
				pMsgHead->nRoleID, pProcessOrderReq->nRoomID, nIdentityType);
		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pProcessOrderReq->nRoomID, pProcessOrderReq->nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("it's not found song order info!{nRoleID=%d, nRoomID=%d, nOrderID=%u, nIsAccept=%d}\n",
				pMsgHead->nRoleID, pProcessOrderReq->nRoomID, pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);
		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	int64_t nCurTime = CDateTime::CurrentDateTime().Seconds();

	if(pSongOrderInfo->nOrderStatus != enmOrderStatus_Processing)
	{
		WRITE_WARNING_LOG("order had finished!{nRoleID=%d, nRoomID=%d, nOrderID=%u, nIsAccept=%d, nOrderStartTime=%u,"
				" nOrderEndTime=%u, nOrderStatus=%d}\n",
				pMsgHead->nRoleID, pProcessOrderReq->nRoomID, pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept,
				pSongOrderInfo->nOrderStartTime, pSongOrderInfo->nOrderEndTime, pSongOrderInfo->nOrderStatus);
		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//接受
	if(pProcessOrderReq->nIsAccept == 1)
	{
//		SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(pProcessOrderReq->nRoomID);
//		if(pSongProgramInfo == NULL)
//		{
//			WRITE_ERROR_LOG("it's not found song program info!{nRoleID=%d, nRoomID=%d, "
//					"nOrderID=%u, nIsAccept=%d}\n", pMsgHead->nRoleID, pProcessOrderReq->nRoomID,
//					pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);
//		}
//		else
//		{
//			if((pSongProgramInfo->nProgramStatus == enmProgramStatus_FightingForTicket) ||
//					(pSongProgramInfo->nProgramStatus == enmProgramStatus_Enjoying) ||
//					(pSongProgramInfo->nProgramStatus == enmProgramStatus_Voteing))
//			{
//				WRITE_ERROR_LOG("previos program is not finish, so you cann't accept new program!{nRoleID=%d, nRoomID=%d, "
//						"nOrderID=%u, nIsAccept=%d}\n", pMsgHead->nRoleID, pProcessOrderReq->nRoomID,
//						pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);
//
//				stResp.nResult = 2;
//				SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
//
//				return S_OK;
//			}
//
//			if((pSongProgramInfo->nProgramStatus == enmProgramStatus_ShowResult) &&
//					(nCurTime - pSongProgramInfo->nShowResultStartTime <= g_SomeConfig.GetShowResultTime()))
//			{
//				WRITE_ERROR_LOG("show result is not finish, so you cann't accept new program!{nRoleID=%d, nRoomID=%d, "
//						"nOrderID=%u, nIsAccept=%d}\n", pMsgHead->nRoleID, pProcessOrderReq->nRoomID,
//						pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);
//
//				stResp.nResult = 2;
//				SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);
//
//				return S_OK;
//			}
//		}

		//发送响应给客户端
		stResp.nResult = S_OK;
		SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P);

		OrderStatus nOrderStatus = enmOrderStatus_Accepted;

		CProcessOrderNoti stNoti;
		stNoti.nOrderID = pSongOrderInfo->nOrderID;
		stNoti.nIsAccept = enmOrderStatus_Accepted;
		stNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
		stNoti.strSongerName = pSongOrderInfo->strSongerName;
		stNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
		stNoti.strRequestName = pSongOrderInfo->strRequestName;
		stNoti.strSongName = pSongOrderInfo->strSongName;
		stNoti.strWishWords = pSongOrderInfo->strWishWords;
		stNoti.nSongCount = pSongOrderInfo->nSongCount;

		//给所有人发通知
		SendMessageNotifyToClient(MSGID_RSCL_PROCESSORDER_NOTI, &stNoti, pSongOrderInfo->nRoomID,
				enmBroadcastType_All, enmInvalidRoleID);

		//更新数据库记录
		CUpdateSongOrderInfoNoti stUpdateSongOrderInfoNoti;
		stUpdateSongOrderInfoNoti.strTransID = pSongOrderInfo->strTransID;
		stUpdateSongOrderInfoNoti.nRoomID = pSongOrderInfo->nRoomID;
		stUpdateSongOrderInfoNoti.nOrderStartTime = pSongOrderInfo->nOrderStartTime;
		stUpdateSongOrderInfoNoti.nOrderEndTime = nCurTime;
		stUpdateSongOrderInfoNoti.nOrderStatus = nOrderStatus;
		stUpdateSongOrderInfoNoti.strSongName = pSongOrderInfo->strSongName;
		stUpdateSongOrderInfoNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
		stUpdateSongOrderInfoNoti.strSongerName = pSongOrderInfo->strSongerName;
		stUpdateSongOrderInfoNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
		stUpdateSongOrderInfoNoti.strRequestName = pSongOrderInfo->strRequestName;
		stUpdateSongOrderInfoNoti.strWishWords = pSongOrderInfo->strWishWords;
		stUpdateSongOrderInfoNoti.nOrderCost = pSongOrderInfo->nOrderCost;
		stUpdateSongOrderInfoNoti.nSongCount = pSongOrderInfo->nSongCount;

		SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pProcessOrderReq->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID);

		//更新内存记录
		pSongOrderInfo->nOrderEndTime = nCurTime;
		pSongOrderInfo->nOrderStatus = nOrderStatus;

		//发送请求
		CUpdateUserAssetReq stUpdateUserAssetReq;
		stUpdateUserAssetReq.strTransID = pSongOrderInfo->strTransID;
		int32_t nUpdateIndex = 0;
		stUpdateUserAssetReq.arrAssetType[nUpdateIndex] = enmAssetType_SpendMoneySum;
		stUpdateUserAssetReq.arrUpdateAssetOperate[nUpdateIndex] = enmUpdateAssetOperate_Increase;
		stUpdateUserAssetReq.arrAssetValue[nUpdateIndex] = pSongOrderInfo->nOrderCost;
		stUpdateUserAssetReq.nCauseID = enmCauseID_AcceptRequestSong;
		stUpdateUserAssetReq.nRoleID = pSongOrderInfo->nRequestRoleID;
		stUpdateUserAssetReq.nUpdateCount = ++nUpdateIndex;

		SendMessageRequest(MSGID_OTIS_UPDATEUSERASSET_REQ , &stUpdateUserAssetReq, pMsgHead->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Item, pMsgHead->nRoleID,
				enmInvalidSessionIndex, 0, NULL, "send updateuserasset request");

		//开始节目了,进入抢票阶段
		//g_RoomSongProgramMgt.StartProgram(pMsgHead->nRoomID, pProcessOrderReq->nOrderID);
	}
	//拒绝
	else
	{
		//转移玩家的点歌费用了
		CFrameSession *pSession = NULL;
		nRet = g_Frame.CreateSession(MSGID_ISOT_UPDATEUSERASSET_RESP, enmSessionType_UpdateUserAsset, this, enmUpdateUserAssetTimeoutPeriod, pSession);
		if(nRet < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("Process order:create process order session is failed!{RoomID=%d, RoleID=%d, "
					"nIsAccept=%d}\n",
					pProcessOrderReq->nRoomID, pMsgHead->nRoleID, pProcessOrderReq->nOrderID, pProcessOrderReq->nIsAccept);

			stResp.nResult = E_UNKNOWN;
			SendMessageResponse(MSGID_RSCL_PROCESSORDER_RESP, pMsgHead, &stResp, enmTransType_P2P, nOptionLen, (char *)pOptionData);

			return nRet;
		}

		CProcessOrderSessionData * pProcessOrderData = new (pSession->GetSessionData())CProcessOrderSessionData();
		memcpy(&pProcessOrderData->m_stMsgHead, pMsgHead, sizeof(pProcessOrderData->m_stMsgHead));
		if(nOptionLen > 0)
		{
			pProcessOrderData->m_nOptionLen = nOptionLen;
			memcpy(pProcessOrderData->m_arrOptionData, pOptionData, nOptionLen);
		}
		memcpy(&pProcessOrderData->m_stMsgBody, pProcessOrderReq, sizeof(pProcessOrderData->m_stMsgBody));
		memcpy(&pProcessOrderData->stSongOrderInfo, pSongOrderInfo, sizeof(SongOrderInfo));

		//发送请求
		CUpdateUserAssetReq stUpdateUserAssetReq;
		stUpdateUserAssetReq.strTransID = pSongOrderInfo->strTransID;
		int32_t nUpdateIndex = 0;
		stUpdateUserAssetReq.arrAssetType[nUpdateIndex] = enmAssetType_Money;
		stUpdateUserAssetReq.arrUpdateAssetOperate[nUpdateIndex] = enmUpdateAssetOperate_Increase;
		stUpdateUserAssetReq.arrAssetValue[nUpdateIndex] = pSongOrderInfo->nOrderCost;
		stUpdateUserAssetReq.nCauseID = enmCauseID_RefuseRequestSong;
		stUpdateUserAssetReq.nRoleID = pSongOrderInfo->nRequestRoleID;
		stUpdateUserAssetReq.nUpdateCount = ++nUpdateIndex;

		SendMessageRequest(MSGID_OTIS_UPDATEUSERASSET_REQ , &stUpdateUserAssetReq, pMsgHead->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Item, pMsgHead->nRoleID,
				pSession->GetSessionIndex(), 0, NULL, "send updateuserasset request");
	}


	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

