/*
 * bll_event_online.cpp
 *
 *  Created on: 2012-1-17
 *      Author: jimm
 */

#include "bll_event_online.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "hallserver_message_define.h"
#include "frame_mem_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t COnlineMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t COnlineMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t COnlineMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	COnlineNoti *pOnlineNoti = dynamic_cast<COnlineNoti *>(pMsgBody);
	if(NULL == pOnlineNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pOnlineNoti=0x%08x}\n",pOnlineNoti);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("online:recv roleid online noti{nRoleID=%d}",pMsgHead->nRoleID);

	CConnInfoMgt::iterator it = g_ConnInfoMgt.find(pOnlineNoti->nRoleID);
	if(it != g_ConnInfoMgt.end())
	{
		FREE((uint8_t *)(it->second));
	}

	ConnInfo *pConnInfo = (ConnInfo *)MALLOC(sizeof(ConnInfo));
	if(pConnInfo == NULL)
	{
		WRITE_ERROR_LOG("null pointer:malloc conn info failed!{RoleID=%d}\n", pOnlineNoti->nRoleID);
		return E_NULLPOINTER;
	}

	uint32_t offset = 0;
	int32_t ret = pConnInfo->MessageDecode((uint8_t *)pOptionData, (uint32_t)nOptionLen, offset);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("online noti:decode connuin failed{nRoleID=%d, ret=0x%08x}\n", pOnlineNoti->nRoleID, ret);
		FREE((uint8_t *)pConnInfo);
		return ret;
	}

	g_ConnInfoMgt.insert(make_pair(pOnlineNoti->nRoleID, pConnInfo));

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

