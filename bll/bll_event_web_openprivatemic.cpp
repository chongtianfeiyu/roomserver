/*
 *  bll_event_web_openprivatemic.cpp
 *
 *  To do：
 *  Created on: 	2012-7-5
 *  Author: 		luocj
 */

#include "bll_event_web_openprivatemic.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "webagent_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CWebOpenPrivateMicMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CWebOpenPrivateMicMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CWebOpenPrivateMicMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CWebEditPrivateMic *pWebEditPrivateMic = dynamic_cast<CWebEditPrivateMic *>(pMsgBody);
	if(NULL == pWebEditPrivateMic)
	{
		WRITE_ERROR_LOG("null pointer:{pWebEditPrivateMic=0x%08x}\n",pWebEditPrivateMic);
		return E_NULLPOINTER;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.GetRoom(pWebEditPrivateMic->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not in this server!{nRoomID=%d} \n",pWebEditPrivateMic->nRoomID);
		return ret;
	}

	if(pWebEditPrivateMic->nEditPrivateType == enmEditPrivateType_Open)
	{
		pRoom->SetRoomHasOprivateMic();
	}
	else if(pWebEditPrivateMic->nEditPrivateType == enmEditPrivateType_Close)
	{
		pRoom->SetRoomHasNotOprivateMic();
	}
	//通知玩家房间设置有变
	CSetRoomNotify stSetRoomNotify;
	stSetRoomNotify.nSetCount = 0;
	stSetRoomNotify.nStringCount = 0;
	stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_OPTION;
	stSetRoomNotify.nSetCount++;
	stSetRoomNotify.nRoomOptionType = pRoom->GetRoomOption();

	SendMessageNotifyToClient(MSGID_RSCL_SET_ROOM_NOTI, &stSetRoomNotify, pWebEditPrivateMic->nRoomID, enmBroadcastType_All,
					enmInvalidRoleID);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
