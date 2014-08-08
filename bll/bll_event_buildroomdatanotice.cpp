/*
 * bll_event_buildroomdatanotice.cpp
 *
 *  Created on: 2012-4-25
 *      Author: jimm
 */

#include "bll_event_buildroomdatanotice.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "roomdispatcher_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CBuildRoomDataNoticeMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

int32_t CBuildRoomDataNoticeMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}

int32_t CBuildRoomDataNoticeMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CBuildRoomDataNoti *pBuildRoomDataNotice = dynamic_cast<CBuildRoomDataNoti *>(pMsgBody);
	if(NULL == pBuildRoomDataNotice)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}

	WRITE_DEBUG_LOG("recv BuildRoomDataNotice{RoomID=%d}\n",pMsgHead->nRoomID);

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t ret = g_RoomMgt.CreateRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("create room error{nRoomID=%d, ret=0x%08x}",pMsgHead->nRoomID,ret);
		return S_OK;
	}

	uint32_t nStartPos = pRoom->GetStartPos();
	uint32_t nEndPos = pRoom->GetEndPos();

	uint32_t nRoomDataSize = nEndPos - nStartPos;
	if(nRoomDataSize >= pBuildRoomDataNotice->nRoomDataSize)
	{
		memcpy(((uint8_t *)pRoom) + nStartPos, pBuildRoomDataNotice->arrRoomData, pBuildRoomDataNotice->nRoomDataSize);
	}
	CRebulidSuccessNoti stRebulidSuccessNoti;
	stRebulidSuccessNoti.nRoomID = pRoom->GetRoomID();
	SendMessageNotifyToServer(MSGID_RSTA_REBULIDSUCCESS_NOTIFY, &stRebulidSuccessNoti, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_RoomDispatcher);
	WRITE_NOTICE_LOG("room is rebulid ok!{roomid=%d}\n",pRoom->GetRoomID());
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
