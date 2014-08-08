/*
 * bll_event_requestsong.cpp
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#include "bll_event_requestsong.h"
#include "def/server_errordef.h"
#include "ctl/song_mgt.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "public_message_define.h"
#include "dal/billengine.h"

#include <string.h>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CRequestSongMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
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

int32_t CRequestSongMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_UpdateUserAsset:
			UpdateUserAssetTimeout(pSession);
		break;
		default:
		break;
	}

	return S_OK;
}

int32_t CRequestSongMessageEvent::UpdateUserAssetResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CRequestSongSessionData * pData =(CRequestSongSessionData *)(pSession->GetSessionData());

	CRequestSongResp stResp;
	CUpdateUserAssetResp *pUpdateUserAssetResp = dynamic_cast<CUpdateUserAssetResp *>(pMsgBody);
	if(NULL == pUpdateUserAssetResp)
	{
		WRITE_ERROR_LOG("null pointer:{pUpdateUserAssetResp=0x%08x}\n", pUpdateUserAssetResp);
		stResp.nResult = enmRequestSongResult_Unknown;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	if(pUpdateUserAssetResp->nResult != enmUpdateUserAsset_OK)
	{
		WRITE_ERROR_LOG("update user asset error!{nResult=%d}\n", pUpdateUserAssetResp->nResult);
		stResp.nResult = pUpdateUserAssetResp->nResult;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongOrderList *pSongOrderList = g_RoomSongOrderListMgt.GetRoomSongOrderList(pData->m_stMsgBody.nRoomID);
	if(pSongOrderList == NULL)
	{
		pSongOrderList = g_RoomSongOrderListMgt.CreateSongOrderList();
		if(pSongOrderList == NULL)
		{
			WRITE_ERROR_LOG("create room song order list failed!{SessionIndex=%d}\n", pSession->GetSessionIndex());
			stResp.nResult = enmRequestSongResult_Unknown;
			SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
			return S_OK;
		}
	}

	if(pSongOrderList->nOrderCount > MaxOrderListLength)
	{
		WRITE_ERROR_LOG("room song order list is too big!{nRoomID=%d, nOrderCount=%d, MaxOrderCount}\n",
				pData->m_stMsgBody.nRoomID, pSongOrderList->nOrderCount, MaxOrderListLength);
		stResp.nResult = enmRequestSongResult_Unknown;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	int64_t nCurTime = CDateTime::CurrentDateTime().Seconds();
	uint32_t nOrderID = g_RoomSongOrderListMgt.MakeOrderID();

	int32_t nIndex = pSongOrderList->nOrderCount;
	if(nIndex >= MaxOrderListLength)
	{
		//如果列表满了，就用最后一个元素
		nIndex = pSongOrderList->nOrderCount - 1;
		memmove(&pSongOrderList->arrSongOrderList[0], &pSongOrderList->arrSongOrderList[1],
				(MaxOrderListLength - 1) * sizeof(SongOrderInfo));
	}

	pSongOrderList->arrSongOrderList[nIndex].nOrderID = nOrderID;
	pSongOrderList->arrSongOrderList[nIndex].nOrderCost = pData->m_nRequestSongCost;
	pSongOrderList->arrSongOrderList[nIndex].nOrderStartTime = nCurTime;
	pSongOrderList->arrSongOrderList[nIndex].nOrderEndTime = 0;
	pSongOrderList->arrSongOrderList[nIndex].nOrderStatus = enmOrderStatus_Processing;
	pSongOrderList->arrSongOrderList[nIndex].nRoomID = pData->m_stMsgBody.nRoomID;
	pSongOrderList->arrSongOrderList[nIndex].nRequestRoleID = pData->m_stMsgHead.nRoleID;
	pSongOrderList->arrSongOrderList[nIndex].strRequestName = pData->strRequestName;
	pSongOrderList->arrSongOrderList[nIndex].nSongerRoleID = pData->m_stMsgBody.nSongerRoleID;
	pSongOrderList->arrSongOrderList[nIndex].strSongerName = pData->strSongerName;
	pSongOrderList->arrSongOrderList[nIndex].strSongName = pData->m_stMsgBody.strSongName;
	pSongOrderList->arrSongOrderList[nIndex].strWishWords = pData->m_stMsgBody.strWishWords;
	pSongOrderList->arrSongOrderList[nIndex].strTransID = pData->strTransID;
	pSongOrderList->arrSongOrderList[nIndex].nSongCount = pData->m_stMsgBody.nSongCount;

	if(pSongOrderList->nOrderCount < MaxOrderListLength)
	{
		++pSongOrderList->nOrderCount;
	}

	g_RoomSongOrderListMgt.UpdateRoomSongOrderList(pData->m_stMsgBody.nRoomID, pSongOrderList);

	stResp.nResult = enmRequestSongResult_OK;

	SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	CRequestSongNoti stNoti;
	stNoti.nOrderID = nOrderID;
	stNoti.nOrderTime = nCurTime;
	stNoti.nSongerRoleID = pData->m_stMsgBody.nSongerRoleID;
	stNoti.strSongerName = pData->strSongerName;
	stNoti.nRequestRoleID = pData->m_stMsgHead.nRoleID;
	stNoti.strRequestName = pData->strRequestName;
	stNoti.strSongName = pData->m_stMsgBody.strSongName;
	stNoti.strWishWords = pData->m_stMsgBody.strWishWords;
	stNoti.nSongCount = pData->m_stMsgBody.nSongCount;

	SendMessageNotifyToClient(MSGID_RSCL_REQUESTSONG_NOTI, &stNoti, pData->m_stMsgBody.nRoomID, enmBroadcastType_ExpectPlayr,
			pData->m_stMsgBody.nSongerRoleID);

	//生成数据库记录
	CUpdateSongOrderInfoNoti stUpdateSongOrderInfoNoti;
	stUpdateSongOrderInfoNoti.strTransID = pSongOrderList->arrSongOrderList[nIndex].strTransID;
	stUpdateSongOrderInfoNoti.nRoomID = pSongOrderList->arrSongOrderList[nIndex].nRoomID;
	stUpdateSongOrderInfoNoti.nOrderStartTime = pSongOrderList->arrSongOrderList[nIndex].nOrderStartTime;
	stUpdateSongOrderInfoNoti.nOrderEndTime = pSongOrderList->arrSongOrderList[nIndex].nOrderEndTime;
	stUpdateSongOrderInfoNoti.nOrderStatus = pSongOrderList->arrSongOrderList[nIndex].nOrderStatus;
	stUpdateSongOrderInfoNoti.strSongName = pSongOrderList->arrSongOrderList[nIndex].strSongName;
	stUpdateSongOrderInfoNoti.nSongerRoleID = pSongOrderList->arrSongOrderList[nIndex].nSongerRoleID;
	stUpdateSongOrderInfoNoti.strSongerName = pSongOrderList->arrSongOrderList[nIndex].strSongerName;
	stUpdateSongOrderInfoNoti.nRequestRoleID = pSongOrderList->arrSongOrderList[nIndex].nRequestRoleID;
	stUpdateSongOrderInfoNoti.strRequestName = pSongOrderList->arrSongOrderList[nIndex].strRequestName;
	stUpdateSongOrderInfoNoti.strWishWords = pSongOrderList->arrSongOrderList[nIndex].strWishWords;
	stUpdateSongOrderInfoNoti.nOrderCost = pSongOrderList->arrSongOrderList[nIndex].nOrderCost;
	stUpdateSongOrderInfoNoti.nSongCount = pSongOrderList->arrSongOrderList[nIndex].nSongCount;

	SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pData->m_stMsgBody.nRoomID,
			enmTransType_ByKey, pData->m_stMsgHead.nRoleID, enmEntityType_DBProxy, pData->m_stMsgHead.nRoleID);

	return S_OK;
}

int32_t CRequestSongMessageEvent::UpdateUserAssetTimeout(CFrameSession *pSession)
{
	CRequestSongSessionData * pData =(CRequestSongSessionData *)(pSession->GetSessionData());

	WRITE_WARNING_LOG("request song time out{nRoleID=%d, nSongerRoleID=%d, RoomID=%d, "
			"strSongName=%s, strWishWords=%s, nSessionIndex=%d}",
			pData->m_stMsgHead.nRoleID, pData->m_stMsgBody.nSongerRoleID, pData->m_stMsgBody.nRoomID,
			pData->m_stMsgBody.strSongName.GetString(), pData->m_stMsgBody.strWishWords.GetString(),
			pSession->GetSessionIndex());

	CRequestSongResp stResp;
	stResp.nResult = enmRequestSongResult_Timeout;
	SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

int32_t CRequestSongMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CRequestSongResp stResp;

	CRequestSongReq *pRequestSongReq = dynamic_cast<CRequestSongReq *>(pMsgBody);
	if(NULL == pRequestSongReq)
	{
		WRITE_ERROR_LOG("null pointer:{pRequestSongReq=0x%08x}\n",pRequestSongReq);
		return E_NULLPOINTER;
	}

	if(pRequestSongReq->nSongCount == 0)
	{
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	CPlayer *pRequestPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pRequestPlayer, nPlayerIndex);
	if(nRet < 0 || pRequestPlayer == NULL)
	{
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	CPlayer *pSongerPlayer = NULL;
//	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	nRet = g_PlayerMgt.GetPlayer(pRequestSongReq->nSongerRoleID, pSongerPlayer, nPlayerIndex);
	if(nRet < 0 || pSongerPlayer == NULL)
	{
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//不是艺人
	IdentityType nIdentityType = pSongerPlayer->GetIdentityType();
	if(((nIdentityType & enmIdentityType_SONGER) == 0) &&
			((nIdentityType & enmIdentityType_DANCER) == 0) &&
			((nIdentityType & enmIdentityType_HOST) == 0))
	{
		WRITE_WARNING_LOG("it's not permission request song to other players!{nRoleID=%d, nRoomID=%d, "
				"SongerRoleID=%d, SongerName=%s, IdentityType=%d}!\n",
				pMsgHead->nRoleID, pRequestSongReq->nRoomID, pRequestSongReq->nSongerRoleID,
				pRequestSongReq->strSongName.GetString(), nIdentityType);
		stResp.nResult = enmRequestSongResult_NoPermission;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//到数据库获取信息
	CFrameSession *pSession = NULL;
	nRet = g_Frame.CreateSession(MSGID_ISOT_UPDATEUSERASSET_RESP, enmSessionType_UpdateUserAsset, this, enmUpdateUserAssetTimeoutPeriod, pSession);
	if(nRet < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("request song:create update user asset session is failed!{RoomID=%d, RoleID=%d, "
				"SongerRoleID=%d, strSongName=%s}\n",
				pMsgHead->nRoomID, pMsgHead->nRoleID, pRequestSongReq->nSongerRoleID, pRequestSongReq->strSongName.GetString());

		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_REQUESTSONG_RESP, pMsgHead, &stResp, enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return nRet;
	}

	char szTransID[enmMaxSerialNumberLength];
	g_BillEngine.GenerateTransID(szTransID);

	CString<enmMaxSerialNumberLength> strTransID(szTransID);

	CRequestSongSessionData * pRequestSongData = new (pSession->GetSessionData())CRequestSongSessionData();
	memcpy(&pRequestSongData->m_stMsgHead, pMsgHead, sizeof(pRequestSongData->m_stMsgHead));
	if(nOptionLen > 0)
	{
		pRequestSongData->m_nOptionLen = nOptionLen;
		memcpy(pRequestSongData->m_arrOptionData, pOptionData, nOptionLen);
	}
	memcpy(&pRequestSongData->m_stMsgBody, pRequestSongReq, sizeof(pRequestSongData->m_stMsgBody));
	pRequestSongData->strTransID = strTransID;
	pRequestSongData->strSongerName.SetString(pSongerPlayer->GetRoleName());
	pRequestSongData->strRequestName.SetString(pRequestPlayer->GetRoleName());
	pRequestSongData->m_nRequestSongCost = g_SomeConfig.GetRequestSongCost() * pRequestSongReq->nSongCount;

	//发送请求
	CUpdateUserAssetReq stUpdateUserAssetReq;
	stUpdateUserAssetReq.strTransID = strTransID;
	stUpdateUserAssetReq.nRoleID = pMsgHead->nRoleID;
	stUpdateUserAssetReq.nCauseID = enmCauseID_RequestSong;
	stUpdateUserAssetReq.nUpdateCount = 1;
	stUpdateUserAssetReq.arrAssetType[0] = enmAssetType_Money;
	stUpdateUserAssetReq.arrUpdateAssetOperate[0] = enmUpdateAssetOperate_Reduce;
	stUpdateUserAssetReq.arrAssetValue[0] = pRequestSongData->m_nRequestSongCost;

	SendMessageRequest(MSGID_OTIS_UPDATEUSERASSET_REQ , &stUpdateUserAssetReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Item, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send updateuserasset request");

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

