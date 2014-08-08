/*
 * bll_event_getorderlist.cpp
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#include "bll_event_getorderlist.h"
#include "def/server_errordef.h"
#include "ctl/song_mgt.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CGetOrderListMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CGetOrderListMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CGetOrderListMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CGetOrderListReq *pGetOrderListReq = dynamic_cast<CGetOrderListReq *>(pMsgBody);
	if(NULL == pGetOrderListReq)
	{
		WRITE_ERROR_LOG("null pointer:{pGetOrderListReq=0x%08x}\n",pGetOrderListReq);
		return E_NULLPOINTER;
	}

	CGetOrderListResp stResp;

	CPlayer *pRequestPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pRequestPlayer, nPlayerIndex);
	if(nRet < 0 || pRequestPlayer == NULL)
	{
		WRITE_WARNING_LOG("it's not found request player!{nRoleID=%d, nRoomID=%d}\n",
				pMsgHead->nRoleID, pGetOrderListReq->nRoomID);
		stResp.nResult = 1;
		SendMessageResponse(MSGID_RSCL_GETORDERLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongOrderList *pSongOrderList = g_RoomSongOrderListMgt.GetRoomSongOrderList(pGetOrderListReq->nRoomID);
	if(pSongOrderList == NULL)
	{
		WRITE_WARNING_LOG("it's not found song order list!{nRoleID=%d, nRoomID=%d}\n",
				pMsgHead->nRoleID, pGetOrderListReq->nRoomID);
		stResp.nResult = S_OK;
		stResp.nOrderCount = 0;
		SendMessageResponse(MSGID_RSCL_GETORDERLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	int32_t nOrderCount = (pSongOrderList->nOrderCount > MaxOrderListLength) ? MaxOrderListLength : pSongOrderList->nOrderCount;

	stResp.nResult = S_OK;
	stResp.nOrderCount = nOrderCount;
	for(int32_t i = 0; i < nOrderCount; ++i)
	{
		//构造响应包
		stResp.arrOrderID[i] = pSongOrderList->arrSongOrderList[i].nOrderID;
		stResp.arrOrderTime[i] = pSongOrderList->arrSongOrderList[i].nOrderStartTime;
		stResp.arrSongerRoleID[i] = pSongOrderList->arrSongOrderList[i].nSongerRoleID;
		stResp.arrSongerName[i] = pSongOrderList->arrSongOrderList[i].strSongerName;
		stResp.arrRequestRoleID[i] = pSongOrderList->arrSongOrderList[i].nRequestRoleID;
		stResp.arrRequestName[i] = pSongOrderList->arrSongOrderList[i].strRequestName;
		stResp.arrWishWords[i] = pSongOrderList->arrSongOrderList[i].strWishWords;
		stResp.arrSongName[i] = pSongOrderList->arrSongOrderList[i].strSongName;
		stResp.arrOrderStatus[i] = pSongOrderList->arrSongOrderList[i].nOrderStatus;
		stResp.arrSongCount[i] = pSongOrderList->arrSongOrderList[i].nSongCount;
	}

	SendMessageResponse(MSGID_RSCL_GETORDERLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

