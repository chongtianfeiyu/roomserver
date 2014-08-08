/*
 *  bll_event_updateselfsetting.cpp
 *
 *  To do£º
 *  Created on: 	2012-5-4
 *  Author: 		luocj
 */

#include "bll_event_updateselfsetting.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "hallserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUpdateSelfSettingMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CUpdateSelfSettingMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CUpdateSelfSettingMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CHallSetSelfSettingNoti *pHallSetSelfSettingNoti = dynamic_cast<CHallSetSelfSettingNoti *>(pMsgBody);
	if(NULL == pHallSetSelfSettingNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pHallSetSelfSettingNoti=0x%08x, nRoleID=%d}\n",pHallSetSelfSettingNoti,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pHallSetSelfSettingNoti->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_WARNING_LOG("get player object failed or player is not in this server!{RoleID=%d, ret=0x%08x}\n",pHallSetSelfSettingNoti->nRoleID, ret);
		return E_NULLPOINTER;
	}

	pPlayer->SetPlayerSelfSetting(pHallSetSelfSettingNoti->nPlayerSelfSetting);

	WRITE_NOTICE_LOG("player set self setting{nRoleID=%d, nPlayerSelfSetting=%d}",pHallSetSelfSettingNoti->nRoleID,pHallSetSelfSettingNoti->nPlayerSelfSetting);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
