/*
 *  bll_event_kickuser.h
 *
 *  To do：
 *  Created on: 	2012-1-10
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_KICKUSER_H_
#define BLL_EVENT_KICKUSER_H_

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

class CKickUserEvent:public CBllBase
{
public:
	CKickUserEvent()
	{
	}

	virtual ~CKickUserEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventKickUser(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t	KickUser(CRoom *pRoom,RoleRank nSrcRoleRank,VipLevel nSrcVipLevel,RoleID nKickRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,KickResult &nKickResult);
    int32_t SendResponseToClient(MessageHeadSS *pMsgHead,KickResult nKickResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
    int32_t SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,KickType nKickType,RoleID nDestRoleID,CString<MaxTextMessageSize> strReason,uint16_t nTime,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t	SendNotifyToServer(CRoom *pRoom,RoomID nRoomID,RoleID nRoleID,CPlayer *pPlayer);
	int32_t KickVisitor(CRoom *pRoom);

};

#define	CREATE_KICKUSEREVENT_INSTANCE		CSingleton<CKickUserEvent>::CreateInstance
#define	GET_KICKUSEREVENT_INSTANCE			CSingleton<CKickUserEvent>::GetInstance
#define	DESTROY_KICKUSEREVENT_INSTANCE		CSingleton<CKickUserEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_KICKUSER_H_ */
