/*
 *  bll_event_redispatch_media.cpp
 *
 *  To do：
 *  Created on: 	2012-5-21
 *  Author: 		luocj
 */

#include "bll_event_redispatch_media.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "dal/to_client_message.h"
#include "dal/from_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CReDispatchMediaEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetMediaInfo:
		GetMediaInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		default:
		break;
	}
	return S_OK;
}
int32_t CReDispatchMediaEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_ReGetMediaDaly:
		TimerStartGetMediaTimeout(pSession);
		break;
		case enmSessionType_GetMediaInfo:
		{
			CGetMediaData * pData = (CGetMediaData *)(pSession->GetSessionData());
			if(NULL == pData)
			{
				WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
				return E_NULLPOINTER;
			}
			StartTimer(pData->m_nRoomID);
		}
		break;
		default:
		break;
	}
	return S_OK;
}
int32_t CReDispatchMediaEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CSetMediaInfoNoti* pSetMediaInfoNoti = dynamic_cast<CSetMediaInfoNoti *>(pMsgBody);
	WRITE_NOTICE_LOG("recv rebulid mediaserver media  noti{nRoomID=%d}",pSetMediaInfoNoti->nRoomID);

	ret = ReBulidMediaServer(pSetMediaInfoNoti);
	if(ret<0)
	{
		WRITE_ERROR_LOG("rebulid mediaserver of room is filed{nRoomID=%d}",pSetMediaInfoNoti->nRoomID);
		return ret;
	}
	if(pSetMediaInfoNoti->nTelServerID == enmInvalidServerID || pSetMediaInfoNoti->nCncServerID == enmInvalidServerID)
	{
		ret=StartTimer(pSetMediaInfoNoti->nRoomID);
		return ret;
	}
	WRITE_NOTICE_LOG("rebulid mediaserver of room is ok{nRoomID=%d}",pSetMediaInfoNoti->nRoomID);
	return S_OK;
}

int32_t CReDispatchMediaEvent::StartTimer(RoomID nRoomID)
{
	CFrameSession *pSession = NULL;
	int32_t ret = g_Frame.CreateSession(0, enmSessionType_ReGetMediaDaly, this, enmReGetMediaDelayPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("rebulid mediaserver:start timer failed!");
		return ret;
	}

	CTimerStartGetMediaData * pTimerMediaData = new (pSession->GetSessionData())CTimerStartGetMediaData();
	pTimerMediaData->m_nRoomID = nRoomID;

	WRITE_NOTICE_LOG("rebulid mediaserver:need reGet media info{nRoomID=%d}",nRoomID);
	return S_OK;
}

int32_t CReDispatchMediaEvent::TimerStartGetMediaTimeout(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}
	CTimerStartGetMediaData * pData = (CTimerStartGetMediaData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	RoomID nRoomID = pData->m_nRoomID;
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("Get Room Filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}

	uint32_t arrMediaAddress[enmNetType_Count];
	uint16_t arrMediaPort[enmNetType_Count];
	if(!pRoom->GetMediaAddressPort(arrMediaAddress, arrMediaPort))
	{
		CFrameSession *pSession = NULL;
		ret = g_Frame.CreateSession(MSGID_HSRS_GETMEIDAINFO_RESP, enmSessionType_GetMediaInfo, this, enmGetMediaInfoTimeoutPeriod, pSession);
		if(ret < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("rebulid mediaserver : create get meida info session is failed!{nRoomID=%d}\n", nRoomID);
			return ret;
		}

		CGetMediaData * pMediaData = new (pSession->GetSessionData())CGetMediaData();
		pMediaData->m_nRoomID = nRoomID;
		//发送请求
		CGetMediaInfoReq stGetMediaInfoReq;

		SendMessageRequest(MSGID_RSHS_GETMEDIAINFO_REQ, &stGetMediaInfoReq, nRoomID,
				enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_RoomDispatcher, enmInvalidRoleID,
				pSession->GetSessionIndex(), 0, NULL, "send GetMediaInfo request");

		return S_OK;
	}
	return S_OK;
}

int32_t CReDispatchMediaEvent::GetMediaInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CGetMediaData * pData =(CGetMediaData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rebulid mediaserver:recv get media info resp{nRoomID=%d}",pData->m_nRoomID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("rebulid mediaserver:get room filed!!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	uint32_t arrMediaAddress[enmNetType_Count];
	uint16_t arrMediaPort[enmNetType_Count];
	if(pRoom->GetMediaAddressPort(arrMediaAddress, arrMediaPort))
	{
		WRITE_NOTICE_LOG("rebulid mediaserver:room is Get at other{nRoomID=%d}",pData->m_nRoomID);
		return S_OK;
	}
	CGetMediaInfoResp *pGetMediaInfoResp = dynamic_cast<CGetMediaInfoResp *>(pMsgBody);
	if(NULL == pGetMediaInfoResp)
	{
		WRITE_ERROR_LOG("rebulid mediaserver:pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}

	if(pGetMediaInfoResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("login request:get media info failed!{nRoomID=%d}\n",pData->m_nRoomID);
		ret=StartTimer(pData->m_nRoomID);
		return pGetMediaInfoResp->nResult;
	}
	if(pGetMediaInfoResp->nTelServerID == enmInvalidServerID || pGetMediaInfoResp->nCncServerID == enmInvalidServerID)
	{
		ret=StartTimer(pData->m_nRoomID);
		return ret;
	}
	//如果房间的人数不等于0，做media的数据迁移
	if(pRoom->GetCurUserCount() > 0)
	{
		CSetMediaInfoNoti stSetMediaInfoNoti;
		stSetMediaInfoNoti.nRoomID = pRoom->GetRoomID();
		stSetMediaInfoNoti.nTelMediaIP = pGetMediaInfoResp->nTelMediaIP;
		stSetMediaInfoNoti.nTelMediaPort = pGetMediaInfoResp->nTelMediaPort;
		stSetMediaInfoNoti.nCncMediaIP = pGetMediaInfoResp->nCncMediaIP;
		stSetMediaInfoNoti.nCncMediaPort = pGetMediaInfoResp->nCncMediaPort;
		stSetMediaInfoNoti.nTelServerID = pGetMediaInfoResp->nTelServerID;
		stSetMediaInfoNoti.nCncServerID = pGetMediaInfoResp->nCncServerID;
		ReBulidMediaServer(&stSetMediaInfoNoti);
	}
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
