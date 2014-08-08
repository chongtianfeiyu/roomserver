/*
 *  bll_event_modifytitle.cpp
 *
 *  To do：
 *  Created on: 	2012-3-5
 *  Author: 		luocj
 */

#include "bll_event_modifytitle.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "hallserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CModifyTitleMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
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

	CGetOfflineArtistInfoResp *pGetArtistInfoResp = dynamic_cast<CGetOfflineArtistInfoResp *>(pMsgBody);
	if(NULL == pGetArtistInfoResp)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pGetArtistInfoResp);
		return E_NULLPOINTER;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pData->m_pMsgBody.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("room is not on this server!{ ret=0x%08x}\n", ret);
		return ret;
	}
	if((pGetArtistInfoResp->nIdentityType & enmIdentityType_SONGER) ||
			(pGetArtistInfoResp->nIdentityType & enmIdentityType_DANCER) ||
			(pGetArtistInfoResp->nIdentityType & enmIdentityType_HOST))//是否为艺人身份
	{
		RoomUserInfo     nRoomUserInfo;
		nRoomUserInfo.nRoleID = pGetArtistInfoResp->nRoleID;
		nRoomUserInfo.nGender = pGetArtistInfoResp->nGender;
		nRoomUserInfo.n179ID  = pGetArtistInfoResp->n179ID;
		nRoomUserInfo.strRoleName = pGetArtistInfoResp->strRoleName;
		nRoomUserInfo.nVipLevel = pGetArtistInfoResp->nVipLevel;
		nRoomUserInfo.nRoleRank = pGetArtistInfoResp->nRoleRank;
		nRoomUserInfo.nIdentityType = pGetArtistInfoResp->nIdentityType;
		nRoomUserInfo.nClientInfo = pGetArtistInfoResp->nClientInfo;
		nRoomUserInfo.nStatus = 0;
		nRoomUserInfo.nItemCount = 0;
		nRoomUserInfo.nMagnateLevel = pGetArtistInfoResp->nMagnateLevel;

		pRoom->m_RoomArtistList.AddArtistInfo(nRoomUserInfo, enmOffline_stat);
	}
	return S_OK;
}
int32_t CModifyTitleMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CModifyTitleMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CModifyTitleNoti *pModifyTitleNoti = dynamic_cast<CModifyTitleNoti *>(pMsgBody);
	if(NULL == pModifyTitleNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pModifyTitleNoti=0x%08x}\n",pModifyTitleNoti);
		return E_NULLPOINTER;
	}

	WRITE_NOTICE_LOG("modify rolerank:recv notify{nRoleID=%d, nRoleRank=%d, nRoomID=%d}\n",pModifyTitleNoti->nRoleID,pModifyTitleNoti->nRoleRank,pModifyTitleNoti->nRoomID);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pModifyTitleNoti->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_WARNING_LOG("player does no exist{nRoleID=%d}",pModifyTitleNoti->nRoleID);
		//此处不能返回，涉及到清除此玩家在房间里的管理权限
	}

	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pModifyTitleNoti->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("room is not on this server!{nRoomID=%d, ret=0x%08x}\n",pModifyTitleNoti->nRoomID,ret);
		return ret;
	}
	else
	{
		//更新主播列表
		if(pPlayer != NULL && pModifyTitleNoti->nRoleRank > enmRoleRank_TempAdmin)//玩家在线且等级在管理员以上时更新
		{
			if((pPlayer->GetIdentityType()& enmIdentityType_SONGER) ||
					(pPlayer->GetIdentityType()& enmIdentityType_DANCER) ||
					(pPlayer->GetIdentityType()& enmIdentityType_HOST))//是否为艺人身份
			{
				RoomUserInfo  stRoomUserInfo;
				stRoomUserInfo.nRoleID = pPlayer->GetRoleID();
				stRoomUserInfo.nGender = pPlayer->GetPlayerGender();
				stRoomUserInfo.n179ID = pPlayer->GetAccountID();
				stRoomUserInfo.strRoleName = pPlayer->GetRoleName();
				stRoomUserInfo.nVipLevel = pPlayer->GetVipLevel();
				stRoomUserInfo.nUserLevel = pPlayer->GetUserLevel();
				stRoomUserInfo.nRoleRank = pModifyTitleNoti->nRoleRank;
				stRoomUserInfo.nIdentityType = pPlayer->GetIdentityType();
				stRoomUserInfo.nClientInfo = pPlayer->GetUserClientInfo();
				stRoomUserInfo.nStatus = pPlayer->GetPlayerStatusInRoom(pRoom->GetRoomID());
				if(pRoom->IsInProhibitList(pPlayer->GetRoleID()))
				{
					stRoomUserInfo.nStatus|=enmStatusType_FORBIDDEN;
				}
				pPlayer->GetItem(stRoomUserInfo.arrItemID,MaxOnUserItemCount,stRoomUserInfo.nItemCount);
			    stRoomUserInfo.nMagnateLevel = pPlayer->GetMagnateLevel();
				pRoom->m_RoomArtistList.AddArtistInfo(stRoomUserInfo, enmOnline_stat);
			}
		}
		if(pPlayer == NULL && pModifyTitleNoti->nRoleRank > enmRoleRank_TempAdmin)//玩家不在线且等级在管理员以上时更新
		{
			//到数据库获取房间信息
			CFrameSession *pSession = NULL;
			ret = g_Frame.CreateSession(MSGID_DBRS_GETROLEIDENTITY_RESP, enmSessionType_GetArtistInfo, this, enmGetRoomInfoTimeoutPeriod, pSession);
			if(ret < 0 || pSession == NULL)
			{
				WRITE_ERROR_LOG("create get artist info session is failed!{RoomID=%d}\n", pMsgHead->nRoomID);
			}
			else
			{
				CGetArtistInfoSessionData * pArtistData = new (pSession->GetSessionData())CGetArtistInfoSessionData();
				memcpy(&pArtistData->m_sMsgHead, pMsgHead, sizeof(pArtistData->m_sMsgHead));
				if(nOptionLen > 0)
				{
					pArtistData->m_nOptionLen = nOptionLen;
					memcpy(pArtistData->m_arrOptionData, pOptionData, nOptionLen);
				}
				memcpy(&pArtistData->m_pMsgBody, pModifyTitleNoti, sizeof(pArtistData->m_pMsgBody));
				//发送请求
				CGetOfflineArtistInfoReq stGetRoomArtistReq;
				stGetRoomArtistReq.nRoleID = pModifyTitleNoti->nRoleID;

				SendMessageRequest(MSGID_RSDB_GETROLEIDENTITY_REQ , &stGetRoomArtistReq, pMsgHead->nRoomID,
						enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
						pSession->GetSessionIndex(), 0, NULL, "send GetArtistInfo request");
			}
		}

		if(pModifyTitleNoti->nRoleRank == enmRoleRank_None) //降级时更新
		{
			pRoom->m_RoomArtistList.DelArtistInfo(pModifyTitleNoti->nRoleID);
		}

		//统计玩家的在线时间(涉及到管理员在房间时长)
		if(pPlayer != NULL)
		{
			ret = StaticInRoomTime(pPlayer,pRoom);
			if(ret < 0)
			{
				WRITE_ERROR_LOG(" player static in room time filed!{nRoleID=%d, nRoomID=%d,ret=0x%08x}",pModifyTitleNoti->nRoleID,pModifyTitleNoti->nRoomID,ret);
			}
		}

		if(pModifyTitleNoti->nRoleRank != enmRoleRank_None)
		{
			ret = pRoom->AddMemberList(pModifyTitleNoti->nRoleID,pModifyTitleNoti->nRoleRank);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("set rank in room filed{ret=0x%08x}\n",ret);
				return ret;
			}
		}
		else
		{
			ret = pRoom->DelMemberList(pModifyTitleNoti->nRoleID);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("del rank in room filed{ret=0x%08x}\n",ret);
				return ret;
			}
		}
	}

	if(pPlayer == NULL)
	{
		WRITE_WARNING_LOG("player does no exist{nRoleID=%d}",pModifyTitleNoti->nRoleID);
		return ret;
	}

	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex))&&!(pRoom->IsRebotPlayerInRoom(pModifyTitleNoti->nRoleID)))
	{
		WRITE_ERROR_LOG("player does not in room{nRoleID=%d, nRoomID=%d}",pModifyTitleNoti->nRoleID,pModifyTitleNoti->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	ret = pPlayer->SetRoleRankInRoom(pModifyTitleNoti->nRoleRank,pModifyTitleNoti->nRoomID);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("set rank in room filed{ret=0x%08x}\n",ret);
		return ret;
	}
	if(pModifyTitleNoti->nRoleRank != enmRoleRank_None)
	{
		ret = pPlayer->AddAdminRoom(pModifyTitleNoti->nRoomID,pModifyTitleNoti->nRoleRank);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("add rank in room filed{ret=0x%08x}\n",ret);
			return ret;
		}
	}
	else
	{
		ret = pPlayer->DelAdminRoom(pModifyTitleNoti->nRoomID);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("del rank in room filed{ret=0x%08x}\n",ret);
			return ret;
		}
	}
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
