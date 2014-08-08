/*
 *  bll_event_set_room.h
 *
 *  To do：
 *  Created on: 	2012-2-7
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_SET_ROOM_H_
#define BLL_EVENT_SET_ROOM_H_

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

class CSetRoomInfoEvent:public CBllBase
{
public:
	CSetRoomInfoEvent()
	{
	}

	virtual ~CSetRoomInfoEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventSetRoomInfo(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
//	int32_t SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
//	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,PullResult nPullResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
//	int32_t SendNotifyToMediaServer(MessageHeadSS *pMsgHead);

	 int32_t SendReqToDB(MessageHeadSS *pMsgHead, CSetRoomReq *pSetRoomReq,const uint16_t nOptionLen , const void *pOptionData ,CPlayer *pPlayer,CRoom *pRoom);
	//保存请求数据到会话中
	class CSetRoomSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		CSetRoomReq m_pMsgBody;

		CSetRoomSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};
};

#define	CREATE_SETROOMINFOEVENT_INSTANCE			CSingleton<CSetRoomInfoEvent>::CreateInstance
#define	GET_SETROOMINFOEVENT_INSTANCE				CSingleton<CSetRoomInfoEvent>::GetInstance
#define	DESTROY_SETROOMINFOEVENT_INSTANCE			CSingleton<CSetRoomInfoEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_SET_ROOM_H_ */
