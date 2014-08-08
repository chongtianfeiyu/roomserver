/*
 * bll_event_getsonglist.cpp
 *
 *  Created on: 2013-3-10
 *      Author: jimm
 */

#include "bll_event_getsonglist.h"
#include "def/server_errordef.h"
#include "ctl/song_mgt.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CGetSongListMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetSongList:
			GetSongListResp(pMsgHead, pMsgBody, pSession);
		break;
		default:
		break;
	}

	return S_OK;
}

int32_t CGetSongListMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetSongList:
			GetSongListTimeout(pSession);
		break;
		default:
		break;
	}

	return S_OK;
}

int32_t CGetSongListMessageEvent::GetSongListResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CGetSongListSessionData * pData =(CGetSongListSessionData *)(pSession->GetSessionData());

	CGetSongListResp stResp;
	CGetSongListFromDBResp *pGetSongListFromDBResp = dynamic_cast<CGetSongListFromDBResp *>(pMsgBody);
	if(NULL == pGetSongListFromDBResp)
	{
		WRITE_ERROR_LOG("null pointer:{pGetSongListFromDBResp=0x%08x}\n", pGetSongListFromDBResp);
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);
		return E_NULLPOINTER;
	}

	if(pGetSongListFromDBResp->nResult < 0)
	{
		stResp.nResult = E_UNKNOWN;
	}
	else
	{
		stResp.nResult = S_OK;
		stResp.nSongerRoleID = pData->m_stMsgBody.nSongerRoleID;
		stResp.nSongCount = pGetSongListFromDBResp->nSongCount;
		for(int32_t i = 0; i < pGetSongListFromDBResp->nSongCount; ++i)
		{
			stResp.arrSongName[i] = pGetSongListFromDBResp->arrSongName[i];
			stResp.arrIsHot[i] = pGetSongListFromDBResp->arrSongAttr[i];
		}

		//从数据库取歌单的，一定是第一次来取，内存里没有找到
		SongList *pSongList = g_PlayerSongListMgt.GetPlayerSongList(pData->m_stMsgBody.nSongerRoleID);
		if(pSongList == NULL)
		{
			pSongList = g_PlayerSongListMgt.CreateSongList();
			if(pSongList != NULL)
			{
				for(int32_t i = 0; i < pGetSongListFromDBResp->nSongCount; ++i)
				{
					pSongList->m_arrSongList[i].strSongName = pGetSongListFromDBResp->arrSongName[i];
					pSongList->m_arrSongList[i].nSongAttr = pGetSongListFromDBResp->arrSongAttr[i];
				}
				pSongList->m_nSongCount = pGetSongListFromDBResp->nSongCount;

				g_PlayerSongListMgt.UpdatePlayerSongList(pData->m_stMsgBody.nSongerRoleID, pSongList);
			}
		}
	}

	SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

int32_t CGetSongListMessageEvent::GetSongListTimeout(CFrameSession *pSession)
{
	CGetSongListSessionData * pData =(CGetSongListSessionData *)(pSession->GetSessionData());

	WRITE_WARNING_LOG("get song list time out{nRoleID=%d, nSongerRoleID=%d, RoomID=%d, nSessionIndex=%d}",
			pData->m_stMsgHead.nRoleID, pData->m_stMsgBody.nSongerRoleID, pData->m_stMsgBody.nRoomID, pSession->GetSessionIndex());

	CGetSongListResp stResp;
	stResp.nResult = E_UNKNOWN;
	SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, &pData->m_stMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

int32_t CGetSongListMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CGetSongListReq *pGetSongListReq = dynamic_cast<CGetSongListReq *>(pMsgBody);
	if(NULL == pGetSongListReq)
	{
		WRITE_ERROR_LOG("null pointer:{pGetSongListReq=0x%08x}\n",pGetSongListReq);
		return E_NULLPOINTER;
	}

	CGetSongListResp stResp;

	CPlayer *pRequestPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pRequestPlayer, nPlayerIndex);
	if(nRet < 0 || pRequestPlayer == NULL)
	{
		WRITE_WARNING_LOG("it's not found request player!{nRoleID=%d, nRoomID=%d}\n",
				pMsgHead->nRoleID, pGetSongListReq->nRoomID);
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	CPlayer *pSongerPlayer = NULL;
//	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	nRet = g_PlayerMgt.GetPlayer(pGetSongListReq->nSongerRoleID, pSongerPlayer, nPlayerIndex);
	if(nRet < 0 || pSongerPlayer == NULL)
	{
		WRITE_WARNING_LOG("it's not found songer!{nRoleID=%d, nRoomID=%d}\n",
				pGetSongListReq->nSongerRoleID, pGetSongListReq->nRoomID);
		stResp.nResult = E_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);
		return S_OK;
	}

	SongList *pSongList = g_PlayerSongListMgt.GetPlayerSongList(pGetSongListReq->nSongerRoleID);
	if(pSongList == NULL)
	{
		//到数据库获取信息
		CFrameSession *pSession = NULL;
		nRet = g_Frame.CreateSession(MSGID_DBRS_GETSONGLISTFROMDB_RESP, enmSessionType_GetSongList, this, enmGetSongListTimeoutPeriod, pSession);
		if(nRet < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("get song list:create get song list session is failed!{RoomID=%d, RoleID=%d, "
					"SongerRoleID=%d}\n",
					pMsgHead->nRoomID, pMsgHead->nRoleID, pGetSongListReq->nSongerRoleID);

			stResp.nResult = E_UNKNOWN;
			SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P, nOptionLen, (char *)pOptionData);

			return nRet;
		}

		CGetSongListSessionData * pGetSongListData = new (pSession->GetSessionData())CGetSongListSessionData();
		memcpy(&pGetSongListData->m_stMsgHead, pMsgHead, sizeof(pGetSongListData->m_stMsgHead));
		if(nOptionLen > 0)
		{
			pGetSongListData->m_nOptionLen = nOptionLen;
			memcpy(pGetSongListData->m_arrOptionData, pOptionData, nOptionLen);
		}
		memcpy(&pGetSongListData->m_stMsgBody, pGetSongListReq, sizeof(pGetSongListData->m_stMsgBody));

		CGetSongListReq CGetSongListFromDBReq;
		CGetSongListFromDBReq.nRoomID = pGetSongListReq->nRoomID;
		CGetSongListFromDBReq.nSongerRoleID = pGetSongListReq->nSongerRoleID;

		SendMessageRequest(MSGID_RSDB_GETSONGLISTFROMDB_REQ , &CGetSongListFromDBReq, pMsgHead->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
				pSession->GetSessionIndex(), 0, NULL, "send getsonglist request");
		return S_OK;
	}

	stResp.nResult = S_OK;
	stResp.nSongerRoleID = pGetSongListReq->nSongerRoleID;
	stResp.nSongCount = pSongList->m_nSongCount;
	for(int32_t i = 0; i < pSongList->m_nSongCount; ++i)
	{
		//构造响应包
		stResp.arrSongName[i] = pSongList->m_arrSongList[i].strSongName;
		stResp.arrIsHot[i] = pSongList->m_arrSongList[i].nSongAttr;
	}

	SendMessageResponse(MSGID_RSCL_GETSONGLIST_RESP, pMsgHead, &stResp, enmTransType_P2P);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

