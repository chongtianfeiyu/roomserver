/*
 *  bll_event_unlock_shutup.h
 *
 *  To do：
 *  Created on: 	2012-1-11
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_UNLOCK_SHUTUP_H_
#define BLL_EVENT_UNLOCK_SHUTUP_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "dal/to_server_message.h"
#include "../dal/to_client_message.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CUnLockShutUpEvent:public CBllBase
{
public:
	CUnLockShutUpEvent()
	{
	}

	virtual ~CUnLockShutUpEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventUnLockShutUp(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t UnLockShutUpPlayer(CRoom *pRoom,RoleRank nSrcRoleRank,RoleID nUnLockShutUpRoleID);
	int32_t SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,UnLockShutUpResult nUnLockShutUpResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
};

#define	CREATE_UNLOCKSHUTUPEVENT_INSTANCE			CSingleton<CUnLockShutUpEvent>::CreateInstance
#define	GET_UNLOCKSHUTUPEVENT_INSTANCE				CSingleton<CUnLockShutUpEvent>::GetInstance
#define	DESTROY_UNLOCKSHUTUPEVENT_INSTANCE			CSingleton<CUnLockShutUpEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_UNLOCK_SHUTUP_H_ */
