/*
 *  bll_event_modify_robotconfig.cpp
 *
 *  To do：
 *  Created on: 	2012-5-29
 *  Author: 		luocj
 */

#include "bll_event_modify_robotconfig.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "webagent_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CModifyRobotConfigEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CModifyRobotConfigEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CModifyRobotConfigEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}

	CWebModifyRobotConfigNoti *pWebModifyRobotConfigNoti = dynamic_cast<CWebModifyRobotConfigNoti *>(pMsgBody);
	if(NULL == pWebModifyRobotConfigNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pWebModifyRobotConfigNoti=0x%08x}\n",pWebModifyRobotConfigNoti);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("recv modify robot config from web{nRoomID=%d, nRobotPercent=%d, nSendPercent=%d} ",pWebModifyRobotConfigNoti->nRoomID,
			pWebModifyRobotConfigNoti->nRobotPercent,pWebModifyRobotConfigNoti->nSendPercent);

	//获取房间
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pWebModifyRobotConfigNoti->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_NOTICE_LOG("room is not exit in this server!{nRoomID=%d}",pWebModifyRobotConfigNoti->nRoomID);
		return E_NULLPOINTER;
	}
	//设置房间的机器人比例，以及发送彩条的概率
	pRoom->SetRobotPercent(pWebModifyRobotConfigNoti->nRobotPercent);
	pRoom->SetRobotSendPercent(pWebModifyRobotConfigNoti->nSendPercent);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
