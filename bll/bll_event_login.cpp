/*
 * bll_event_login.cpp
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#include "bll_event_login.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "common/common_api.h"
#include "roomserver_message_define.h"
#include "dal/to_client_message.h"
#include "dal/to_server_message.h"
#include "public_typedef.h"
#include "config/someconfig.h"
#include "ctl/item_rebulid_ctl.h"

#include <arpa/inet.h>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CLoginMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetRoleInfo:
		GetRoleInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		case enmSessionType_GetRoomInfo:
		GetRoomInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		case enmSessionType_GetMediaInfo:
		GetMediaInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		case enmSessionType_GetEnterCount:
		GetEnterRoomCountResp(pMsgHead, pMsgBody, pSession);
		break;
		case enmSessionType_GetItemInfo:
		GetItemInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		default:
		break;
	}

	return S_OK;
}
int32_t CLoginMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetRoleInfo:
		GetRoleInfoTimeout(pSession);
		break;
		case enmSessionType_GetRoomInfo:
		GetRoomInfoTimeout(pSession);
		break;
		case enmSessionType_GetMediaInfo:
		GetMediaInfoTimeout(pSession);
		break;
		case enmSessionType_GetEnterCount:
		GetEnterRoomCountTimeout(pSession);
		break;
		case enmSessionType_GetItemInfo:
		GetItemInfoTimeout(pSession);
		break;
		default:
		break;
	}

	return S_OK;
}
int32_t CLoginMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CLoginResp stLoginResp;

	uint32_t offset = 0;
	ConnInfo stConnInfo;
	int32_t ret = stConnInfo.MessageDecode((uint8_t *)pOptionData, (uint32_t)nOptionLen, offset);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("login request :decode connuin failed{nRoleID=%d, ret=0x%08x}\n", pMsgHead->nRoleID, ret);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return ret;
	}

	ConnInfo *pConnInfo = g_DataCenter.GetConnInfo(pMsgHead->nRoleID);
	if(!stConnInfo.Compare(pConnInfo))
	{
		WRITE_ERROR_LOG("login request :conn info is not equal!{nRoleID=%d, ret=0x%08x}\n", pMsgHead->nRoleID, ret);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return ret;
	}

	CLoginReq *pLoginMsg = dynamic_cast<CLoginReq *>(pMsgBody);
	if(NULL == pLoginMsg)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("login request:recv player enter room request{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);

	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		//todo : write something

		//房间不存在，创建房间
		ret = g_RoomMgt.CreateRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
		if(ret < 0 || pRoom == NULL)
		{
			WRITE_ERROR_LOG("login request:creat room filed{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID, ret);
			stLoginResp.nResult = enmLoginResult_CreateRoomFailed;
			stLoginResp.strFailReason = "unknown error";

			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
					enmTransType_P2P, nOptionLen, (char *)pOptionData);

			return E_NULLPOINTER;

		}
		//到数据库获取房间信息
		CFrameSession *pSession = NULL;
		ret = g_Frame.CreateSession(MSGID_DBRS_GETROOMINFO_RESP, enmSessionType_GetRoomInfo, this, enmGetRoomInfoTimeoutPeriod, pSession);
		if(ret < 0 || pSession == NULL)
		{
			WRITE_ERROR_LOG("login request:create get room info session is failed!{RoomID=%d}\n", pMsgHead->nRoomID);

			g_RoomMgt.DestroyRoom(pMsgHead->nRoomID);

			stLoginResp.nResult = enmLoginResult_CreateSessionFailed;
			stLoginResp.strFailReason = "unknown error";

			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
					enmTransType_P2P, nOptionLen, (char *)pOptionData);

			return ret;
		}

		CGetRoomInfoSessionData * pRoomData = new (pSession->GetSessionData())CGetRoomInfoSessionData();
		memcpy(&pRoomData->m_sMsgHead, pMsgHead, sizeof(pRoomData->m_sMsgHead));
		if(nOptionLen > 0)
		{
			pRoomData->m_nOptionLen = nOptionLen;
			memcpy(pRoomData->m_arrOptionData, pOptionData, nOptionLen);
		}
		memcpy(&pRoomData->m_pMsgBody, pLoginMsg, sizeof(pRoomData->m_pMsgBody));
		//发送请求
		CGetRoomInfoReq stGetRoomInfoReq;
		stGetRoomInfoReq.nRoomID = pMsgHead->nRoomID;

		SendMessageRequest(MSGID_RSDB_GETROOMINFO_REQ , &stGetRoomInfoReq, pMsgHead->nRoomID,
				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
				pSession->GetSessionIndex(), 0, NULL, "send GetRoomInfo request");
	}
	//如果有房间了
	else
	{
		return EnterRoom(pMsgHead, pMsgBody, pRoom, nOptionLen, pOptionData,true);
	}

	return S_OK;
}

int32_t CLoginMessageEvent::GetRoleInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CGetRoleInfoSessionData * pData =(CGetRoleInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("login request:recv get player info resp{nRoleID=%d}",pData->m_sMsgHead.nRoleID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("Get Room Filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("login request:Get Player Filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CLoginResp stLoginResp;
	CLoginNoti stLoginNoti;
	CGetRoleInfoResp *pGetRoleInfoResp = dynamic_cast<CGetRoleInfoResp *>(pMsgBody);
	if(NULL == pGetRoleInfoResp)
	{
		WRITE_ERROR_LOG("login request:pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return E_NULLPOINTER;
	}

	if(pGetRoleInfoResp->nResult < 0)
	{
		WRITE_ERROR_LOG("login request:get role info failed!{nRoleID=%d, ret=0x%08x}",pData->m_sMsgHead.nRoleID,pGetRoleInfoResp->nResult);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return pGetRoleInfoResp->nResult;
	}
	pPlayer->SetPlayerGender(pGetRoleInfoResp->nPlayerGender);
	pPlayer->SetRoleName(pGetRoleInfoResp->strRoleName.GetString());
	pPlayer->SetAdminRoomID(pGetRoleInfoResp->arrRoomID,pGetRoleInfoResp->arrRoleRank, pGetRoleInfoResp->nRoomCount);
	pPlayer->SetVipLevel(pGetRoleInfoResp->nVipLevel);
	pPlayer->SetOnlineTime(pGetRoleInfoResp->nOnlineTime);
	pPlayer->SetIdentityType(pGetRoleInfoResp->nIdentityType);
	pPlayer->SetOnMicTime(pGetRoleInfoResp->nOnMicTime);
	pPlayer->SetAccountID(pGetRoleInfoResp->nAccountID);
	pPlayer->SetPlayerSelfSetting(pGetRoleInfoResp->nSelfSetting);
	pPlayer->SetLoginTimes(pGetRoleInfoResp->nLoginTimes);
	pPlayer->SetCollectRoomList(pGetRoleInfoResp->nCollectCount, pGetRoleInfoResp->arrCollectRoomList);

	//更新房间的管理员列表
	for(int32_t i = 0;i<pGetRoleInfoResp->nRoomCount;i++)
	{
		if(pGetRoleInfoResp->arrRoomID[i] == pRoom->GetRoomID())
		{
			ret = pRoom->AddMemberList(pPlayer->GetRoleID(),pGetRoleInfoResp->arrRoleRank[i]);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("set rank in room filed{ret=0x%08x, RoleID=%d, RoomID=%d, RoleRank=%d}\n",ret,pPlayer->GetRoleID()
				                 , pGetRoleInfoResp->arrRoomID[i],pGetRoleInfoResp->arrRoleRank[i]);
				stLoginResp.nResult = enmLoginResult_Unknown;
				stLoginResp.strFailReason = "unknown error";
				SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
						enmTransType_P2P);
				DestroyPlayerAndRoom(pRoom,pPlayer);
				return ret;
			}
		}
	}

	//更新服务器内容
	return GetEnterRoomCountFromHall(&pData->m_sMsgHead,&pData->m_pMsgBody,pPlayer,pRoom,nPlayerIndex);

	return S_OK;
}

int32_t CLoginMessageEvent::GetRoleInfoTimeout(CFrameSession* pSession)
{
	CGetRoleInfoSessionData * pData = (CGetRoleInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_WARNING_LOG("login request:recv get player info time out{nRoleID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoleID,pSession->GetSessionIndex());
	//销毁玩家对象
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("Get Room Filed!{nRoomID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("Get Player Filed!{nRoleID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoleID,E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CLoginResp stLoginResp;
	stLoginResp.nResult = enmLoginResult_Timeout;
	stLoginResp.strFailReason = "get role info timeout";

	SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
			enmTransType_P2P);
	DestroyPlayerAndRoom(pRoom,pPlayer);
	return S_OK;
}

int32_t CLoginMessageEvent::GetRoomInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CGetRoomInfoSessionData * pData = (CGetRoomInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("login request:recv get room info resp{nRoomID=%d}",pData->m_sMsgHead.nRoomID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("Get Room Filed!!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CLoginResp stLoginResp;
	CGetRoomInfoResp *pGetRoomInfoResp = dynamic_cast<CGetRoomInfoResp *>(pMsgBody);
	if(NULL == pGetRoomInfoResp)
	{
		WRITE_ERROR_LOG("login request:pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.RealDestroyRoom(pData->m_sMsgHead.nRoomID);
		return E_NULLPOINTER;
	}
	if(pGetRoomInfoResp->nResult < 0)
	{
		WRITE_ERROR_LOG("login request:get Room info from DB failed!{nRoomID=%d,ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,pGetRoomInfoResp->nResult);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.RealDestroyRoom(pData->m_sMsgHead.nRoomID);
		return pGetRoomInfoResp->nResult;
	}
	//房间被封
	if(pGetRoomInfoResp->nTimeUnLock > (uint32_t)CTimeValue::CurrentTime().Seconds())
	{
		WRITE_ERROR_LOG("login request:room is be locked!{nRoomID=%d}\n",pData->m_sMsgHead.nRoomID);
		stLoginResp.nResult = enmLoginResult_RoomLocked;
		stLoginResp.strFailReason = pGetRoomInfoResp->strLockReason.GetString();
		stLoginResp.nKickedLeftTime = (pGetRoomInfoResp->nTimeUnLock-pGetRoomInfoResp->nTimeLock)/SECOND_PER_DAY;

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.RealDestroyRoom(pData->m_sMsgHead.nRoomID);
		return S_OK;
	}
	//后台设置房间容量的时候的一个保护
	if(pGetRoomInfoResp->nMaxShowUserLimit > pGetRoomInfoResp->nMaxRealUserLimit
			|| pGetRoomInfoResp->nMaxRealUserLimit > MaxUserCountPerRoom)
	{
		WRITE_WARNING_LOG("modify room capacity: value is error {nRoomRealCapacity=%d,nRoomShowCapacity=%d,MaxUserCountPerRoom=%d}",pGetRoomInfoResp->nMaxRealUserLimit,pGetRoomInfoResp->nMaxShowUserLimit,MaxUserCountPerRoom);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.DestroyRoom(pData->m_sMsgHead.nRoomID);
	}

	pRoom->SetChannelType(pGetRoomInfoResp->nChannelType);
	pRoom->SetChannelID(pGetRoomInfoResp->nChannelID);
	pRoom->SetChannelName(pGetRoomInfoResp->strChannelName.GetString());
	pRoom->SetRegionType(pGetRoomInfoResp->nRegionType);
	pRoom->SetRegionID(pGetRoomInfoResp->nRegionID);
	pRoom->SetRegionName(pGetRoomInfoResp->strRegionName.GetString());
	pRoom->SetRoomType(pGetRoomInfoResp->nRoomType);
	pRoom->SetRoomID(pGetRoomInfoResp->nRoomID);
	pRoom->SetRoomName(pGetRoomInfoResp->strRoomName.GetString());
	pRoom->SetMicroTime(pGetRoomInfoResp->nMicroTime);
	pRoom->SetMaxRealUserLimit(pGetRoomInfoResp->nMaxRealUserLimit);
	pRoom->SetMaxShowUserLimit(pGetRoomInfoResp->nMaxShowUserLimit);
	pRoom->SetHostID(pGetRoomInfoResp->nHostID);
	pRoom->SetHostName(pGetRoomInfoResp->strHostName.GetString());
	pRoom->SetHostGender(pGetRoomInfoResp->nHostGender);
	pRoom->SetHost179ID(pGetRoomInfoResp->nHost179ID);
	pRoom->SetRoomFund(pGetRoomInfoResp->nRoomFund);
	pRoom->SetRoomOption(pGetRoomInfoResp->nRoomOption);
	pRoom->SetRoomPassWord(pGetRoomInfoResp->strPassword.GetString());
	pRoom->SetMemberList(pGetRoomInfoResp->arrRoleID, pGetRoomInfoResp->arrRoleRank,
			pGetRoomInfoResp->arrTotalTime, pGetRoomInfoResp->arrMicTime,pGetRoomInfoResp->nCurMemberCount);
	pRoom->SetRoomNotice(pGetRoomInfoResp->strRoomNotice.GetString());
	pRoom->SetRoomSignature(pGetRoomInfoResp->strRoomSignature.GetString());
	pRoom->SetRoomWelcome(pGetRoomInfoResp->strRoomWelcome.GetString());
	pRoom->SetBlackList(pGetRoomInfoResp->arrBlackRoleID,pGetRoomInfoResp->nInBlackPlayerCount);
	pRoom->SetRobotPercent(pGetRoomInfoResp->nRobotPercent);
	pRoom->SetRobotSendPercent(pGetRoomInfoResp->nSendPercent);

	//为了roomdispatcher更新房间mic的状态（涉及到初值）
	pRoom->ReSetAllMic();

	//发送创建房间通知
	CCreateRoomNotice stCreateRoomNotice;
	stCreateRoomNotice.nRegionType = pGetRoomInfoResp->nRegionType;
	stCreateRoomNotice.nRegionID = pGetRoomInfoResp->nRegionID;
	stCreateRoomNotice.strRegionName = pGetRoomInfoResp->strRegionName.GetString();
	stCreateRoomNotice.nChannelType = pGetRoomInfoResp->nChannelType;
	stCreateRoomNotice.nChannelID = pGetRoomInfoResp->nChannelID;
	stCreateRoomNotice.strChannelName = pGetRoomInfoResp->strChannelName.GetString();
	stCreateRoomNotice.nRoomType = pGetRoomInfoResp->nRoomType;
	stCreateRoomNotice.nRoomID = pMsgHead->nRoomID;
	stCreateRoomNotice.strRoomName = pGetRoomInfoResp->strRoomName.GetString();
	stCreateRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stCreateRoomNotice.nMemberCount = pGetRoomInfoResp->nCurMemberCount;
	for(uint16_t i = 0;i < pGetRoomInfoResp->nCurMemberCount;i++)
	{
		stCreateRoomNotice.arrRoleID[i] = pGetRoomInfoResp->arrRoleID[i];
		stCreateRoomNotice.arrRoleRank[i] = pGetRoomInfoResp->arrRoleRank[i];
	}
	stCreateRoomNotice.nHostID = pGetRoomInfoResp->nHostID;
	stCreateRoomNotice.strHostName = pGetRoomInfoResp->strHostName.GetString();

	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_Item);
	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_Hall);
	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_Media);
	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_Tunnel);
	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_RoomDispatcher);
	SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_HallDataCenter);

	return EnterRoom(&pData->m_sMsgHead, &pData->m_pMsgBody, pRoom, pData->m_nOptionLen, pData->m_arrOptionData,true);
}

int32_t CLoginMessageEvent::GetRoomInfoTimeout(CFrameSession* pSession)
{
	CGetRoomInfoSessionData * pData = (CGetRoomInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_WARNING_LOG("login request:recv get room info resp time out{nRoomID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoomID,pSession->GetSessionIndex());
	CLoginResp stLoginResp;

	stLoginResp.nResult = enmLoginResult_Timeout;
	stLoginResp.strFailReason = "get room info timeout";

	SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
			enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
	g_RoomMgt.RealDestroyRoom(pData->m_sMsgHead.nRoomID);

	return S_OK;
}

int32_t CLoginMessageEvent::GetMediaInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CGetMediaInfoSessionData * pData = (CGetMediaInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("login request:recv get media info resp{nRoomID=%d}",pData->m_sMsgHead.nRoomID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("login request:get room filed!!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}

	CLoginResp stLoginResp;
	CGetMediaInfoResp *pGetMediaInfoResp = dynamic_cast<CGetMediaInfoResp *>(pMsgBody);
	if(NULL == pGetMediaInfoResp)
	{
		WRITE_ERROR_LOG("login request:pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.DestroyRoom(pData->m_sMsgHead.nRoomID);
		return E_NULLPOINTER;
	}

	if(pGetMediaInfoResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("login request:get media info failed!{nRoomID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,pGetMediaInfoResp->nResult);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
		g_RoomMgt.DestroyRoom(pData->m_sMsgHead.nRoomID);
		return pGetMediaInfoResp->nResult;
	}
	uint32_t arrMediaAddress[enmNetType_Count] ;
	uint16_t arrMediaPort[enmNetType_Count] ;
	ServerID arrMediaServerID[enmNetType_Count] ;

	if(pRoom->GetMediaAddressPort(arrMediaAddress, arrMediaPort))
	{
		WRITE_NOTICE_LOG("room is Get at other{nRoomID=%d}",pRoom->GetRoomID());
		return EnterRoom(&pData->m_sMsgHead, &pData->m_pMsgBody, pRoom, pData->m_nOptionLen, pData->m_arrOptionData);
	}

	arrMediaAddress[enmNetType_Tel] = pGetMediaInfoResp->nTelMediaIP;
	arrMediaPort[enmNetType_Tel] = pGetMediaInfoResp->nTelMediaPort;
	arrMediaAddress[enmNetType_Cnc] = pGetMediaInfoResp->nCncMediaIP;
	arrMediaPort[enmNetType_Cnc] = pGetMediaInfoResp->nCncMediaPort;
	arrMediaServerID[enmNetType_Tel] = pGetMediaInfoResp->nTelServerID;
	arrMediaServerID[enmNetType_Cnc] = pGetMediaInfoResp->nCncServerID;

	pRoom->SetMediaAddressPort(arrMediaAddress, arrMediaPort);
	pRoom->SetMediaServerID(arrMediaServerID);

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
	return EnterRoom(&pData->m_sMsgHead, &pData->m_pMsgBody, pRoom, pData->m_nOptionLen, pData->m_arrOptionData);
}

int32_t CLoginMessageEvent::GetMediaInfoTimeout(CFrameSession* pSession)
{
	CGetMediaInfoSessionData * pData = (CGetMediaInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_WARNING_LOG("login request:get media time out {nRoomID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoomID,pSession->GetSessionIndex());
	CLoginResp stLoginResp;

	stLoginResp.nResult = enmLoginResult_Timeout;
	stLoginResp.strFailReason = "get media info timeout";

	SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
			enmTransType_P2P, pData->m_nOptionLen, pData->m_arrOptionData);
	g_RoomMgt.DestroyRoom(pData->m_sMsgHead.nRoomID);
	return S_OK;
}

int32_t CLoginMessageEvent::GetEnterRoomCountResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	int32_t ret = S_OK;
	CGetEnterRoomSessionData * pData = (CGetEnterRoomSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("login request:recv get global info resp{nRoleID=%d}",pData->m_sMsgHead.nRoleID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("login request:get room filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("login request:get player filed!!{nRoleID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoleID,E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CLoginNoti stLoginNoti;
	CLoginResp stLoginResp;
	CGetRoleGlobalInfoResp *pGetRoleGlobalInfoResp = dynamic_cast<CGetRoleGlobalInfoResp *>(pMsgBody);
	if(NULL == pGetRoleGlobalInfoResp)
	{
		WRITE_ERROR_LOG("login request:pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return E_NULLPOINTER;
	}

	if(pGetRoleGlobalInfoResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("login request:get player global info failed {nRoleID=%d, ret=0x%8x}\n",pData->m_sMsgHead.nRoleID,pGetRoleGlobalInfoResp->nResult);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
				enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return pGetRoleGlobalInfoResp->nResult;
	}
	pPlayer->SetTotleEnterRoomCount(pGetRoleGlobalInfoResp->nEnterCount);
	//如果获取到的全局状态为不可用的状态，则表示不在麦上，设置为在房间
	if(pGetRoleGlobalInfoResp->nPlayerState == enmInvalidPlayerState)
	{
		pPlayer->SetPlayerGlobalState(enmPlayerState_InRoom);
	}
	else
	{
		pPlayer->SetPlayerGlobalState(pGetRoleGlobalInfoResp->nPlayerState);
	}
	pPlayer->SetNewPlayerRoomCount(pGetRoleGlobalInfoResp->nEnterNewPlayerRoomCount);
	//判断是否为游客，是游客不拉去item信息
	if(pPlayer->GetVipLevel() == enmVipLevel_NONE)
	{
		CLoginNoti stLoginNoti;
		//验证玩家
		VerifyPlayer(pRoom, pPlayer, nPlayerIndex,pPlayer->GetConnInfo().nAddress,&pData->m_pMsgBody, stLoginResp, stLoginNoti);

		if(stLoginResp.nResult == enmLoginResult_OK)
		{
			//进入房间通知
			CEnterRoomNotice stEnterRoomNotice;
			stEnterRoomNotice.nRoleID = pData->m_sMsgHead.nRoleID;
			stEnterRoomNotice.nRoomID = pData->m_sMsgHead.nRoomID;
			stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
			stEnterRoomNotice.nPlayerType = enmPlayerType_Normal;
			stEnterRoomNotice.nRoleRank = pRoom->GetRoleRank(pData->m_sMsgHead.nRoleID);
			stEnterRoomNotice.strRoleName = pPlayer->GetRoleName();
			stEnterRoomNotice.nAccountID = pPlayer->GetAccountID();
			stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
			stEnterRoomNotice.nIdentityType = pPlayer->GetIdentityType();
			stEnterRoomNotice.nKey = stLoginResp.nKey;
			if(pPlayer->IsHideEnter())
			{
				stEnterRoomNotice.nEnterRoomType = enmEnterRoomType_Hid;
			}

			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Item);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Hall);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_RoomDispatcher);

			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_P2P, pData->m_sMsgHead.nRoleID,
				enmEntityType_Media, pPlayer->GetConnInfo().nServerID);

			//如果没有给进房者直连的Media发送过上麦通知,则发一次
			pRoom->SendAddTransmitUserNotifyWhenPlayerEnterRoom(pPlayer);

			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Task);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_P2P, pData->m_sMsgHead.nRoleID, enmEntityType_Tunnel,pPlayer->GetConnInfo().nServerID);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_HallDataCenter);
			//游客没有Item
			stLoginNoti.stRoomUserInfo.nItemCount = 0;
			stLoginNoti.stRoomUserInfo.nMagnateLevel = enmMagnateLevel_0;
			SendMessageNotifyToClient(MSGID_RSCL_LOGIN_NOTI, &stLoginNoti, pData->m_sMsgHead.nRoomID, enmBroadcastType_ExceptPlayr,
					pData->m_sMsgHead.nRoleID, 0, NULL, "send login room notify message");
			//进机器人
			WatchShow(pRoom,pPlayer,stLoginResp);
			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
												enmTransType_P2P);

			//pRoom->UpdateUserCountOnMedia(pPlayer->GetConnInfo().nServerID, 1);
		}
		else
		{
			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,
									enmTransType_P2P);
			DestroyPlayerAndRoom(pRoom,pPlayer);
		}

		if(stLoginResp.nResult == enmLoginResult_OK)
		{
			DistributionRobot(pRoom);
		}

		return S_OK;
	}
	//获取玩家的item信息
	ret = GetItemInfoFromItem(&pData->m_sMsgHead,&pData->m_pMsgBody,pPlayer,pRoom,nPlayerIndex);
	return ret;
}

int32_t CLoginMessageEvent::GetEnterRoomCountTimeout(CFrameSession* pSession)
{
	CGetEnterRoomSessionData * pData = (CGetEnterRoomSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_WARNING_LOG("login request:get global info time out{nRoleID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoleID,pSession->GetSessionIndex());
	CLoginResp stLoginResp;
	stLoginResp.nResult = enmLoginResult_Timeout;
	stLoginResp.strFailReason = "get global info timeout";
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("login request:get room filed!{nRoomID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,ret);
		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("login request:get player filed!{nRoleID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoleID,ret);
		return E_NULLPOINTER;
	}
	SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);
	DestroyPlayerAndRoom(pRoom,pPlayer);

	return S_OK;
}

int32_t CLoginMessageEvent::GetItemInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	CLoginResp stLoginResp;

	CGetItemInfoSessionData * pData = (CGetItemInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("login request:recv get item info resp{nRoleID=%d}",pData->m_sMsgHead.nRoleID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("login request:get room filed!{nRoomID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_GetItemError;
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);

		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("login request:get player filed!{nRoleID=%d, ret=0x%08x}\n", pData->m_sMsgHead.nRoleID, E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_GetItemError;
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);

		return E_NULLPOINTER;
	}
	CLoginNoti stLoginNoti;
	CGetItemInfoResp *pGetItemInfoResp = dynamic_cast<CGetItemInfoResp *>(pMsgBody);
	if(NULL == pGetItemInfoResp)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return E_NULLPOINTER;
	}

	if(pGetItemInfoResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("get item info failed {nRoleID=%d, ret=0x%8x}\n",pData->m_sMsgHead.nRoleID,pGetItemInfoResp->nResult);
		stLoginResp.nResult = enmLoginResult_GetItemError;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
		return pGetItemInfoResp->nResult;
	}

	//验证玩家
	VerifyPlayer(pRoom, pPlayer, nPlayerIndex,pPlayer->GetConnInfo().nAddress,&pData->m_pMsgBody, stLoginResp, stLoginNoti);

	if(stLoginResp.nResult == enmLoginResult_OK)
	{
		//更新主播列表主播状态
		pRoom->m_RoomArtistList.SetOnlineStat(pData->m_sMsgHead.nRoleID,enmOnline_stat);
		//进入房间通知
		CEnterRoomNotice stEnterRoomNotice;
		stEnterRoomNotice.nRoleID = pData->m_sMsgHead.nRoleID;
		stEnterRoomNotice.nRoomID = pData->m_sMsgHead.nRoomID;
		stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
		stEnterRoomNotice.nPlayerType = enmPlayerType_Normal;
		stEnterRoomNotice.nRoleRank = pRoom->GetRoleRank(pData->m_sMsgHead.nRoleID);
		stEnterRoomNotice.strRoleName = pPlayer->GetRoleName();
		stEnterRoomNotice.nAccountID = pPlayer->GetAccountID();
		stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
		stEnterRoomNotice.nIdentityType = pPlayer->GetIdentityType();
		stEnterRoomNotice.nKey = stLoginResp.nKey;
		if(pPlayer->IsHideEnter())
		{
			stEnterRoomNotice.nEnterRoomType = enmEnterRoomType_Hid;
		}

		//在房间是管理员的时候通知DB进房，方便统计最后进房时间
		if(stEnterRoomNotice.nRoleRank > enmRoleRank_TempAdmin&&stEnterRoomNotice.nRoleRank != enmRoleRank_Super)
		{
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_ByKey, pData->m_sMsgHead.nRoleID, enmEntityType_DBProxy);
		}
		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Item);
		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Hall);
		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_RoomDispatcher);

		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_P2P, pData->m_sMsgHead.nRoleID,
				enmEntityType_Media, pPlayer->GetConnInfo().nServerID);

		//如果没有给进房者直连的Media发送过上麦通知,则发一次
		pRoom->SendAddTransmitUserNotifyWhenPlayerEnterRoom(pPlayer);

		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_Task);
		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_P2P, pData->m_sMsgHead.nRoleID, enmEntityType_Tunnel,pPlayer->GetConnInfo().nServerID);
		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pData->m_sMsgHead.nRoomID, enmTransType_Broadcast, pData->m_sMsgHead.nRoleID, enmEntityType_HallDataCenter);

		stLoginNoti.stRoomUserInfo.nItemCount = pGetItemInfoResp->arrPlayerItemInfo[0].nItemCount;
		for(uint32_t i = 0;i < stLoginNoti.stRoomUserInfo.nItemCount;i++)
		{
			stLoginNoti.stRoomUserInfo.arrItemID[i] = pGetItemInfoResp->arrPlayerItemInfo[0].arrItemID[i];
		}
		stLoginNoti.stRoomUserInfo.nMagnateLevel = pGetItemInfoResp->arrMagnateLevel[0];
		SendMessageNotifyToClient(MSGID_RSCL_LOGIN_NOTI, &stLoginNoti, pData->m_sMsgHead.nRoomID, enmBroadcastType_ExceptPlayr,
				pData->m_sMsgHead.nRoleID, 0, NULL, "send login room notify message");
		//进机器人
		WatchShow(pRoom,pPlayer,stLoginResp);
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);

		//增加mediaserver上的用户计数
		//pRoom->UpdateUserCountOnMedia(pPlayer->GetConnInfo().nServerID, 1);
	}
	else
	{
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);
		DestroyPlayerAndRoom(pRoom,pPlayer);
	}


	if(stLoginResp.nResult == enmLoginResult_OK)
	{
		DistributionRobot(pRoom);
	}

	return S_OK;
}

int32_t CLoginMessageEvent::GetItemInfoTimeout(CFrameSession* pSession)
{
	CGetItemInfoSessionData * pData =(CGetItemInfoSessionData *)(pSession->GetSessionData());
	if(NULL == pData)
	{
		WRITE_ERROR_LOG("pData==NULL{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CRoom *pRoom = NULL;
	CPlayer *pPlayer = NULL;
	do
	{
		WRITE_WARNING_LOG("login request:recv get item info time out{nRoleID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoleID,pSession->GetSessionIndex());
		//获取房间对象
		RoomIndex nRoomIndex = enmInvalidRoomIndex;
		int32_t ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
		if(ret < 0 || pRoom == NULL)
		{
			WRITE_ERROR_LOG("login request:get room filed!{nRoomID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoomID,E_NULLPOINTER);
			break;
		}
		//获取玩家对象
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(pData->m_sMsgHead.nRoleID, pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("login request:get player filed!{nRoleID=%d, ret=0x%08x}\n",pData->m_sMsgHead.nRoleID,E_NULLPOINTER);
			break;
		}
	}while(0);

	CLoginResp stLoginResp;
	stLoginResp.nResult = enmLoginResult_GetItemTimeout;
	stLoginResp.strFailReason = "get item info timeout";

	SendMessageResponse(MSGID_RSCL_LOGIN_RESP, &pData->m_sMsgHead, &stLoginResp,enmTransType_P2P);

	DestroyPlayerAndRoom(pRoom,pPlayer);

	return S_OK;
}

int32_t CLoginMessageEvent::EnterRoom(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, CRoom *pRoom,
		const uint16_t nOptionLen, const void *pOptionData,bool isNeedGetMediaInfo)
{
	CLoginResp stLoginResp;

	if(NULL == pMsgHead||NULL == pMsgBody||NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pRoom=0x%08x}\n",pMsgBody,pMsgHead,pRoom);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CLoginNoti stLoginNoti;
	CLoginReq *pLoginMsg = dynamic_cast<CLoginReq *>(pMsgBody);
	if(NULL == pLoginMsg)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}
//	uint32_t arrMediaAddress[enmNetType_Count];
//	uint16_t arrMediaPort[enmNetType_Count];
//	if(!pRoom->GetMediaAddressPort(arrMediaAddress, arrMediaPort) && isNeedGetMediaInfo)
//	{
//		CFrameSession *pSession = NULL;
//		ret = g_Frame.CreateSession(MSGID_HSRS_GETMEIDAINFO_RESP, enmSessionType_GetMediaInfo, this, enmGetMediaInfoTimeoutPeriod, pSession);
//		if(ret < 0 || pSession == NULL)
//		{
//			WRITE_ERROR_LOG("login request:create get meida info session is failed!{RoleID=%d, ret=0x%08x}\n",
//					pMsgHead->nRoleID, ret);
//
//			if(pRoom->GetCurUserCount()==0&&pRoom->GetRobotCount()==0)
//			{
//				g_RoomMgt.DestroyRoom(pRoom->GetRoomID());
//			}
//			stLoginResp.nResult = enmLoginResult_CreateSessionFailed;
//			stLoginResp.strFailReason = "unknown error";
//
//			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
//					enmTransType_P2P, nOptionLen, (char *)pOptionData);
//
//			return ret;
//		}
//
//		CGetMediaInfoSessionData * pMediaData = new (pSession->GetSessionData())CGetMediaInfoSessionData();
//		memcpy(&pMediaData->m_sMsgHead, pMsgHead, sizeof(pMediaData->m_sMsgHead));
//		//此时的可选字段里面包含着玩家的连接信息ConnUin
//		if(nOptionLen > 0)
//		{
//			pMediaData->m_nOptionLen = nOptionLen;
//			memcpy(pMediaData->m_arrOptionData, pOptionData, nOptionLen);
//		}
//		memcpy(&pMediaData->m_pMsgBody, pLoginMsg, sizeof(pMediaData->m_pMsgBody));
//		//发送请求
//		CGetMediaInfoReq stGetMediaInfoReq;
//
//		SendMessageRequest(MSGID_RSHS_GETMEDIAINFO_REQ, &stGetMediaInfoReq, pMsgHead->nRoomID,
//				enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_RoomDispatcher, pMsgHead->nRoleID,
//				pSession->GetSessionIndex(), 0, NULL, "send GetMediaInfo request");
//
//		return S_OK;
//	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		//玩家不存在，创建玩家
		ret = g_PlayerMgt.CreatePlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("create player error{nRoleID=%d, ret=%d}",pMsgHead->nRoleID,ret);
		}
		else
		{
			ConnUin stInfo;
			uint32_t offset = 0;
			ret = stInfo.MessageDecode((uint8_t *)pOptionData, (uint32_t)nOptionLen, offset);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("login request:decode connuin failed{nRoleID=%d, ret=0x%08x}\n", pMsgHead->nRoleID, ret);

				stLoginResp.nResult = enmLoginResult_DecodeConnUinFailed;
				stLoginResp.strFailReason = "unknown error";
				SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,enmTransType_P2P, nOptionLen, (char *)pOptionData);

				DestroyPlayerAndRoom(pRoom,pPlayer);
				return ret;
			}
			pPlayer->SetConnInfo(stInfo);

			//判断是否是游客，是游客，着直接设置玩家信息且不到数据库获取基本信息
			if(stInfo.nAccountType != enmAccountType_Visitor)
			{
				CFrameSession *pSession = NULL;
				ret = g_Frame.CreateSession(MSGID_DBRS_GETROLEINFO_RESP, enmSessionType_GetRoleInfo, this, enmGetRoleInfoTimeoutPeriod, pSession);
				if(ret < 0 || pSession == NULL)
				{
					WRITE_ERROR_LOG("login request:create get role info session is failed!{RoleID=%d, ret=%0x08x}\n", pMsgHead->nRoleID, ret);

					stLoginResp.nResult = enmLoginResult_CreateSessionFailed;
					stLoginResp.strFailReason = "unknown error";
					SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,enmTransType_P2P, nOptionLen, (char *)pOptionData);

					DestroyPlayerAndRoom(pRoom,pPlayer);
					return ret;
				}

				CGetRoleInfoSessionData * pRoleData = new (pSession->GetSessionData())CGetRoleInfoSessionData();
				memcpy(&pRoleData->m_sMsgHead, pMsgHead, sizeof(pRoleData->m_sMsgHead));
				memcpy(&pRoleData->m_pMsgBody, pLoginMsg, sizeof(pRoleData->m_pMsgBody));

				pPlayer->SetRoleID(pMsgHead->nRoleID);
				//发送请求
				CGetRoleInfoReq stGetRoleInfoReq;
				stGetRoleInfoReq.nRoleID = pMsgHead->nRoleID;
				SendMessageRequest(MSGID_RSDB_GETROLEINFO_REQ, &stGetRoleInfoReq, pMsgHead->nRoomID,
						enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
						pSession->GetSessionIndex(), 0, NULL, "send GetRoleInfo request");

				return ret;
			}
			else
			{
				pPlayer->SetPlayerGender(enmGender_Male);
				pPlayer->SetVipLevel(enmVipLevel_NONE);
				//当为游客的时候179ID等于RoleID
				pPlayer->SetAccountID(pPlayer->GetRoleID());
				pPlayer->SetOnlineTime(0);
				pPlayer->SetIdentityType(enmIdentityType_None);
				pPlayer->SetOnMicTime(0);
				return GetEnterRoomCountFromHall(pMsgHead,pLoginMsg,pPlayer,pRoom,nPlayerIndex,nOptionLen, pOptionData);
			}
		}
	}
	//玩家已经 在其他的房间了
	else
	{
		ConnInfo stConnInfo = pPlayer->GetConnInfo();
		ConnInfo *pConnInfo = g_DataCenter.GetConnInfo(pMsgHead->nRoleID);

		if(!stConnInfo.Compare(pConnInfo))
		{
			WRITE_ERROR_LOG("login request:conn info is not equal!{RoleID=%d}\n", pMsgHead->nRoleID);

			stLoginResp.nResult = enmLoginResult_Unknown;
			stLoginResp.strFailReason = "unknown error";
			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,enmTransType_P2P, nOptionLen, (char *)pOptionData);

			DestroyPlayerAndRoom(pRoom,pPlayer);
			return S_OK;
		}

		//表明是第一次进入房间，且在获取玩家信息完成之前又收到进房响应
		if(pPlayer->GetCurEnterRoomCount() == 0)
		{
			WRITE_ERROR_LOG("login request is too fast, get role info is not finish!{RoomID=%d, RoleID=%d}\n", pMsgHead->nRoomID, pMsgHead->nRoleID);

			stLoginResp.nResult = enmLoginResult_Unknown;
			stLoginResp.strFailReason = "unknown error";
			SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,enmTransType_P2P, nOptionLen, (char *)pOptionData);

			DestroyPlayerAndRoom(pRoom,pPlayer);
			return S_OK;
		}
		//获取现在进入的房间数
		return GetEnterRoomCountFromHall(pMsgHead,pLoginMsg,pPlayer,pRoom,nPlayerIndex,nOptionLen, pOptionData);

	}

	return ret;
}

int32_t CLoginMessageEvent::VerifyPlayer(CRoom *pRoom, CPlayer *pPlayer,
		PlayerIndex nPlayerIndex,uint32_t nAddress,CLoginReq *stLoginReq, CLoginResp &stLoginResp, CLoginNoti &stLoginNoti)
{
	int32_t ret = S_OK;
	if(NULL == pRoom || NULL == pPlayer || NULL == stLoginReq)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x, stLoginReq=0x%08x}\n",pRoom,pPlayer,stLoginReq);
		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "Unknown";
		return E_NULLPOINTER;
	}
	//玩家已经在这个房间里面
	if(pRoom->IsPlayerInRoom(nPlayerIndex))
	{
		WRITE_WARNING_LOG("login request:player alreadInRoom!{RoleID=%d}\n", pPlayer->GetRoleID());
		stLoginResp.nResult = enmLoginResult_AlreadInRoom;
		stLoginResp.strFailReason = "alread in room";
		return S_OK;
	}
	pPlayer->SetUserClientInfo(stLoginReq->nClientInfo);
	bool isOfficialPlayer = pPlayer->IsOfficialPlayer();
	if(!isOfficialPlayer && pPlayer->IsHideEnter())
	{
		WRITE_ERROR_LOG("login request:player can not hide enter:{nRoleID=%d, isOfficialPlayer=%d}\n",pPlayer->GetRoleID(),isOfficialPlayer);
		stLoginResp.nResult = enmLoginResult_Unknown;
		return E_NULLPOINTER;
	}

	RoomType nRoomType = pRoom->GetRoomType();
	bool isSpecial = (isOfficialPlayer || (pPlayer->GetIdentityType() & enmIdentityType_AGENT));
	//已达到同时可以进入的最大房间数（新手房除外）
	if(!pPlayer->CanEnterRoom(isSpecial) && nRoomType != enmRoomType_NewPlayer)
	{
		//回复错误码
		int32_t nMaxEnterRoom = pPlayer->GetMaxCanEnterRoomCount();
		if(pPlayer->GetTotleEnterRoomCount() >= MaxEnterRoomCount)
		{
			stLoginResp.nResult = enmLoginResult_OpenRoomToBig;
			nMaxEnterRoom = MaxEnterRoomCount;
		}
		else
		{
			stLoginResp.nResult = enmLoginResult_MaxRoomNum;
			stLoginResp.strFailReason = "MaxRoomNum";
			//给客户端此玩家可以进入的最大的房间数量
			stLoginResp.nKickedLeftTime = nMaxEnterRoom;
		}
		WRITE_WARNING_LOG("login request:player enter to MaxRoomNum!{RoleID=%d, nEnterRoomCount=%d}\n", pPlayer->GetRoleID(),nMaxEnterRoom);
		return S_OK;
	}

	//IP被封
	uint32_t nEndLockTime = 0;
	if(pRoom->IsInLockIPList(nAddress,nEndLockTime))
	{
		WRITE_WARNING_LOG("login request:player be locked IP!{RoleID=%d}\n", pPlayer->GetRoleID());
		stLoginResp.nResult = enmLoginResult_IPLocked;
		stLoginResp.strFailReason = "alread locked ip";
		stLoginResp.nKickedLeftTime = nEndLockTime-(uint32_t)CTimeValue::CurrentTime().Seconds();
		return S_OK;
	}
	//在被踢列表中
	uint32_t nEndKickTime = pRoom->GetEndKickTime(pPlayer->GetRoleID());
	if(nEndKickTime > 0 && nEndKickTime > (uint32_t)CTimeValue::CurrentTime().Seconds())
	{
		WRITE_WARNING_LOG("login request:player be Kicked!{RoleID=%d}\n", pPlayer->GetRoleID());
		stLoginResp.nResult = enmLoginResult_Kicked;
		stLoginResp.strFailReason = "alread kicked";
		stLoginResp.nKickedLeftTime = nEndKickTime-(uint32_t)CTimeValue::CurrentTime().Seconds();
		return S_OK;
	}
	//在黑名单中
	if(pRoom->IsInBlackList(pPlayer->GetRoleID()))
	{
		WRITE_WARNING_LOG("login request:player be BlackList!{RoleID=%d}\n", pPlayer->GetRoleID());
		stLoginResp.nResult = enmLoginResult_BlackList;
		stLoginResp.strFailReason = "in black list";
		return S_OK;
	}
	//是不是管理员及以上级别，或者官方人员
	bool isVip = pPlayer->GetVipLevel()>=enmVipLevel_Gold;
	bool isAdmin = pRoom->GetRoleRank(pPlayer->GetRoleID())>= enmRoleRank_Admin;
	//判断房间是否关闭
	if(pRoom->IsRoomCloseed() && (!isOfficialPlayer) && (pRoom->GetRoleRank(pPlayer->GetRoleID()) < enmRoleRank_Host))
	{
		WRITE_NOTICE_LOG("login request:room is closeed!{nRoomID=%d}\n", pRoom->GetRoomID());
		stLoginResp.nResult = enmLoginResult_RoomCloseed;
		stLoginResp.strFailReason = "RoomCloseed";
		return ret;
	}
	bool isNeedPassWord = false;
	if(isVip||isOfficialPlayer)
	{
		//判断房间还可以进去不
		if(!pRoom->PlayerCanEnter(true) && !pPlayer->IsHideEnter())
		{
			//返回房间满的错误
			WRITE_WARNING_LOG("login request:room full![nRoomID=%d]\n", pRoom->GetRoomID());
			stLoginResp.nResult = enmLoginResult_RoomFull;
			stLoginResp.strFailReason = "RoomFull";
			return S_OK;

		}
		//判断密码
		if(!isOfficialPlayer && !isAdmin)
		{
			if(pRoom->IsNeedPassWord())
			{
				isNeedPassWord = true;
			}
		}
	}
	else
	{
		if(!pRoom->PlayerCanEnter(false))
		{
			//返回房间满的错误
			WRITE_WARNING_LOG("login request:room full![nRoomID=%d]\n", pRoom->GetRoomID());
			stLoginResp.nResult = enmLoginResult_RoomFull;
			stLoginResp.strFailReason = "RoomFull";
			return S_OK;
		}
		//判断密码
		if(pRoom->IsNeedPassWord() && !isAdmin)
		{
			isNeedPassWord = true;
		}
	}
	if(isNeedPassWord)
	{
		if(strcmp(stLoginReq->strRoomPassword.GetString(),"") == 0)
		{
			WRITE_WARNING_LOG("login request:player NeedPassword![nRoleID=%d]\n", pPlayer->GetRoleID());
			stLoginResp.nResult = enmLoginResult_NeedPassword;
			stLoginResp.strFailReason = "NeedPassword";
			return S_OK;
		}
		if(strcmp(stLoginReq->strRoomPassword.GetString(),pRoom->GetRoomPassWord())!=0)
		{
			WRITE_WARNING_LOG("login request:player PasswordWrong![nRoleID=%d]\n", pPlayer->GetRoleID());
			stLoginResp.nResult = enmLoginResult_PasswordWrong;
			stLoginResp.strFailReason = "NeedPassword";
			return S_OK;
		}
	}

	{
		//添加到房间里面去，并发送广播
		ret = pRoom->AddPlayer(nPlayerIndex, pPlayer->GetVipLevel(), pPlayer->IsHideEnter());
		if(ret < 0)
		{
			WRITE_WARNING_LOG("login request:room full![nRoomID=%d]\n", pRoom->GetRoomID());
			stLoginResp.nResult = enmLoginResult_RoomFull;
			stLoginResp.strFailReason = "RoomFull";
			return S_OK;
		}
		if(pPlayer->IsHideEnter())
		{
			WRITE_NOTICE_LOG("login request:player hide enter room{nRoleID=%d, nRoomID=%d, nHideEnterCount=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID(),pRoom->GetHideEnterOfficerCount());
		}
		//如果是官方人员给超管的权限
		if(isOfficialPlayer)
		{
			ret = pRoom->AddMemberList(pPlayer->GetRoleID(),enmRoleRank_Super);
			if(ret < 0)
			{
				WRITE_WARNING_LOG("login request:room full![nRoomID=%d]\n", pRoom->GetRoomID());
				stLoginResp.nResult = enmLoginResult_Other;
				stLoginResp.strFailReason = "Other";
				return S_OK;
			}
		}

		pPlayer->EnterRoom(pRoom->GetRoomID());
		pPlayer->SetRoleRankInRoom(pRoom->GetRoleRank(pPlayer->GetRoleID()),pRoom->GetRoomID());
		pPlayer->SetPlayerType(enmPlayerType_Normal);
		pPlayer->SetKey(Random2(pPlayer->GetRoleID()+pRoom->GetRoomID()),pRoom->GetRoomID());

		stLoginResp.nResult = enmLoginResult_OK;
		stLoginResp.strRoomName = pRoom->GetRoomName();
		stLoginResp.strRoomSignature = pRoom->GetRoomSignature();

		MediaServerInfo stMediaServerInfo = g_DataCenter.GetMediaServerInfo(pPlayer->GetConnInfo().nServerID);
		stLoginResp.nTelMediaIP = stMediaServerInfo.m_nAddress;
		stLoginResp.nTelMediaPort = stMediaServerInfo.m_nPort;
		stLoginResp.nCncMediaIP = stMediaServerInfo.m_nAddress;
		stLoginResp.nCncMediaPort = stMediaServerInfo.m_nPort;

		stLoginResp.strHostName = pRoom->GetHostName();
		stLoginResp.nHost179ID = pRoom->GetHost179ID();
		stLoginResp.nHostGender = pRoom->GetHostGender();
		stLoginResp.nKey = pPlayer->GetKey(pRoom->GetRoomID());
		stLoginResp.nIsCollected = pPlayer->IsCollected(pRoom->GetRoomID());

		stLoginNoti.stRoomUserInfo.nRoleID = pPlayer->GetRoleID();
		stLoginNoti.stRoomUserInfo.nGender = pPlayer->GetPlayerGender();
		stLoginNoti.stRoomUserInfo.n179ID = pPlayer->GetAccountID();
		stLoginNoti.stRoomUserInfo.strRoleName = pPlayer->GetRoleName();
		stLoginNoti.stRoomUserInfo.nVipLevel = pPlayer->GetVipLevel();
		stLoginNoti.stRoomUserInfo.nUserLevel = pPlayer->GetUserLevel();
		stLoginNoti.stRoomUserInfo.nRoleRank = pPlayer->GetRoleRankInRoom(pRoom->GetRoomID());
		stLoginNoti.stRoomUserInfo.nIdentityType = pPlayer->GetIdentityType();
		stLoginNoti.stRoomUserInfo.nClientInfo = pPlayer->GetUserClientInfo();
		stLoginNoti.stRoomUserInfo.nStatus = pPlayer->GetPlayerStatusInRoom(pRoom->GetRoomID());
		if(pRoom->IsInProhibitList(pPlayer->GetRoleID()))
		{
			stLoginNoti.stRoomUserInfo.nStatus|=enmStatusType_FORBIDDEN;
		}
	}

	WRITE_NOTICE_LOG("login request:palyer enter room success{nRoleID=%d, nRoomID=%d, nNetType=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID(),pPlayer->GetPlayerNetType());
	return S_OK;
}

int32_t CLoginMessageEvent::WatchShow(CRoom *pRoom, CPlayer *pPlayer,CLoginResp &stLoginResp,const uint16_t nOptionLen, const void *pOptionData )
{
	if(NULL == pRoom||NULL == pPlayer)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x}\n",pRoom,pPlayer);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	RoleID arrPublicMic[MaxPublicMicCount] = {0};
	uint16_t nPublicMicCount = 0;
	pRoom->GetOnPublicMic(arrPublicMic,MaxPublicMicCount,nPublicMicCount);

//	CAddTransmitUserNotice stAddTransmitUserNotice;
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.nRoomID = pRoom->GetRoomID();
//	stAddTransmitUserNotice.arrAddRoleID[0] = pPlayer->GetRoleID();
	stLoginResp.nPublicMicCount = nPublicMicCount;
	for(int32_t i = 0;i < nPublicMicCount;i++)
	{
		RoleID arrReciverID[1] = {pPlayer->GetRoleID()};
		g_BllBase.SendAddTransmitUserNotifyToMediaServer(arrPublicMic[i], arrReciverID, 1, pRoom, true);
//		stAddTransmitUserNotice.nRoleID = arrPublicMic[i];
//		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_Broadcast,arrPublicMic[i],enmEntityType_Media,pRoom->GetMediaServerIDByType(pPlayer->GetPlayerNetType()));
		WRITE_NOTICE_LOG("login request:watvh public mic{nRoleID=%d, nPublicMic=%d} ",pPlayer->GetRoleID(),arrPublicMic[i]);

		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		g_PlayerMgt.GetPlayer(arrPublicMic[i],pPlayer,nPlayerIndex);
		if(NULL==pPlayer)
		{
			continue;
		}
		stLoginResp.stRoomUserInfo[i].nRoleID = pPlayer->GetRoleID();
		stLoginResp.stRoomUserInfo[i].nGender = pPlayer->GetPlayerGender();
		stLoginResp.stRoomUserInfo[i].n179ID = pPlayer->GetAccountID();
		stLoginResp.stRoomUserInfo[i].strRoleName = pPlayer->GetRoleName();
		stLoginResp.stRoomUserInfo[i].nVipLevel = pPlayer->GetVipLevel();
		stLoginResp.stRoomUserInfo[i].nUserLevel = pPlayer->GetUserLevel();
		stLoginResp.stRoomUserInfo[i].nRoleRank = pPlayer->GetRoleRankInRoom(pRoom->GetRoomID());
		stLoginResp.stRoomUserInfo[i].nIdentityType = pPlayer->GetIdentityType();
		stLoginResp.stRoomUserInfo[i].nClientInfo = pPlayer->GetUserClientInfo();
		stLoginResp.stRoomUserInfo[i].nStatus = pPlayer->GetPlayerStatusInRoom(pRoom->GetRoomID());
		if(pRoom->IsInProhibitList(pPlayer->GetRoleID()))
		{
			stLoginResp.stRoomUserInfo[i].nStatus|=enmStatusType_FORBIDDEN;
		}
		stLoginResp.stRoomUserInfo[i].nItemCount = 0;
	}

	return ret;
}

int32_t CLoginMessageEvent::GetEnterRoomCountFromHall(MessageHeadSS *pMsgHead, CLoginReq* pMsgBody, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
		const uint16_t nOptionLen , const void *pOptionData )
{
	CLoginResp stLoginResp;

	if(NULL == pRoom || NULL == pPlayer || NULL == pMsgHead || NULL == pMsgBody)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x, pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pRoom,pPlayer,pMsgHead,pMsgBody);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_HSRS_GET_ROLE_GLOBALINFO_RESP, enmSessionType_GetEnterCount, this, enmGetEnterCountTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("login request:create get enter room count info session is failed!{RoleID=%d, ret=0x%08x}\n", pMsgHead->nRoleID, ret);

		stLoginResp.nResult = enmLoginResult_CreateSessionFailed;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return ret;
	}

	CGetEnterRoomSessionData * pData = new (pSession->GetSessionData())CGetEnterRoomSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	memcpy(&pData->m_pMsgBody, pMsgBody, sizeof(pData->m_pMsgBody));
	//发送请求
	CGetRoleGlobalInfoReq stCGetRoleGlobalInfoReq;
	SendMessageRequest(MSGID_RSHS_GET_ROLE_GLOBALINFO_REQ, &stCGetRoleGlobalInfoReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Hall, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send GetEnterCount request");

	return S_OK;
}

int32_t CLoginMessageEvent::GetItemInfoFromItem(MessageHeadSS *pMsgHead, CLoginReq* pMsgBody, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
		const uint16_t nOptionLen , const void *pOptionData )
{
	CLoginResp stLoginResp;

	if(NULL == pRoom || NULL == pPlayer || NULL == pMsgHead || NULL == pMsgBody)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x, pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pRoom,pPlayer,pMsgHead,pMsgBody);

		stLoginResp.nResult = enmLoginResult_Unknown;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;

	//如果item正在恢复，则返回错误
	CItemRebulidCtl &stItemRebulidCtl = GET_ITEMREBULIDCTL_INSTANCE();
	if(stItemRebulidCtl.IsItemRebulid())
	{
		WRITE_ERROR_LOG("login request:item server is rebulided!{RoleID=%d}\n", pMsgHead->nRoleID);
		stLoginResp.nResult = enmLoginResult_Timeout;
		stLoginResp.strFailReason = "time out";
		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}

	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_ISRS_GET_ITEMINFO_RESP, enmSessionType_GetItemInfo, this, enmGetItemInfoTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("login request:create get item info session is failed!{RoleID=%d, ret=0x%08x}\n", pMsgHead->nRoleID, ret);

		stLoginResp.nResult = enmLoginResult_CreateSessionFailed;
		stLoginResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_LOGIN_RESP, pMsgHead, &stLoginResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return ret;
	}

	CGetItemInfoSessionData * pData = new (pSession->GetSessionData())CGetItemInfoSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	memcpy(&pData->m_pMsgBody, pMsgBody, sizeof(pData->m_pMsgBody));
	//发送请求
	CGetItemInfoReq stCGetItemInfoReq;
	stCGetItemInfoReq.nRoleID = pMsgHead->nRoleID;

	SendMessageRequest(MSGID_RSIS_GET_ITEMINFO_REQ, &stCGetItemInfoReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Item, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send get item info request");

	return S_OK;
}

int32_t CLoginMessageEvent::DestroyPlayerAndRoom(CRoom *pRoom,CPlayer *pPlayer)
{
	RoleID nRoleID = enmInvalidRoleID;
	//删除玩家
	if(NULL == pPlayer)
	{
		WRITE_ERROR_LOG("null pointer:{pPlayer=0x%08x}\n",pPlayer);
	}
	else if(pPlayer->GetCurEnterRoomCount() == 0)
	{
		nRoleID = pPlayer->GetRoleID();
		g_PlayerMgt.DestroyPlayer(nRoleID);
		WRITE_DEBUG_LOG("player do not in any room {nRoleID=%d}\n",nRoleID);
	}
	//删除房间
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
	}
//	else if(pRoom->GetCurUserCount()==0&&pRoom->GetRobotCount() == 0)
//	{
//		nRoomID = pRoom->GetRoomID();
//		CDestoryRoomNotice stDestoryRoomNotice;
//		stDestoryRoomNotice.nRoomID = nRoomID;
//		stDestoryRoomNotice.nServerID = g_Frame.GetServerConfigImpl()->GetServerID();
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Item);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_RoomDispatcher);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Hall);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Tunnel);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));
//		WRITE_DEBUG_LOG("Room do not has player so destroy{nRoomID=%d}\n",nRoomID);
//		g_RoomMgt.DestroyRoom(nRoomID);
//	}
	return S_OK;
}

int32_t CLoginMessageEvent::DistributionRobot(CRoom *pRoom)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	//判断是否需要加机器人
	CStaPlayerCount stStaPlayerCount;
	int32_t nNeedRobotCount = pRoom->GetNeedRobotCount();
//	if(nNeedRobotCount < 0)
//	{
//		ret = KickRobot(pRoom,-nNeedRobotCount);
//		if(ret<0)
//		{
//			WRITE_ERROR_LOG("kick reboot filed!\n");
//		}
//
//	}
	for(int32_t i = 0;i < nNeedRobotCount;i++)
	{
		int32_t nGetCount = 0;
		uint32_t nRoleIDBegin = g_RebotConfig.GetRoleIDInfo().nRoleIDBegin;
		uint32_t nRoleIDEnd = g_RebotConfig.GetRoleIDInfo().nRoleIDEnd;
		int32_t nRoleCount = nRoleIDEnd - nRoleIDBegin+1;

		while(nGetCount < nRoleCount)
		{
			nGetCount++;
			RoleID nRobotRoleID = GET_REBOTCTL_INSTANCE().GetRebotRoleID();
			CPlayer *pPlayer = NULL;
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			ret = g_PlayerMgt.GetPlayer(nRobotRoleID, pPlayer, nPlayerIndex);
			if(ret < 0 || pPlayer == NULL)
			{
				//玩家不存在，创建玩家
				ret = g_PlayerMgt.CreatePlayer(nRobotRoleID, pPlayer, nPlayerIndex);
				if(ret < 0 || pPlayer == NULL)
				{
					WRITE_ERROR_LOG(" create robot %d filed !\n",nRobotRoleID);
					continue;
				}
			}
			//设置机器人的相关属性
			pPlayer->SetVipLevel(GET_REBOTCTL_INSTANCE().GetRebootVip());
			//现在是一个机器人进一个房间（后面可调）
			if(!(pPlayer->CanEnterRoom()))
			{
				WRITE_DEBUG_LOG("robot %d in other room !\n",nRobotRoleID);
				continue;
			}
			pPlayer->SetAccountID(GET_REBOTCTL_INSTANCE().GetRebot179ID());
			pPlayer->SetRoleName(GET_REBOTCTL_INSTANCE().GetRebotRoleName());
			pPlayer->SetIdentityType(enmIdentityType_None);
			//设置为机器人
			pPlayer->SetPlayerType(enmPlayerType_Rebot);
			//设置玩家的性别
			pPlayer->SetPlayerGender((pPlayer->GetAccountID())%2);
			//设置玩家的财富等级
			pPlayer->SetMagnateLevel(GET_REBOTCTL_INSTANCE().GetMagnateLevel(pPlayer->GetVipLevel()));
			//设置玩家的clientInfo
			if(GET_REBOTCTL_INSTANCE().IsHasMic())
			{
				pPlayer->SetUserClientInfo(enmUserClientInfo_PC|enmUserClientInfo_HasCamera);
			}

			ret = pRoom->AddRebotPlayer(nRobotRoleID);
			if(ret<0)
			{
				if (ret == (int32_t)E_RS_KEYEXIST)
				{
					WRITE_NOTICE_LOG(" robot %d is in this room %d !\n",nRobotRoleID,pRoom->GetRoomID());
					continue;
				}
				WRITE_ERROR_LOG(" robot is too more in room %d !\n",pRoom->GetRoomID());
				return ret;
			}
			pPlayer->EnterRoom(pRoom->GetRoomID());
			CLoginNoti stLoginNoti;
			stLoginNoti.stRoomUserInfo.nRoleID = pPlayer->GetRoleID();
			stLoginNoti.stRoomUserInfo.nGender = pPlayer->GetPlayerGender();
			stLoginNoti.stRoomUserInfo.n179ID = pPlayer->GetAccountID();
			stLoginNoti.stRoomUserInfo.strRoleName = pPlayer->GetRoleName();
			stLoginNoti.stRoomUserInfo.nVipLevel = pPlayer->GetVipLevel();
			stLoginNoti.stRoomUserInfo.nUserLevel = pPlayer->GetUserLevel();
			stLoginNoti.stRoomUserInfo.nRoleRank = pPlayer->GetRoleRankInRoom(pRoom->GetRoomID());
			stLoginNoti.stRoomUserInfo.nIdentityType = pPlayer->GetIdentityType();
			stLoginNoti.stRoomUserInfo.nClientInfo = pPlayer->GetUserClientInfo();
			stLoginNoti.stRoomUserInfo.nStatus = pPlayer->GetPlayerStatusInRoom(pRoom->GetRoomID());
			if(pRoom->IsInProhibitList(pPlayer->GetRoleID()))
			{
				stLoginNoti.stRoomUserInfo.nStatus|=enmStatusType_FORBIDDEN;
			}
			pPlayer->GetItem(stLoginNoti.stRoomUserInfo.arrItemID,MaxOnUserItemCount,stLoginNoti.stRoomUserInfo.nItemCount);
			stLoginNoti.stRoomUserInfo.nMagnateLevel = pPlayer->GetMagnateLevel();
			//给客户端发送通知
			SendMessageNotifyToClient(MSGID_RSCL_LOGIN_NOTI, &stLoginNoti, pRoom->GetRoomID(), enmBroadcastType_All,
					pPlayer->GetRoleID());
			//给需要的服务器发送通知
			CEnterRoomNotice stEnterRoomNotice;
			stEnterRoomNotice.nRoleID = pPlayer->GetRoleID();
			stEnterRoomNotice.nRoomID = pRoom->GetRoomID();
			stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
			stEnterRoomNotice.nPlayerType = enmPlayerType_Rebot;
			stEnterRoomNotice.nRoleRank = enmRoleRank_None;
			stEnterRoomNotice.strRoleName = pPlayer->GetRoleName();
			stEnterRoomNotice.nAccountID = pPlayer->GetAccountID();
			stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
			stEnterRoomNotice.nIdentityType = pPlayer->GetIdentityType();

			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Item);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_RoomDispatcher);
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_HallDataCenter);
			break;
		}
	}
	stStaPlayerCount.nRoomID = pRoom->GetRoomID();
	stStaPlayerCount.nPlayerCount = pRoom->GetCurUserCount();
	stStaPlayerCount.nRobotCount = pRoom->GetRobotCount();

	SendMessageNotifyToServer(MSGID_RSDB_STA_PLAYER_COUNT_NOTIFY, &stStaPlayerCount, pRoom->GetRoomID(), enmTransType_ByKey, pRoom->GetRoomID(), enmEntityType_DBProxy);
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END

