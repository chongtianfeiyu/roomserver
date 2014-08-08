/*
 *  bll_event_getwaitting.h
 *
 *  To do：
 *  Created on: 	2012-2-9
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_GETWAITTING_H_
#define BLL_EVENT_GETWAITTING_H_


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

class CGetWaittingEvent:public CBllBase
{
public:
	CGetWaittingEvent()
	{
	}

	virtual ~CGetWaittingEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventGetWaitting(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,CRoom *pRoom,const uint16_t nOptionLen , const void *pOptionData);
	int32_t SrvPrivateShow(CRoom *pRoom, CPlayer *pPlayer,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendProgramInfo(MessageHeadSS * pMsgHead, CRoom *pRoom);
};

#define	CREATE_GETWAITTINGEVENT_INSTANCE		CSingleton<CGetWaittingEvent>::CreateInstance
#define	GET_GETWAITTINGEVENT_INSTANCE			CSingleton<CGetWaittingEvent>::GetInstance
#define	DESTROY_GETWAITTINGEVENT_INSTANCE		CSingleton<CGetWaittingEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_GETWAITTING_H_ */
