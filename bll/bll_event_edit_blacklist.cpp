/*
 *  bll_event_edit_blacklist.cpp
 *
 *  To do：
 *  Created on: 	2012-6-14
 *  Author: 		luocj
 */

#include "bll_event_edit_blacklist.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "webagent_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CDelBlackListEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CDelBlackListEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CDelBlackListEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CWebRSEditBlackListNotice *pWebRSEditBlackListNotice = dynamic_cast<CWebRSEditBlackListNotice *>(pMsgBody);
	if(NULL == pWebRSEditBlackListNotice)
	{
		WRITE_ERROR_LOG("null pointer:{pWebRSEditBlackListNotice=0x%08x}\n",pWebRSEditBlackListNotice);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("recv modify edit black from web{nRoomID=%d, nRoleID=%d, nEditBlackListType=%d} ",pWebRSEditBlackListNotice->nRoomID,
			pWebRSEditBlackListNotice->nRoleID,pWebRSEditBlackListNotice->nEditBlackListType);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pWebRSEditBlackListNotice->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not exit in this server!{nRoomID=%d}",pWebRSEditBlackListNotice->nRoomID);
		return E_NULLPOINTER;
	}
	//从黑名单中删除这个玩家
	if(pWebRSEditBlackListNotice->nEditBlackListType == enmEditBlackListType_Remove)
	{
		pRoom->DelPlayerFromBlackList(pWebRSEditBlackListNotice->nRoleID);
	}
	else if(pWebRSEditBlackListNotice->nEditBlackListType == enmEditBlackListType_Add)
	{
		//加入黑名单
		ret = pRoom->AddBlackList(pWebRSEditBlackListNotice->nRoleID);
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because  add to blacklist error!{nRoleID=%d, nRoomID=%d}",pWebRSEditBlackListNotice->nRoleID,pRoom->GetRoomID());
			return ret;
		}
		//获取玩家对象
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(pWebRSEditBlackListNotice->nRoleID, pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_NOTICE_LOG("get player filed!!{nRoleID=%d, ret=0x%08x}\n",pWebRSEditBlackListNotice->nRoleID,E_NULLPOINTER);
			return E_NULLPOINTER;
		}
		//判断玩家是否在房间
		if(!pPlayer->IsInRoom(pRoom->GetRoomID()))
		{
			WRITE_NOTICE_LOG("player is not in room!{nRoomID=%d, nRoleID=%d}",pWebRSEditBlackListNotice->nRoomID,pWebRSEditBlackListNotice->nRoleID);
			return S_OK;
		}
		//发送加入黑名单通知
		CAddBlackNoti stAddBlackNoti;
		stAddBlackNoti.nSrcRoleID = pPlayer->GetRoleID();
		stAddBlackNoti.nDestRoleID = pPlayer->GetRoleID();
		SendMessageNotifyToClient(MSGID_RSCL_ADD_BLACK_NOTI, &stAddBlackNoti, pRoom->GetRoomID(), enmBroadcastType_All,
				pPlayer->GetRoleID(),nOptionLen,(char*)pOptionData, "send add user to black notify");
		//玩家退出房间
		ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,pMsgHead->nMessageID);
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because exit room filed!{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			return ret;
		}
	}
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
