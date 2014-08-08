/*
 *  bll_event_modify_capacity.h
 *
 *  To do：
 *  Created on: 	2012-4-13
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_MODIFY_CAPACITY_H_
#define BLL_EVENT_MODIFY_CAPACITY_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CModifyCapacityMessageEvent : public CBllBase
{
public:
	CModifyCapacityMessageEvent()
	{
	}

	virtual ~CModifyCapacityMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t KickPlayerForModifyCapacity(CRoom *pRoom,int32_t nModifyCapacity);
	int32_t SendNotifyToUser(RoomID nRoomID,RoleID nDestRoleID);
};

#define	CREATE_MODIFYCAPACITYEVENT_INSTANCE	    CSingleton<CModifyCapacityMessageEvent>::CreateInstance
#define	GET_MODIFYCAPACITYEVENT_INSTANCE		CSingleton<CModifyCapacityMessageEvent>::GetInstance
#define	DESTROY_MODIFYCAPACITYEVENT_INSTANCE	CSingleton<CModifyCapacityMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_MODIFY_CAPACITY_H_ */
