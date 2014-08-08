/*
 * bll_event_updatesonglist.cpp
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#include "bll_event_updatesonglist.h"
#include "def/server_errordef.h"
#include "ctl/song_mgt.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUpdateSongListMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CUpdateSongListMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CUpdateSongListMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CUpdateSongListReq *pUpdateSongListReq = dynamic_cast<CUpdateSongListReq *>(pMsgBody);
	if(NULL == pUpdateSongListReq)
	{
		WRITE_ERROR_LOG("null pointer:{pUpdateSongListReq=0x%08x}\n",pUpdateSongListReq);
		return E_NULLPOINTER;
	}

	CUpdateSongListResp stResp;

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(nRet < 0 || pPlayer == NULL)
	{
		WRITE_WARNING_LOG("it's not found request player!{nRoleID=%d, nRoomID=%d}\n",
				pMsgHead->nRoleID, pUpdateSongListReq->nRoomID);
		stResp.nResult = enmUpdateSongListResult_Unknown;
		SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	//不是艺人
	IdentityType nIdentityType = pPlayer->GetIdentityType();
	if(((nIdentityType & enmIdentityType_SONGER) == 0) &&
			((nIdentityType & enmIdentityType_DANCER) == 0) &&
			((nIdentityType & enmIdentityType_HOST) == 0))
	{
		WRITE_WARNING_LOG("she is not a songer!{nRoleID=%d, nRoomID=%d, IdentityType=%d}!\n",
				pMsgHead->nRoleID, pUpdateSongListReq->nRoomID, nIdentityType);
		stResp.nResult = enmUpdateSongListResult_NoPermission;
		SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongList *pSongList = g_PlayerSongListMgt.GetPlayerSongList(pMsgHead->nRoleID);
	if(pSongList == NULL)
	{
		pSongList = g_PlayerSongListMgt.CreateSongList();
		if(pSongList == NULL)
		{
			WRITE_WARNING_LOG("create song list failed!{nRoleID=%d, nRoomID=%d}\n",
					pMsgHead->nRoleID, pUpdateSongListReq->nRoomID);
			stResp.nResult = enmUpdateSongListResult_Unknown;
			SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
			return S_OK;
		}
	}

	if(pUpdateSongListReq->nSongCount > MaxSongCountPerPlayer)
	{
		WRITE_WARNING_LOG("update song count is too much!{nRoleID=%d, nRoomID=%d, nSongCount=%d, MaxSongCountPerPlayer=%d}\n",
				pMsgHead->nRoleID, pUpdateSongListReq->nRoomID, pUpdateSongListReq->nSongCount, MaxSongCountPerPlayer);
		stResp.nResult = enmUpdateSongListResult_Full;
		SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	pSongList->m_nSongCount = pUpdateSongListReq->nSongCount;

	//判断热推歌曲是否已满
	int32_t nHotSongCount = 0;
	for(int32_t i = 0; i < pUpdateSongListReq->nSongCount; ++i)
	{
		if(pUpdateSongListReq->arrIsHot[i] != 0)
		{
			++nHotSongCount;
		}
		if(nHotSongCount > MaxHotSongCountPerPlayer)
		{
			WRITE_WARNING_LOG("hot song count is too much!{nRoleID=%d, nRoomID=%d}\n",
					pMsgHead->nRoleID, pUpdateSongListReq->nRoomID);
			stResp.nResult = enmUpdateSongListResult_HotFull;
			SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
			return S_OK;
		}
	}

	stResp.nResult = enmUpdateSongListResult_OK;
	stResp.nSongCount = pUpdateSongListReq->nSongCount;
	for(int32_t i = 0; i < pUpdateSongListReq->nSongCount; ++i)
	{
		//更新内存
		pSongList->m_arrSongList[i].strSongName = pUpdateSongListReq->arrSongName[i];
		pSongList->m_arrSongList[i].nSongAttr = pUpdateSongListReq->arrIsHot[i];

		//构造响应包
		stResp.arrSongName[i] = pUpdateSongListReq->arrSongName[i];
		stResp.arrIsHot[i] = pUpdateSongListReq->arrIsHot[i];
	}

	g_PlayerSongListMgt.UpdatePlayerSongList(pMsgHead->nRoleID, pSongList);

	SendMessageResponse(MSGID_RSCL_UPDATESONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);

	//更新数据库
	CUpdateSongListNoti stNoti;
	stNoti.nRoomID = pUpdateSongListReq->nRoomID;
	stNoti.nSongerRoleID = pMsgHead->nRoleID;
	stNoti.nSongCount = pSongList->m_nSongCount;
	for(int32_t i = 0; i < pSongList->m_nSongCount; ++i)
	{
		stNoti.arrSongIndex[i] = i;
		stNoti.arrSongName[i] = pSongList->m_arrSongList[i].strSongName;
		stNoti.arrSongAttr[i] = pSongList->m_arrSongList[i].nSongAttr;
	}
	SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGLIST_NOTI, &stNoti, pUpdateSongListReq->nRoomID, enmTransType_ByKey,
			pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

