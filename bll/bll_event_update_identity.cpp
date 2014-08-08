/*
 *  bll_event_update_identity.cpp
 *
 *  To do£º
 *  Created on: 	2012-9-19
 *  Author: 		luocj
 */

#include "bll_event_update_identity.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "itemserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CUpdateIdentityMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CUpdateIdentityMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CUpdateIdentityMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CUpdateIdentityNoti *pUpdateIdentityNoti = dynamic_cast<CUpdateIdentityNoti *>(pMsgBody);
	if(NULL == pUpdateIdentityNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pUpdateIdentityNoti=0x%08x, nRoleID=%d}\n",pUpdateIdentityNoti,pMsgHead->nRoleID);
		return E_NULLPOINTER;
	}

	for(int32_t i = 0;i<pUpdateIdentityNoti->nCount;i++)
	{
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		int32_t ret = g_PlayerMgt.GetPlayer(pUpdateIdentityNoti->nRoleID[i], pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_WARNING_LOG("get player object failed or player is not in this server!{RoleID=%d, ret=0x%08x}\n",pUpdateIdentityNoti->nRoleID[i], ret);
			continue;
		}

		pPlayer->SetIdentityType(pUpdateIdentityNoti->nIdentityType[i]);
	}
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
