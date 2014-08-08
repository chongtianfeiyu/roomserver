/*
 * bll_timer_clearroomorderinfo.cpp
 *
 *  Created on: 2013-3-12
 *      Author: jimm
 */

#include "bll_timer_clearroomorderinfo.h"
#include "ctl/server_datacenter.h"
#include "public_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//消息事件
int32_t CClearRoomOrderInfoTimer::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

//会话事件激活
int32_t CClearRoomOrderInfoTimer::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
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

int32_t CClearRoomOrderInfoTimer::OnSessionTimeOut(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	switch(pSession->GetSessionType())
	{
	case enmSessionType_ClearRoomOrderInfo:
		ClearRoomOrderInfo(pSession);
		break;
	case enmSessionType_UpdateUserAsset:
		UpdateUserAssetTimeout(pSession);
		break;
	}

	return nRet;
}

int32_t CClearRoomOrderInfoTimer::ClearRoomOrderInfo(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CClearRoomOrderInfoSessionData * pData =(CClearRoomOrderInfoSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("clear room order info!{RoomID=%d, SongerRoleID=%d}\n",
			pData->nRoomID, pData->nSongerRoleID);

	DeleteTimerInfo(pData->nSongerRoleID);

	SongOrderList *pSongOrderList = g_RoomSongOrderListMgt.GetRoomSongOrderList(pData->nRoomID);
	if(pSongOrderList == NULL)
	{
		WRITE_ERROR_LOG("it's not found song order list!{RoomID=%d, SongerRoleID=%d}\n", pData->nRoomID,
				pData->nSongerRoleID);
		return nRet;
	}

	CProcessOrderNoti stNoti;

	uint16_t nOrderCount = 0;
	SongOrderInfo arrSongOrderList[MaxOrderListLength];

	for(int32_t i = 0; i < pSongOrderList->nOrderCount; ++i)
	{
		if(pSongOrderList->arrSongOrderList[i].nSongerRoleID == pData->nSongerRoleID)
		{
			//未完成的订单
			if(pSongOrderList->arrSongOrderList[i].nOrderStatus == enmOrderStatus_Processing)
			{
				stNoti.nOrderID = pSongOrderList->arrSongOrderList[i].nOrderID;
				stNoti.nIsAccept = enmOrderStatus_Refused;
				stNoti.nSongerRoleID = pSongOrderList->arrSongOrderList[i].nSongerRoleID;
				stNoti.strSongerName = pSongOrderList->arrSongOrderList[i].strSongerName;
				stNoti.nRequestRoleID = pSongOrderList->arrSongOrderList[i].nRequestRoleID;
				stNoti.strRequestName = pSongOrderList->arrSongOrderList[i].strRequestName;
				stNoti.strSongName = pSongOrderList->arrSongOrderList[i].strSongName;
				stNoti.strWishWords = pSongOrderList->arrSongOrderList[i].strWishWords;
				stNoti.nSongCount = pSongOrderList->arrSongOrderList[i].nSongCount;

				SendMessageNotifyToClient(MSGID_RSCL_PROCESSORDER_NOTI, &stNoti, pData->nRoomID,
						enmBroadcastType_ExpectPlayr, pSongOrderList->arrSongOrderList[i].nRequestRoleID);

				//转移玩家的点歌费用了
				CFrameSession *pSession = NULL;
				nRet = g_Frame.CreateSession(MSGID_ISOT_UPDATEUSERASSET_RESP, enmSessionType_UpdateUserAsset, this, enmUpdateUserAssetTimeoutPeriod, pSession);
				if(nRet < 0 || pSession == NULL)
				{
					WRITE_ERROR_LOG("Process order:create process order session is failed!{RoomID=%d, RequestID=%d, "
							"nSongerRoleID=%s, nOrderID=%u, SongName=%s, nTransID=%s}\n",
							pSongOrderList->arrSongOrderList[i].nRoomID, pSongOrderList->arrSongOrderList[i].nRequestRoleID,
							pSongOrderList->arrSongOrderList[i].nSongerRoleID, pSongOrderList->arrSongOrderList[i].nOrderID,
							pSongOrderList->arrSongOrderList[i].strSongName.GetString(), pSongOrderList->arrSongOrderList[i].strTransID.GetString());

					continue;
				}

				CUpdateUserAssetSessionData * pUpdateUserAssetData = new (pSession->GetSessionData())CUpdateUserAssetSessionData();
				memcpy(&pUpdateUserAssetData->stSongOrderInfo, &pSongOrderList->arrSongOrderList[i], sizeof(SongOrderInfo));

				//发送请求
				CUpdateUserAssetReq stUpdateUserAssetReq;
				stUpdateUserAssetReq.strTransID = pSongOrderList->arrSongOrderList[i].strTransID;
				int32_t nUpdateIndex = 0;
				stUpdateUserAssetReq.arrAssetType[nUpdateIndex] = enmAssetType_Money;
				stUpdateUserAssetReq.arrUpdateAssetOperate[nUpdateIndex] = enmUpdateAssetOperate_Increase;
				stUpdateUserAssetReq.arrAssetValue[nUpdateIndex] = pSongOrderList->arrSongOrderList[i].nOrderCost;

				//拒绝
				stUpdateUserAssetReq.nCauseID = enmCauseID_RefuseRequestSong;
				stUpdateUserAssetReq.nRoleID = pSongOrderList->arrSongOrderList[i].nRequestRoleID;
				stUpdateUserAssetReq.nUpdateCount = ++nUpdateIndex;

				SendMessageRequest(MSGID_OTIS_UPDATEUSERASSET_REQ , &stUpdateUserAssetReq, pData->nRoomID,
						enmTransType_ByKey, pSongOrderList->arrSongOrderList[i].nRequestRoleID, enmEntityType_Item,
						pSongOrderList->arrSongOrderList[i].nRequestRoleID, pSession->GetSessionIndex(),
						0, NULL, "send updateuserasset request");
			}
			//已完成的订单
			else
			{

			}
		}
		else
		{
			arrSongOrderList[nOrderCount] = pSongOrderList->arrSongOrderList[i];
			++nOrderCount;
		}
	}

	memcpy(pSongOrderList->arrSongOrderList, arrSongOrderList, nOrderCount * sizeof(SongOrderInfo));
	pSongOrderList->nOrderCount = nOrderCount;

	return nRet;
}

int32_t CClearRoomOrderInfoTimer::UpdateUserAssetResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CUpdateUserAssetSessionData * pData =(CUpdateUserAssetSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("songer is not exist!so update requester asset!{strTransID=%s, "
			"nRoomID=%d, nRequestRoleID=%d, nSongerRoleID=%d, strSongName=%s}\n",
			pData->stSongOrderInfo.strTransID.GetString(), pData->stSongOrderInfo.nRoomID,
			pData->stSongOrderInfo.nRequestRoleID, pData->stSongOrderInfo.nSongerRoleID,
			pData->stSongOrderInfo.strSongName.GetString());

	CProcessOrderNoti stNoti;
	stNoti.nOrderID = pData->stSongOrderInfo.nOrderID;
	stNoti.nIsAccept = enmOrderStatus_Refused;
	stNoti.nSongerRoleID = pData->stSongOrderInfo.nSongerRoleID;
	stNoti.strSongerName = pData->stSongOrderInfo.strSongerName;
	stNoti.nRequestRoleID = pData->stSongOrderInfo.nRequestRoleID;
	stNoti.strRequestName = pData->stSongOrderInfo.strRequestName;
	stNoti.strSongName = pData->stSongOrderInfo.strSongName;
	stNoti.strWishWords = pData->stSongOrderInfo.strWishWords;
	stNoti.nSongCount = pData->stSongOrderInfo.nSongCount;

	SendMessageNotifyToClient(MSGID_RSCL_PROCESSORDER_NOTI, &stNoti, pData->stSongOrderInfo.nRoomID,
			enmBroadcastType_ExpectPlayr, pData->stSongOrderInfo.nRequestRoleID);

	OrderStatus nOrderStatus = enmOrderStatus_Refused;
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

	SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pData->stSongOrderInfo.nRoomID,
			enmTransType_ByKey, pData->stSongOrderInfo.nRequestRoleID, enmEntityType_DBProxy, pData->stSongOrderInfo.nRequestRoleID);

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pData->stSongOrderInfo.nRoomID, pData->stSongOrderInfo.nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("update song order info, but it's not found song order info!"
				"{strTransID=%s, nRequestRoleID=%d, nSongerRoleID=%d, nRoomID=%d, nOrderID=%u}\n",
				pData->stSongOrderInfo.strTransID.GetString(), pData->stSongOrderInfo.nRequestRoleID,
				pData->stSongOrderInfo.nSongerRoleID, pData->stSongOrderInfo.nRoomID,
				pData->stSongOrderInfo.nOrderID);
		return S_OK;
	}

	//更新内存记录
	pSongOrderInfo->nOrderEndTime = nCurTime;
	pSongOrderInfo->nOrderStatus = nOrderStatus;

	return nRet;
}

int32_t CClearRoomOrderInfoTimer::UpdateUserAssetTimeout(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CUpdateUserAssetSessionData * pData =(CUpdateUserAssetSessionData *)(pSession->GetSessionData());

	WRITE_ERROR_LOG("songer is not exist!so update requester asset, but timeout!{strTransID=%s, "
			"nRoomID=%d, nRequestRoleID=%d, nSongerRoleID=%d, strSongName=%s}\n",
			pData->stSongOrderInfo.strTransID.GetString(), pData->stSongOrderInfo.nRoomID,
			pData->stSongOrderInfo.nRequestRoleID, pData->stSongOrderInfo.nSongerRoleID,
			pData->stSongOrderInfo.strSongName.GetString());

	return nRet;
}

int32_t CClearRoomOrderInfoTimer::CreateTimer(RoomID nRoomID, RoleID nSongerRoleID)
{
	WRITE_INFO_LOG("songer leave from public mic, so we'll create timer!{RoomID=%d, SongerRoleID=%d}\n",
			nRoomID, nSongerRoleID);

	SongOrderList *pSongOrderList = g_RoomSongOrderListMgt.GetRoomSongOrderList(nRoomID);
	if(pSongOrderList != NULL)
	{
		CFrameSession *pSession = NULL;
		int32_t nRet = g_Frame.CreateSession(0, enmSessionType_ClearRoomOrderInfo, this, enmClearRoomOrderInfoPeriod, pSession);
		if(nRet < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("songer leave from public mic, but create timer failed!{nRet=0x08x}\n", nRet);
			return E_NULLPOINTER;
		}

		CClearRoomOrderInfoSessionData * pData = new (pSession->GetSessionData())CClearRoomOrderInfoSessionData();
		pData->nRoomID = nRoomID;
		pData->nSongerRoleID = nSongerRoleID;

		if(m_nTimerCount < MaxSongerTimerCount)
		{
			m_arrSongerTimerInfo[m_nTimerCount].nRoomID = nRoomID;
			m_arrSongerTimerInfo[m_nTimerCount].nSongerRoleID = nSongerRoleID;
			m_arrSongerTimerInfo[m_nTimerCount].nSessionIndex = pSession->GetSessionIndex();
			++m_nTimerCount;
		}
	}

	return S_OK;
}

int32_t CClearRoomOrderInfoTimer::DeleteTimer(RoleID nSongerRoleID)
{
	WRITE_INFO_LOG("songer get a public mic, so we'll delete she's all timer!{SongerRoleID=%d}\n", nSongerRoleID);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(nSongerRoleID, pPlayer, nPlayerIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("it's not found songer player!{nRet=0x08x}\n", nRet);
		return nRet;
	}

	IdentityType nIdentityType = pPlayer->GetIdentityType();
	if(((nIdentityType & enmIdentityType_SONGER) == 0) &&
			((nIdentityType & enmIdentityType_DANCER) == 0) &&
			((nIdentityType & enmIdentityType_HOST) == 0))
	{
		WRITE_ERROR_LOG("songer's identity is not songer!{nIdentityType=%d}\n", nIdentityType);
		return nRet;
	}

	int32_t nDelIndex = -1;
	for(int32_t i = 0; i < m_nTimerCount; ++i)
	{
		if(m_arrSongerTimerInfo[i].nSongerRoleID == nSongerRoleID)
		{
			g_Frame.RemoveSession(m_arrSongerTimerInfo[i].nSessionIndex);
			nDelIndex = i;
			break;
		}
	}

	if(nDelIndex >= 0)
	{
		//表示不是最后一个元素
		if(nDelIndex < m_nTimerCount - 1)
		{
			m_arrSongerTimerInfo[nDelIndex] = m_arrSongerTimerInfo[m_nTimerCount - 1];
		}

		--m_nTimerCount;
	}

	return S_OK;
}

int32_t CClearRoomOrderInfoTimer::DeleteTimerInfo(RoleID nSongerRoleID)
{
	WRITE_INFO_LOG("delete clear room order timer info!{SongerRoleID=%d}\n", nSongerRoleID);

	int32_t nDelIndex = -1;
	for(int32_t i = 0; i < m_nTimerCount; ++i)
	{
		if(m_arrSongerTimerInfo[i].nSongerRoleID == nSongerRoleID)
		{
			nDelIndex = i;
			break;
		}
	}

	if(nDelIndex >= 0)
	{
		//表示不是最后一个元素
		if(nDelIndex < m_nTimerCount - 1)
		{
			m_arrSongerTimerInfo[nDelIndex] = m_arrSongerTimerInfo[m_nTimerCount - 1];
		}

		--m_nTimerCount;
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

