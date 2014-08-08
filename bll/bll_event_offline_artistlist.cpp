/*
 * bll_event_offline_artistlist.cpp
 *
 *  Created on: 2013-5-6
 *      Author: liufl
 */

#include "bll_event_offline_artistlist.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN
int32_t CGetArtistMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{

	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	CGetArtistInfoSessionData * pData =(CGetArtistInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}

	CGetRoomArtistResp *pGetRoomArtistResp = dynamic_cast<CGetRoomArtistResp *>(pMsgBody);
	if(NULL == pGetRoomArtistResp)
	{
		WRITE_ERROR_LOG("null pointer:{pGetRoomArtistResp=0x%08x}\n",pGetRoomArtistResp);
		return E_NULLPOINTER;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_pMsgBody.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("room is not on this server!{nRoomID=%d, ret=0x%08x}\n",pData->m_pMsgBody.nRoomID,ret);
		return ret;
	}
	//初始化主播列表
	for(int32_t i = 0 ; i < pGetRoomArtistResp->nCount; ++i)
	{
		RoomUserInfo     nRoomUserInfo;
		nRoomUserInfo.nRoleID = pGetRoomArtistResp->arrRoleID[i];
		nRoomUserInfo.nGender = pGetRoomArtistResp->arrGender[i];
		nRoomUserInfo.n179ID  = pGetRoomArtistResp->arr179ID[i];
		nRoomUserInfo.strRoleName = pGetRoomArtistResp->strRoleName[i];
		nRoomUserInfo.nVipLevel = pGetRoomArtistResp->arrVipLevel[i];
		nRoomUserInfo.nRoleRank = pGetRoomArtistResp->arrRoleRank[i];
		nRoomUserInfo.nIdentityType = pGetRoomArtistResp->arrIdentityType[i];
		nRoomUserInfo.nClientInfo = pGetRoomArtistResp->arrClientInfo[i];
		nRoomUserInfo.nStatus = 0;
		nRoomUserInfo.nItemCount = 0;
		nRoomUserInfo.nMagnateLevel = pGetRoomArtistResp->arrMagnateLevel[i];

		pRoom->m_RoomArtistList.AddArtistInfo(nRoomUserInfo, enmOffline_stat);
	}

	//回应客户端
	CGetArtistListResp stArtistListResp;


	stArtistListResp.nCount = pRoom->m_RoomArtistList.GetOfflineArtistList(stArtistListResp.stRoomUserInfo);

	SendMessageResponse(MSGID_CLRM_GETARTISTOFFLINE_RESP, &(pData->m_sMsgHead), &stArtistListResp,
			enmTransType_P2P, nOptionLen, (char *)pOptionData);

	return S_OK;
}
int32_t CGetArtistMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CGetArtistMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CGetArtistListReq *pGetArtistReq = dynamic_cast<CGetArtistListReq *>(pMsgBody);
	if(NULL == pGetArtistReq)
	{
		WRITE_ERROR_LOG("null pointer:{pGetArtistReq=0x%08x}\n",pGetArtistReq);
		return E_NULLPOINTER;
	}

	CGetArtistListResp stArtistListResp;
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pGetArtistReq->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("room is not on this server!{nRoomID=%d, ret=0x%08x}\n", pGetArtistReq->nRoomID,ret);
		return ret;
	}
	if(pRoom->m_RoomArtistList.nArtistCount == 0) //没有艺人信息，从DB获取
	{
		//到数据库获取房间信息
		CFrameSession *pSession = NULL;
		ret = g_Frame.CreateSession(MSGID_RSDB_GETROOMARTIST_RESP, enmSessionType_GetRoomArtist, this, enmGetRoomInfoTimeoutPeriod, pSession);
		if(ret < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("login request:create get room info session is failed!{RoomID=%d}\n", pMsgHead->nRoomID);


			stArtistListResp.nCount = 0;

			SendMessageResponse(MSGID_CLRM_GETARTISTOFFLINE_RESP, pMsgHead, &stArtistListResp,
					enmTransType_P2P, nOptionLen, (char *)pOptionData);

			return ret;
		}

		CGetArtistInfoSessionData * pArtistData = new (pSession->GetSessionData())CGetArtistInfoSessionData();
		memcpy(&pArtistData->m_sMsgHead, pMsgHead, sizeof(pArtistData->m_sMsgHead));
		if(nOptionLen > 0)
		{
			pArtistData->m_nOptionLen = nOptionLen;
			memcpy(pArtistData->m_arrOptionData, pOptionData, nOptionLen);
		}
		memcpy(&pArtistData->m_pMsgBody, pGetArtistReq, sizeof(pArtistData->m_pMsgBody));
		//发送请求
		CGetRoomArtistReq stGetRoomArtistReq;
		stGetRoomArtistReq.nRoomID = pMsgHead->nRoomID;

		SendMessageRequest(MSGID_RSDB_GETROOMARTIST_REQ , &stGetRoomArtistReq, pMsgHead->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
				pSession->GetSessionIndex(), 0, NULL, "send GetArtistInfo request");
	}
	else
	{
		stArtistListResp.nCount =  pRoom->m_RoomArtistList.GetOfflineArtistList(stArtistListResp.stRoomUserInfo);
		SendMessageResponse(MSGID_CLRM_GETARTISTOFFLINE_RESP, pMsgHead, &stArtistListResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
	}


	return S_OK;
}


FRAME_ROOMSERVER_NAMESPACE_END
