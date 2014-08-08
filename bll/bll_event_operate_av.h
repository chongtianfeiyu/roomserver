/*
 *  bll_event_operate_av.h
 *
 *  To do：
 *  Created on: 	2011-12-20
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_OPERATE_AV_H_
#define BLL_EVENT_OPERATE_AV_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "dal/to_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class COprateAVReqEvent:public CBllBase
{
public:
	COprateAVReqEvent()
	{
	}

	virtual ~COprateAVReqEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventOperateAVReq(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendNotifyToMediaServer(RoleID nModifyRoleID,RoomID nRoomID,COperateVAReq *pOperateVAReq,int32_t nMediaID);
};

#define	CREATE_OPRATEAVEVENT_INSTANCE		CSingleton<COprateAVReqEvent>::CreateInstance
#define	GET_OPRATEAVEVENT_INSTANCE			CSingleton<COprateAVReqEvent>::GetInstance
#define	DESTROY_OPRATEAVEVENT_INSTANCE		CSingleton<COprateAVReqEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* BLL_EVENT_OPERATE_AV_H_ */
