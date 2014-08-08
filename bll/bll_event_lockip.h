/*
 *  bll_event_lockip.h
 *
 *  To do：
 *  Created on: 	2012-1-12
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_LOCKIP_H_
#define BLL_EVENT_LOCKIP_H_

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

class CLockIPEvent:public CBllBase
{
public:
	CLockIPEvent()
	{
	}

	virtual ~CLockIPEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventLockIP(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t LockPlayerIP(CRoom *pRoom,const uint32_t nEndLockTime,RoleRank nSrcRoleRank,RoleID nLockIPRoleID,LockIpResult &nLockIpResult);
	int32_t SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,LockIpResult nLockIpResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
};

#define	CREATE_LOCKIPEVENT_INSTANCE			CSingleton<CLockIPEvent>::CreateInstance
#define	GET_LOCKIPEVENT_INSTANCE			CSingleton<CLockIPEvent>::GetInstance
#define	DESTROY_LOCKIPEVENT_INSTANCE		CSingleton<CLockIPEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_LOCKIP_H_ */
