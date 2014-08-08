/*
 *  bll_event_p2p_response.h
 *
 *  To do：
 *  Created on: 	2011-12-19
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_P2P_RESPONSE_H_
#define BLL_EVENT_P2P_RESPONSE_H_


#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CP2PResponseEvent:public CBllBase
{
public:
	CP2PResponseEvent()
	{
	}

	virtual ~CP2PResponseEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventP2PResponse(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t GenerateConnectP2PResult(PlayerState nPlayerState,ConnectP2PResult &nConnectP2PResult,bool isSelf );

	int32_t SendRespToClient(MessageHeadSS *pMsgHead,ConnectP2PResult nConnectP2PResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendNotifyToClient(ResponseP2PType nResponseP2PType,RoomID nRoomID,RoleID nDestRoleID,RoleID nAnswerRoleID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendNotifyToMediaServer(RoomID nRoomID,RoleID nSrcRoleID,RoleID nAnswerRoleID);
};

#define	CREATE_P2PREPONSEEVENT_INSTANCE		CSingleton<CP2PResponseEvent>::CreateInstance
#define	GET_P2PREPONSEEVENT_INSTANCE		CSingleton<CP2PResponseEvent>::GetInstance
#define	DESTROY_P2PREPONSEEVENT_INSTANCE	CSingleton<CP2PResponseEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_P2P_RESPONSE_H_ */
