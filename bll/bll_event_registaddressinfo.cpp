/*
 * bll_event_registaddressinfo.cpp
 *
 *  Created on: 2013-3-5
 *      Author: jimm
 */

#include "bll_event_registaddressinfo.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "mediaserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CRegistAddressInfoMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CRegistAddressInfoMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CRegistAddressInfoMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CRegistAddressInfoNoti *pRegistAddressInfoNoti = dynamic_cast<CRegistAddressInfoNoti *>(pMsgBody);
	if(NULL == pRegistAddressInfoNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pRegistAddressInfoNoti=0x%08x}\n",pRegistAddressInfoNoti);
		return E_NULLPOINTER;
	}

	MediaServerInfo stInfo;
	stInfo.m_nServerID = pMsgHead->nSourceID;
	stInfo.m_nAddress = pRegistAddressInfoNoti->nInternetAddress;
	stInfo.m_nPort = pRegistAddressInfoNoti->nInternetPort;

	g_DataCenter.RegistMediaServerInfo(stInfo);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

