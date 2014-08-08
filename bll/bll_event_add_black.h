/*
 *  bll_event_add_black.h
 *
 *  To do：
 *  Created on: 	2012-1-10
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_ADD_BLACK_H_
#define BLL_EVENT_ADD_BLACK_H_

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

class CAddBlockEvent:public CBllBase
{
public:
	CAddBlockEvent()
	{
	}

	virtual ~CAddBlockEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t OnEventAddBlock(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t AddPlayerToBlack(CRoom *pRoom,RoleID nAddBlockRoleID);
	int32_t SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t SendResponseToUser(MessageHeadSS *pMsgHead,AddBlackResult nAddBlackResult,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	class CAddBlackSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		uint16_t m_nOptionLen;
		char m_arrOptionData[MaxOptionDataSize];
		RoleID nDestRoleID;

		CAddBlackSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			m_nOptionLen = 0;
			memset(&m_arrOptionData, 0, sizeof(m_arrOptionData));
			nDestRoleID = enmInvalidRoleID;
		}
	};
};

#define	CREATE_ADDBLOCKEVENT_INSTANCE		CSingleton<CAddBlockEvent>::CreateInstance
#define	GET_ADDBLOCKEVENT_INSTANCE			CSingleton<CAddBlockEvent>::GetInstance
#define	DESTROY_ADDBLOCKEVENT_INSTANCE		CSingleton<CAddBlockEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_ADD_BLACK_H_ */
