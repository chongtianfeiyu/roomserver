/*
 * bll_event_logout.cpp
 *
 *  Created on: 2011-12-20
 *      Author: jimm
 */

#include "bll_event_logout.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "roomserver_message_define.h"
#include "dal/to_client_message.h"
#include "dal/to_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CLogoutMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CLogoutMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CLogoutMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CLogoutNoti stLogoutNoti;
	stLogoutNoti.nRoleID = pMsgHead->nRoleID;

	CLogoutReq *pLogoutMsg = dynamic_cast<CLogoutReq *>(pMsgBody);
	if(NULL == pLogoutMsg)
	{
		WRITE_ERROR_LOG("null pointer:{pLogoutMsg=0x%08x}\n",pLogoutMsg);
		SendMessageNotifyToClient(MSGID_RSCL_LOGOUT_NOTI, &stLogoutNoti, pMsgHead->nRoomID, enmBroadcastType_All,
				enmInvalidRoleID, nOptionLen, (char *)pOptionData, "send logout room notify message");

		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("logout:recv roleid logout request{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID, pMsgHead->nRoomID);

	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		//todo : write something
		WRITE_ERROR_LOG("logout:get room error!{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID,ret);
		SendMessageNotifyToClient(MSGID_RSCL_LOGOUT_NOTI, &stLogoutNoti, pMsgHead->nRoomID, enmBroadcastType_All,
				enmInvalidRoleID, nOptionLen, (char *)pOptionData, "send logout room notify message");
		return ret;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("logout:get player object error[nRoleID=%d, ret=0x%08x]\n",pMsgHead->nRoleID,ret);
		SendMessageNotifyToClient(MSGID_RSCL_LOGOUT_NOTI, &stLogoutNoti, pMsgHead->nRoomID, enmBroadcastType_All,
				enmInvalidRoleID, nOptionLen, (char *)pOptionData, "send logout room notify message");
		return ret;
	}
	//更新主播列表状态
	pRoom->m_RoomArtistList.SetOnlineStat(pMsgHead->nRoleID,enmOffline_stat);

	//退房
	ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,pMsgHead->nMessageID,true);

	if(ret < 0)
	{
		WRITE_ERROR_LOG("logout:player exit room filed{nRoleID=%d, nRommID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return ret;
	}



	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END

