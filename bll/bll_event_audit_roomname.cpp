/*
 *  bll_event_audit_roomname.cpp
 *
 *  To do：
 *  Created on: 	2012-4-13
 *  Author: 		luocj
 */

#include "bll_event_audit_roomname.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "public_message_define.h"
#include "dal/to_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CAuditRoomNameMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CAuditRoomNameMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CAuditRoomNameMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CAuditRoomNameNotify *pAuditRoomNameNotify = dynamic_cast<CAuditRoomNameNotify *>(pMsgBody);
	if(NULL == pAuditRoomNameNotify)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{pAuditRoomNameNotify=0x%08x}\n",pAuditRoomNameNotify);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("recv room name is audited by web {nRoomID=%d}",pAuditRoomNameNotify->nRoomID);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pAuditRoomNameNotify->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not exit!{nRoomID=%d}",pAuditRoomNameNotify->nRoomID);
		return E_NULLPOINTER;
	}
	//设置房间名字
	pRoom->SetRoomName(pAuditRoomNameNotify->strRoomName.GetString());

	//通知客户端房间名字修改
	CSetRoomNotify stSetRoomNotify;
	stSetRoomNotify.nSrcRoleID = pMsgHead->nRoleID;
	stSetRoomNotify.nSetCount = 0;
	stSetRoomNotify.nStringCount = 0;
	stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_NAME;
	stSetRoomNotify.arrSetString[stSetRoomNotify.nStringCount] = pAuditRoomNameNotify->strRoomName.GetString();
	stSetRoomNotify.nStringCount++;
	stSetRoomNotify.nSetCount++;
	SendMessageNotifyToClient(MSGID_RSCL_SET_ROOM_NOTI, &stSetRoomNotify, pAuditRoomNameNotify->nRoomID, enmBroadcastType_All,
			pMsgHead->nRoleID,0,NULL,"send audit room name notify ");

	WRITE_NOTICE_LOG("room is audited!{nRoomID=%d, strRoomName=%s}\n",pAuditRoomNameNotify->nRoomID, pAuditRoomNameNotify->strRoomName.GetString());
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
