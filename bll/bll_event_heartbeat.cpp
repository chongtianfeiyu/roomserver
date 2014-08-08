/*
 *  bll_event_heartbeat.cpp
 *
 *  To do：
 *  Created on: 	2012-5-10
 *  Author: 		luocj
 */

#include "bll_event_heartbeat.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CHeartBeatEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CHeartBeatEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CHeartBeatEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("pMsgBody null or pMsgHead null ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CPlayerHeartBeat *pPlayerHeartBeat = dynamic_cast<CPlayerHeartBeat *>(pMsgBody);
	if(pPlayerHeartBeat==NULL)
	{
		WRITE_ERROR_LOG("pPlayerHeartBeat null ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("recv roleID heart beat{nRoleID=%d}\n",pPlayerHeartBeat->nRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		//没有次玩家
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d, ret=0x%08x}",pMsgHead->nRoleID,ret);
		return ret;
	}
	for(int32_t i = 0;i < pPlayerHeartBeat->nRoomCount;i++)
	{
        WRITE_DEBUG_LOG("recv player in room heart beat {nRoleID=%d, nRoomID=%d}",pPlayerHeartBeat->nRoleID,pPlayerHeartBeat->arrRoomID[i]);
        pPlayer->RecvHeartBeatInRoom(pPlayerHeartBeat->arrRoomID[i]);
	}
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
