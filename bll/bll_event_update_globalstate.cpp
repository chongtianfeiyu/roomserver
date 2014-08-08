/*
 *  bll_event_update_globalstate.cpp
 *
 *  To do£º
 *  Created on: 	2012-5-8
 *  Author: 		luocj
 */

#include "bll_event_update_globalstate.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "hallserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUpdateStateMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CUpdateStateMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CUpdateStateMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CHallUpdateGlobalStateNoti *pHallUpdateGlobalStateNoti = dynamic_cast<CHallUpdateGlobalStateNoti *>(pMsgBody);
	if(NULL == pHallUpdateGlobalStateNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pHallUpdateGlobalStateNoti=0x%08x, nRoleID=%d}\n",pHallUpdateGlobalStateNoti,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pHallUpdateGlobalStateNoti->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_WARNING_LOG("get player object failed or player is not in this server!{RoleID=%d, ret=0x%08x}\n",pHallUpdateGlobalStateNoti->nRoleID, ret);
		return E_NULLPOINTER;
	}

	pPlayer->SetPlayerGlobalState(pHallUpdateGlobalStateNoti->nPlayerState);

	WRITE_NOTICE_LOG("palyer set global state{nRoleID=%d, nPlayerState=%d}\n",pHallUpdateGlobalStateNoti->nRoleID,pHallUpdateGlobalStateNoti->nPlayerState);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
