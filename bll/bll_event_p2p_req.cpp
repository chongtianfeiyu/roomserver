/*
 *  bll_event_p2p_req.cpp
 *
 *  To do：
 *  Created on: 	2011-12-19
 *  Author: 		luocj
 */

#include "bll_event_p2p_req.h"
#include "../dal/to_client_message.h"
#include "ctl/server_datacenter.h"
#include "dal/to_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CP2PReqEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CP2PReqEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CP2PReqEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_SENDP2P_REQ:
		OnEventP2PReq(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * 处理请求建立一对一视频（来自client）
 */
int32_t CP2PReqEvent::OnEventP2PReq(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CP2PMicReq *pP2PMicReq = dynamic_cast<CP2PMicReq *>(pMsgBody);
	if(pP2PMicReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pP2PMicReq=0x%08x}\n",pP2PMicReq);
		return E_NULLPOINTER;
	}
    if(pMsgHead->nRoleID == pP2PMicReq->nDestRoleID)
    {
    	WRITE_ERROR_LOG("recv in room p2pReq Dest is Src{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
    	return ret;
    }
	WRITE_NOTICE_LOG("p2p req:recv roleID in room req p2p{nRoleID=%d, nRoomID=%d, nDestRoleID=%d} \n",pMsgHead->nRoleID,pMsgHead->nRoomID,pP2PMicReq->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		//没有次玩家
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID, ret);
		return S_OK;
	}
	//判断玩家是否在房间中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//获取被请求的玩家
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pP2PMicReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	if(ret < 0 || pDestPlayer == NULL)
	{
		//没有次玩家
		WRITE_ERROR_LOG("player does no exist{nDestRoleID=%d}",pP2PMicReq->nDestRoleID);
		return ret;
	}
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex))&&!pRoom->IsRebotPlayerInRoom(pP2PMicReq->nDestRoleID))
	{
		WRITE_ERROR_LOG("player does no in room{nDestRoleID=%d, nRoomID=%d}",pP2PMicReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	ConnectP2PResult nConnectP2PResult = enmConnectP2PResult_OK;
    //todo 判断玩家的VIP够不够
	CheckVipLevel(pPlayer->GetVipLevel(),pDestPlayer->GetPlayerSelfSetting(),nConnectP2PResult);
	if(nConnectP2PResult != enmConnectP2PResult_OK)
	{
		//回应答复
		SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
		return ret;
	}
	//判断消息来时的请求建立一对一视频的玩家的状态(自己以及请求者)
	PlayerState nPlayerState = pPlayer->GetPlayerGlobalState();
	if(enmInvalidPlayerState == nPlayerState)
	{
		ret = E_RS_PLAYRENOTINROOM;
		WRITE_ERROR_LOG("get playerState in room error!{nRoleID=%d, ret=0x%08x}\n",pMsgHead->nRoleID,ret);
		return ret;
	}
	PlayerState nDestPlayerState = pDestPlayer->GetPlayerGlobalState();
	if(enmInvalidPlayerState == nDestPlayerState&&!(pDestPlayer->IsReboot()))
	{
		ret = E_RS_PLAYRENOTINROOM;
		WRITE_ERROR_LOG("get playerState in room error!{nDestRoleID=%d, ret=0x%08x}\n",pP2PMicReq->nDestRoleID,ret);
		return ret;
	}

	GenerateConnectP2PResult(nPlayerState,nConnectP2PResult,true);
	if(nConnectP2PResult != enmConnectP2PResult_OK)
	{
		//回应答复
		SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
		return ret;
	}
	GenerateConnectP2PResult(nDestPlayerState,nConnectP2PResult,false);
	if(nConnectP2PResult != enmConnectP2PResult_OK)
	{
		//回应答复
		SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
		return ret;
	}

    //答复等待对方同意
	SendRespToClient(pMsgHead,nConnectP2PResult,nOptionLen,pOptionData);
	//通知被请求者
	SendNotifyToClient(pMsgHead->nRoomID,pP2PMicReq->nDestRoleID,pMsgHead->nRoleID,nOptionLen,pOptionData);

	WRITE_NOTICE_LOG("player in room eq p2p mic to player success{nRoleID=%d, nRoomID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pP2PMicReq->nDestRoleID);
	return ret;
}
int32_t CP2PReqEvent::GenerateConnectP2PResult(PlayerState nPlayerState,ConnectP2PResult &nConnectP2PResult,bool isSelf)
{
	int32_t ret = S_OK;
	switch(nPlayerState)
	{
		case enmPlayerState_PublicMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_PUBLIC;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_PUBLIC;
			}
		}

		break;
		case enmPlayerState_PrivateMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_PRIVATE;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_PRIVATE;
			}
		}
		break;
		case enmPlayerState_P2PMic:
		{
			if(isSelf)
			{
				nConnectP2PResult = enmConnectP2PResult_SELF_ON_P2P;
			}
			else
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_ON_P2P;
			}
		}
		break;
		default:
		break;
	}
	return ret;
}

int32_t CP2PReqEvent::SendRespToClient(MessageHeadSS *pMsgHead,ConnectP2PResult nConnectP2PResult,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	CP2PResp stP2PResp;
	stP2PResp.nConnectP2PResult = nConnectP2PResult;
	SendMessageResponse(MSGID_RSCL_SENDP2P_RESP, pMsgHead, &stP2PResp,enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return ret;
}

int32_t CP2PReqEvent::SendNotifyToClient(RoomID nRoomID,RoleID nDestRoleID,RoleID nReqRoleID, const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	CP2PRequestNoti stP2PRequestNoti;
	stP2PRequestNoti.nSrcRoleID = nReqRoleID;
	SendMessageNotifyToClient(MSGID_RSCL_SENDP2P_NOTI, &stP2PRequestNoti, nRoomID, enmBroadcastType_ExpectPlayr,
			nDestRoleID, nOptionLen, (char*)pOptionData, "send req p2p notify");
	return ret;
}

int32_t CP2PReqEvent::CheckVipLevel(VipLevel nReqVipLevel,PlayerSelfSetting nDestPlayerSelfSetting,ConnectP2PResult &nConnectP2PResult)
{
	int32_t ret = S_OK;
	//请求者的vip等级低于vip
	if(enmVipLevel_RED > nReqVipLevel)
	{
		nConnectP2PResult = enmConnectP2PResult_SELF_NOT_VIP;
		return ret;
	}

	switch(nDestPlayerSelfSetting&enmSelfSeting_P2PShow_Mask)
	{
		case enmSelfSeting_P2PShow_Vip:
		{
            if(enmVipLevel_RED > nReqVipLevel)
            {
            	nConnectP2PResult = enmConnectP2PResult_OTHER_Need_Vip;
            }
		}
		break;
		case enmSelfSeting_P2PShow_King:
		{
			if(enmVipLevel_Silver_KING > nReqVipLevel)
			{
				nConnectP2PResult = enmConnectP2PResult_OTHER_Need_King;
			}
		}
		break;
		case enmSelfSeting_P2PShow_Close:
		{
				nConnectP2PResult = enmConnectP2PResult_OTHER_Close;
		}
		break;
		default:
			break;
	}
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
