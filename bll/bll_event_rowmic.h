/*
 *  bll_event_rowmic.h
 *
 *  To do：
 *  Created on: 	2011-12-26
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_ROWMIC_H_
#define BLL_EVENT_ROWMIC_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"
#include "../dal/to_client_message.h"
#include "ctl/server_datacenter.h"
#include "dal/to_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CRowMicEvent:public CBllBase
{
public:
	CRowMicEvent()
	{
	}

	virtual ~CRowMicEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventRequestShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t PushOnPrivateMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t PushOnPublicMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t PushOnWaittingMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,bool isFreeOpened,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnEventExitShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendNotifyToMediaServer(MessageHeadSS *pMsgHead,CRoom *pRoom);
	int32_t SendRespToClient(MessageHeadSS *pMsgHead,RequestShowResult nRequestShowResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL,uint8_t nIndex=0,uint32_t nTime = 0,uint8_t nFreeShowOpened = 0);
	int32_t SendRequestShowNotify(RequestShowResult nRequestShowResult,RoomID nRoomID,RoleID nRoleID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL,uint8_t nIndex=0,uint32_t nTime = 0);

};

#define	CREATE_ROWMICEVENT_INSTANCE		CSingleton<CRowMicEvent>::CreateInstance
#define	GET_ROWMICEVENT_INSTANCE		CSingleton<CRowMicEvent>::GetInstance
#define	DESTROY_ROWMICEVENT_INSTANCE	CSingleton<CRowMicEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_ROWMIC_H_ */
