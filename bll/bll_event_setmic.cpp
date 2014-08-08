/*
 *  bll_event_setmic.cpp
 *
 *  To do：
 *  Created on: 	2012-5-7
 *  Author: 		luocj
 */

#include "bll_event_setmic.h"
#include "def/server_errordef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CSetMicStatusMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CSetMicStatusMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CSetMicStatusMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CSetMicStatusReq *pCSetMicStatusReq = dynamic_cast<CSetMicStatusReq *>(pMsgBody);
	if(NULL == pCSetMicStatusReq)
	{
		WRITE_ERROR_LOG("null pointer:{pCSetMicStatusReq=0x%08x}\n",pCSetMicStatusReq);
		return E_NULLPOINTER;
	}

	WRITE_NOTICE_LOG("set mic:recv set mic req{nRoleID=%d, nRoomID=%d, nMicIndex=%d, nMicStatus=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pCSetMicStatusReq->nMicIndex,pCSetMicStatusReq->nMicStatus);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("get player object failed{RoleID=%d, ret=0x%08x}\n",pMsgHead->nRoleID, ret);
		SendResponseToUser(pMsgHead,enmSetMicResult_UNKNOWN,nOptionLen,pOptionData);
		return E_NULLPOINTER;
	}

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("room is not exit!{nRoomID=%d}",pMsgHead->nRoomID);
		SendResponseToUser(pMsgHead,enmSetMicResult_UNKNOWN,nOptionLen,pOptionData);
		return E_NULLPOINTER;
	}

	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleId=%d,nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		SendResponseToUser(pMsgHead,enmSetMicResult_UNKNOWN,nOptionLen,pOptionData);
		return E_RS_PLAYRENOTINROOM;
	}

	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//室主以上可以修改这个状态
	if(nSrcRoleRank < enmRoleRank_Host)
	{
		// todo 发送权限不够回应
		WRITE_NOTICE_LOG("set mic:player has no permission{nRoleID=%d, nSrcRoleRank=%d}",pMsgHead->nRoleID,nSrcRoleRank);
		SendResponseToUser(pMsgHead,enmSetMicResult_NO_PERMISSION,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}

	switch(pCSetMicStatusReq->nMicStatus)
	{
		case enmMicStatus_Open:
		{
			pRoom->OpenedMicByIndex(pCSetMicStatusReq->nMicIndex);
		}
		break;
		case enmMicStatus_Close:
		{
			pRoom->LockedMicByIndex(pCSetMicStatusReq->nMicIndex);
           //或者在此麦上的玩家，下他的麦
			RoleID nRoleID = pRoom->GetPlayerOnMicByIndex(pCSetMicStatusReq->nMicIndex);
			if(nRoleID != enmInvalidRoleID)
			{
				CPlayer *pOnMicPlayer = NULL;
				PlayerIndex nOnMicPlayerIndex = enmInvalidPlayerIndex;
				int32_t ret = g_PlayerMgt.GetPlayer(nRoleID, pOnMicPlayer, nOnMicPlayerIndex);
				if(ret < 0 || pOnMicPlayer == NULL)
				{
					WRITE_ERROR_LOG("set mic:get onmic player object failed{nMicIndex=%d, nRoleID=%d, ret=0x%08x}\n",pCSetMicStatusReq->nMicIndex,nRoleID, ret);
					SendResponseToUser(pMsgHead,enmSetMicResult_UNKNOWN,nOptionLen,pOptionData);
					return E_NULLPOINTER;
				}
				ret = ExitShow(pOnMicPlayer,pRoom,false,pMsgHead->nMessageID,0,NULL,enmExitShowType_Other);
				if(ret<0)
				{
					WRITE_ERROR_LOG("set mic:player set mic filed because player exit show filed{nSrcRoleID=%d, nOnMicRoleID=%d}",pMsgHead->nRoleID,nRoleID);
					SendResponseToUser(pMsgHead,enmSetMicResult_UNKNOWN,nOptionLen,pOptionData);
					return ret;
				}
				WRITE_NOTICE_LOG("set mic:player exit show{nMicIndex=%d, nRoleID=%d}",pCSetMicStatusReq->nMicIndex,nRoleID);
			}
		}
		break;
		default:
			break;

	}
	//发送成功回复及通知
	SendResponseToUser(pMsgHead,enmSetMicResult_OK,nOptionLen,pOptionData);

	CSetMicNotify stSetMicNotify;
	stSetMicNotify.nSrcRoleID = pMsgHead->nRoleID;
	stSetMicNotify.nMicStatus = pCSetMicStatusReq->nMicStatus;
	stSetMicNotify.nMicIndex = pCSetMicStatusReq->nMicIndex;
	SendMessageNotifyToClient(MSGID_RSCL_SET_MIC_NOTI, &stSetMicNotify, pMsgHead->nRoomID, enmBroadcastType_ExceptPlayr,
			pMsgHead->nRoleID,nOptionLen,(char*)pOptionData, "send set mic notify");

	WRITE_NOTICE_LOG("set mic:recv set mic ok{nRoleID=%d, nRoomID=%d, nMicIndex=%d, nMicStatus=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID,pCSetMicStatusReq->nMicIndex,pCSetMicStatusReq->nMicStatus);
	return S_OK;
}

/*
 * 发送响应到客户端
 */
int32_t CSetMicStatusMessageEvent::SendResponseToUser(MessageHeadSS *pMsgHead,SetMICResult nSetMICResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CSetMicResp stSetMicResp;
	stSetMicResp.nSetMICResult = nSetMICResult;
	SendMessageResponse(MSGID_RSCL_SET_MIC_RESP, pMsgHead, &stSetMicResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}
FRAME_ROOMSERVER_NAMESPACE_END
