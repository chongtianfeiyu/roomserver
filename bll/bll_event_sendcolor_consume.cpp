/*
 *  bll_event_sendcolor_consume.cpp
 *
 *  To do：
 *  Created on: 	2012-5-29
 *  Author: 		luocj
 */

#include "bll_event_sendcolor_consume.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "itemserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CSendColorEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CSendColorEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CSendColorEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CBigConsumeNoti *pBigConsumeNoti = dynamic_cast<CBigConsumeNoti *>(pMsgBody);
	if(NULL == pBigConsumeNoti)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed[ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("player in room consume is more so robot send color{roleid=%d,roomid=%d}",pBigConsumeNoti->nSrcRoleID,pBigConsumeNoti->nRoomID);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pBigConsumeNoti->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not exit in this server{nRoomID=%d}",pBigConsumeNoti->nRoomID);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pBigConsumeNoti->nSrcRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoomID=%d}",pBigConsumeNoti->nSrcRoleID);
		return ret;
	}
	SendColorToPalyer(pRoom,pPlayer);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
