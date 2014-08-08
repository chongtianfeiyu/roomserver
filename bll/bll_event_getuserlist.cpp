/*
 * bll_event_getuserlist.cpp
 *
 *  Created on: 2011-12-24
 *      Author: jimm
 */

#include "bll_event_getuserlist.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "dal/to_client_message.h"
#include "config/rebotconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

static RoomUserInfo arrRoomUserInfo[MaxUserCountPerRoom];

int32_t CGetUsetListMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetItemInfo:
		GetItemInfoResp(pMsgHead, pMsgBody, pSession);
		break;
		default:
		break;
	}

	return S_OK;
}
int32_t CGetUsetListMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}
	switch(pSession->GetSessionType())
	{
		case enmSessionType_GetItemInfo:
		GetItemInfoTimeout(pSession);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CGetUsetListMessageEvent::GetItemInfoResp(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession)
{
	int32_t ret = S_OK;
	CGetItemInfoSessionData * pData =(CGetItemInfoSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("get userlist:recv get item info resp {nRoleID=%d, nRoomID=%d}\n",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
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
	CGetUserListResp stGetUserListResp;
	CGetItemInfoResp *pGetItemInfoResp = dynamic_cast<CGetItemInfoResp *>(pMsgBody);
	if(NULL == pGetItemInfoResp)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);

		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, &pData->m_sMsgHead, &stGetUserListResp,enmTransType_P2P);

		return E_NULLPOINTER;
	}

	if(pGetItemInfoResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("get item info failed{ret=0x%8X}\n",pGetItemInfoResp->nResult);
		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, &pData->m_sMsgHead, &stGetUserListResp,enmTransType_P2P);
		return pGetItemInfoResp->nResult;
	}

	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);
	RoleID arrRobotRoleID[MaxUserCountPerRoom];
	int32_t nRobotCount = 0;
	pRoom->GetAllRebotPlayers(arrRobotRoleID,MaxUserCountPerRoom,nRobotCount);
	int32_t nTotalPlayerCount = nPlayerCount+nRobotCount;
	CPlayer *pPlayerTemp = NULL;
	int32_t nUserInfoIndex = 0; //房间玩家信息的下标
	int32_t nIndexTemp = 0; //房间玩家信息的下标
	for(int32_t i = 0; i < nTotalPlayerCount && i < MaxUserCountPerRoom; ++i)
	{

		if(i<nPlayerCount)
		{
			pPlayerTemp = arrPlayer[i];
		}
		else
		{
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			g_PlayerMgt.GetPlayer(arrRobotRoleID[i-nPlayerCount],pPlayerTemp,nPlayerIndex);
		}

		if(NULL==pPlayerTemp)
		{
			continue;
		}
		if(pPlayerTemp->GetRoleID() == pPlayer->GetRoleID() )
		{
			nIndexTemp = nUserInfoIndex;
			nUserInfoIndex = -1;  //自己放在第一个包返回
		}
		++nUserInfoIndex;
		arrRoomUserInfo[nUserInfoIndex].nRoleID = pPlayerTemp->GetRoleID();
		arrRoomUserInfo[nUserInfoIndex].nGender = pPlayerTemp->GetPlayerGender();
		arrRoomUserInfo[nUserInfoIndex].n179ID = pPlayerTemp->GetAccountID();
		arrRoomUserInfo[nUserInfoIndex].strRoleName = pPlayerTemp->GetRoleName();
		arrRoomUserInfo[nUserInfoIndex].nVipLevel = pPlayerTemp->GetVipLevel();
		arrRoomUserInfo[nUserInfoIndex].nUserLevel = pPlayerTemp->GetUserLevel();
		arrRoomUserInfo[nUserInfoIndex].nRoleRank = pPlayerTemp->GetRoleRankInRoom(pMsgHead->nRoomID);
		arrRoomUserInfo[nUserInfoIndex].nIdentityType = pPlayerTemp->GetIdentityType();
		arrRoomUserInfo[nUserInfoIndex].nClientInfo = pPlayerTemp->GetUserClientInfo();
		arrRoomUserInfo[nUserInfoIndex].nStatus = pPlayerTemp->GetPlayerStatusInRoom(pRoom->GetRoomID());
		if(pRoom->IsInProhibitList(pPlayerTemp->GetRoleID()))
		{
			arrRoomUserInfo[nUserInfoIndex].nStatus|=enmStatusType_FORBIDDEN;
		}
		arrRoomUserInfo[nUserInfoIndex].nItemCount = 0;
		//根据获得的信息得到道具
		for(uint32_t j = 0;j < pGetItemInfoResp->nPlayerCount;j++)
		{
			if(pGetItemInfoResp->arrPlayerItemInfo[j].nRoleID == pPlayerTemp->GetRoleID())
			{
				arrRoomUserInfo[nUserInfoIndex].nItemCount = pGetItemInfoResp->arrPlayerItemInfo[j].nItemCount;
				for(uint32_t k=0;k<arrRoomUserInfo[nUserInfoIndex].nItemCount;k++)
				{
					arrRoomUserInfo[nUserInfoIndex].arrItemID[k] = pGetItemInfoResp->arrPlayerItemInfo[j].arrItemID[k];
				}
				if(pPlayerTemp->IsReboot())
				{
					arrRoomUserInfo[nUserInfoIndex].nMagnateLevel = pPlayerTemp->GetMagnateLevel();
				}
				else
				{
					arrRoomUserInfo[nUserInfoIndex].nMagnateLevel = pGetItemInfoResp->arrMagnateLevel[j];
				}
				break;
			}
		}
		if(pPlayerTemp->GetRoleID() == pPlayer->GetRoleID() )
		{
			nUserInfoIndex = nIndexTemp;
		}
	}
	//初始化result
	stGetUserListResp.nResult = 0;
	stGetUserListResp.strFailReason = "";
	//需要发送的次数
	int32_t nCount = ((nTotalPlayerCount/ GetRoomUserInfoCountPerOnce) + 1);
	//已经完成发送的数量
	int32_t index = 0;
	for(int32_t i = 0; i < nCount; ++i)
	{
		stGetUserListResp.nCount = 0;
		memset(stGetUserListResp.arrRoomUserInfo, 0, sizeof(stGetUserListResp.arrRoomUserInfo));

		int32_t nCopyCount = (nTotalPlayerCount - index > GetRoomUserInfoCountPerOnce ? GetRoomUserInfoCountPerOnce : nTotalPlayerCount - index);
		memcpy(stGetUserListResp.arrRoomUserInfo, &arrRoomUserInfo[index], nCopyCount * sizeof(RoomUserInfo));
		stGetUserListResp.nCount = nCopyCount;

		index += nCopyCount;
		//已经是最后一轮了
		if(i+1 >= nCount)
		{
			stGetUserListResp.nEndFlag = 1;
		}
		else
		{
			stGetUserListResp.nEndFlag = 0;
		}

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, &pData->m_sMsgHead, &stGetUserListResp,
				enmTransType_P2P);
	}
	//登录次数小于一个次数时，进房进行彩条欢迎
//	if(pPlayer->GetLoginTimes() < g_RebotConfig.GetNewPlayerLoginTimes())
//	{
//		SendColorToPalyer(pRoom,pPlayer);
//	}

	//进入房间通知
	CEnterRoomNotice stEnterRoomNotice;
	stEnterRoomNotice.nRoleID = pPlayer->GetRoleID();
	stEnterRoomNotice.nRoomID = pRoom->GetRoomID();
	stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stEnterRoomNotice.nPlayerType = enmPlayerType_Normal;
	stEnterRoomNotice.nRoleRank = pRoom->GetRoleRank(pPlayer->GetRoleID());
	stEnterRoomNotice.strRoleName = pPlayer->GetRoleName();
	stEnterRoomNotice.nAccountID = pPlayer->GetAccountID();
	stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
	stEnterRoomNotice.nIdentityType = pPlayer->GetIdentityType();
	stEnterRoomNotice.nKey = pPlayer->GetKey(pRoom->GetRoomID());

	SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_SysInfo);
	return S_OK;
}

int32_t CGetUsetListMessageEvent::GetItemInfoTimeout(CFrameSession* pSession)
{
	CGetItemInfoSessionData * pData =(CGetItemInfoSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("get item info time out{nRoleID=%d, nRoomID=%d, nSessionIndex=%d}",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID,pSession->GetSessionIndex());
	CGetUserListResp stGetUserListResp;

	stGetUserListResp.nResult = 1;
	stGetUserListResp.strFailReason = "get item info timeout";

	SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, &pData->m_sMsgHead, &stGetUserListResp,
			enmTransType_P2P);

	return S_OK;
}

int32_t CGetUsetListMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	CGetUserListResp stGetUserListResp;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CGetUserListReq *pGetUserListMsg = dynamic_cast<CGetUserListReq *>(pMsgBody);
	if(NULL == pGetUserListMsg)
	{
		WRITE_ERROR_LOG("null pointer:{pGetUserListMsg=0x%08x}\n",pGetUserListMsg);
		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);

		return E_NULLPOINTER;
	}

	WRITE_DEBUG_LOG("recv player getuserlist request{nRoleID=%d, nRoomID=%d}", pMsgHead->nRoleID,pMsgHead->nRoomID);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("get player object error{nRoleID=%d,ret=0x%08x}", pMsgHead->nRoleID, ret);

		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "get user list error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get player object error{nRoleID=%d,ret=%08X}", pMsgHead->nRoleID, ret);

		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "get user list error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}

	//玩家不在此房间
	if(!pRoom->IsPlayerInRoom(nPlayerIndex))
	{
		WRITE_ERROR_LOG("get player object error{nRoleID=%d,ret=%08X}", pMsgHead->nRoleID, ret);

		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "get user list error";

		SendMessageResponse( MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return S_OK;
	}

//		CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
//		int32_t nPlayerCount = 0;
//		g_DataCenter.GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);
//		RoleID arrRobotRoleID[MaxUserCountPerRoom];
//		int32_t nRobotCount = 0;
//		pRoom->GetAllRebotPlayers(arrRobotRoleID,MaxUserCountPerRoom,nRobotCount);
//		int32_t nTotalPlayerCount = nPlayerCount+nRobotCount;
//		for(int32_t i = 0; i < nTotalPlayerCount; ++i)
//		{
//			CPlayer *pPlayerTemp = NULL;
//			if(i<nPlayerCount)
//			{
//				pPlayerTemp = arrPlayer[i];
//			}
//			else
//			{
//				//			CPlayer *pRobot = NULL;
//				PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
//				g_PlayerMgt.GetPlayer(arrRobotRoleID[i-nPlayerCount],pPlayerTemp,nPlayerIndex);
//				//			pPlayerTemp = pRobot;
//			}
//			arrRoomUserInfo[i].nRoleID = pPlayerTemp->GetRoleID();
//			arrRoomUserInfo[i].nGender = pPlayerTemp->GetPlayerGender();
//			arrRoomUserInfo[i].n179ID = pPlayerTemp->GetAccountID();
//			arrRoomUserInfo[i].strRoleName = pPlayerTemp->GetRoleName();
//			arrRoomUserInfo[i].nVipLevel = pPlayerTemp->GetVipLevel();
//			arrRoomUserInfo[i].nUserLevel = pPlayerTemp->GetUserLevel();
//			arrRoomUserInfo[i].nRoleRank = pPlayerTemp->GetRoleRankInRoom(pMsgHead->nRoomID);
//			arrRoomUserInfo[i].nClientInfo = pPlayerTemp->GetUserClientInfo();
//			arrRoomUserInfo[i].nStatus = pPlayerTemp->GetPlayerStatusInRoom(pRoom->GetRoomID());
//			if(pRoom->IsInProhibitList(pPlayerTemp->GetRoleID()))
//			{
//				arrRoomUserInfo[i].nStatus|=enmStatusType_FORBIDDEN;
//			}
//			pPlayerTemp->GetItem(arrRoomUserInfo[i].arrItemID,MaxOnUserItemCount,arrRoomUserInfo[i].nItemCount);
//			//		arrRoomUserInfo[i].nStampPid = 0;
//		}
//		//初始化result
//		stGetUserListResp.nResult = 0;
//		stGetUserListResp.strFailReason = "";
//
//		int32_t nCount = ((nTotalPlayerCount/ GetRoomUserInfoCountPerOnce) + 1);
//		int32_t index = 0;
//		for(int32_t i = 0; i < nCount; ++i)
//		{
//			stGetUserListResp.nCount = 0;
//			memset(stGetUserListResp.arrRoomUserInfo, 0, sizeof(stGetUserListResp.arrRoomUserInfo));
//
//			int32_t nCopyCount = (nTotalPlayerCount - index > GetRoomUserInfoCountPerOnce ? GetRoomUserInfoCountPerOnce : nTotalPlayerCount - index);
//			memcpy(stGetUserListResp.arrRoomUserInfo, &arrRoomUserInfo[index], nCopyCount * sizeof(RoomUserInfo));
//			stGetUserListResp.nCount = nCopyCount;
//
//			index += nCopyCount;
//			//已经是最后一轮了
//			if(index >= nCount)
//			{
//				stGetUserListResp.nEndFlag = 1;
//			}
//			else
//			{
//				stGetUserListResp.nEndFlag = 0;
//			}
//
//			SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,
//					enmTransType_P2P, nOptionLen, (char *)pOptionData);
//		}
//		ret = SrvPrivateShow(pRoom,pPlayer,nOptionLen, pOptionData);
//		if(ret<0)
//		{
//			WRITE_NOTICE_LOG("player %d srv private mic filed !\n",pPlayer->GetRoleID());
//		}
	return GetItemInfoFromItem(pMsgHead,pPlayer,pRoom,nPlayerIndex,nOptionLen,pOptionData);
}
int32_t CGetUsetListMessageEvent::GetItemInfoFromItem(MessageHeadSS *pMsgHead, CPlayer *pPlayer,CRoom *pRoom,PlayerIndex nPlayerIndex,
		const uint16_t nOptionLen , const void *pOptionData )
{
	if(NULL==pRoom||NULL==pPlayer||NULL==pMsgHead)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x, pMsgHead=0x%08x}\n",pRoom,pPlayer,pMsgHead);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CGetUserListResp stGetUserListResp;
	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_ISRS_GET_ITEMINFO_RESP, enmSessionType_GetItemInfo, this, enmGetItemInfoTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("create get item info session is failed!{nRoleID=%d}\n", pMsgHead->nRoleID);

		stGetUserListResp.nResult = 1;
		stGetUserListResp.strFailReason = "unknown error";

		SendMessageResponse(MSGID_RSCL_GETUSERLIST_RESP, pMsgHead, &stGetUserListResp,enmTransType_P2P);

		return ret;
	}

	CGetItemInfoSessionData * pData = new (pSession->GetSessionData())CGetItemInfoSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	//发送请求
	CGetItemInfoReq stCGetItemInfoReq;
	//为0是表示获取房间里所有人的
	stCGetItemInfoReq.nRoleID = 0;

	SendMessageRequest(MSGID_RSIS_GET_ITEMINFO_REQ, &stCGetItemInfoReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_Item, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send get item info request");

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

