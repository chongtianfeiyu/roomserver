/*
 *  bll_event_pushon_show.h
 *
 *  To do：
 *  Created on: 	2012-2-7
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_PUSHON_SHOW_H_
#define BLL_EVENT_PUSHON_SHOW_H_

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

class CPushOnShowEvent:public CBllBase
{
public:
	CPushOnShowEvent()
	{
	}

	virtual ~CPushOnShowEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventPushOnShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendNotifyToUser(MessageHeadSS *pMsgHead,RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,uint8_t nIndex,NotifyType nNotifyType,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,PushResult nPushResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendNotifyToMediaServer(MessageHeadSS *pMsgHead,RoleID nPushRoleID,int32_t nDestTelMediaID,int32_t nDestCncMediaID);
	int32_t PushPlayerOnShow(MessageHeadSS *pMsgHead,CRoom *pRoom,RoleID nSrcRoleID,CPlayer* pDestPlayer, int32_t nMicIndex);
	int32_t OnEventAnswerPushOnShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
};

#define	CREATE_PUSHONSHOWEVENT_INSTANCE			CSingleton<CPushOnShowEvent>::CreateInstance
#define	GET_PUSHONSHOWEVENT_INSTANCE			CSingleton<CPushOnShowEvent>::GetInstance
#define	DESTROY_PUSHONSHOWEVENT_INSTANCE		CSingleton<CPushOnShowEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_PUSHON_SHOW_H_ */
