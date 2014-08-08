/*
 *  bll_event_p2p_req.h
 *
 *  To do：
 *  Created on: 	2011-12-19
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_P2P_REQ_H_
#define BLL_EVENT_P2P_REQ_H_


#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "dal/from_client_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CP2PReqEvent:public CBllBase
{
public:
	CP2PReqEvent()
	{
	}

	virtual ~CP2PReqEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventP2PReq(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t GenerateConnectP2PResult(PlayerState nPlayerState,ConnectP2PResult &nConnectP2PResult,bool isSelf );

	int32_t SendRespToClient(MessageHeadSS *pMsgHead,ConnectP2PResult nConnectP2PResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t SendNotifyToClient(RoomID nRoomID,RoleID nDestRoleID,RoleID nReqRoleID, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t CheckVipLevel(VipLevel nReqVipLevel,PlayerSelfSetting nDestPlayerSelfSetting,ConnectP2PResult &nConnectP2PResult);
};

#define	CREATE_P2PREQEVENT_INSTANCE		CSingleton<CP2PReqEvent>::CreateInstance
#define	GET_P2PREQEVENT_INSTANCE		CSingleton<CP2PReqEvent>::GetInstance
#define	DESTROY_P2PREQEVENT_INSTANCE	CSingleton<CP2PReqEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_P2P_REQ_H_ */
