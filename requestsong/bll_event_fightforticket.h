/*
 * bll_event_fightforticket.h
 *
 *  Created on: 2013-3-23
 *      Author: jimm
 */

#ifndef BLL_EVENT_FIGHTFORTICKET_H_
#define BLL_EVENT_FIGHTFORTICKET_H_

#include "def/server_namespace.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"
#include "dal/from_server_message.h"
#include "dal/dal_room.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "main_frame.h"
#include "bll/bll_base.h"
#include "ctl/song_mgt.h"
#include "ctl/program_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CFightForTicketMessageEvent : public CBllBase
{
public:
	CFightForTicketMessageEvent()
	{
	}

	virtual ~CFightForTicketMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

protected:
	int32_t FightForTicketFinish(RoomID nRoomID, uint32_t nOrderID,
			SongProgramInfo *pSongProgramInfo, CPlayer *arrPlayer[], int32_t nPlayerCount);
};

#define	CREATE_FIGHTFORTICKETEVENT_INSTANCE			CSingleton<CFightForTicketMessageEvent>::CreateInstance
#define	GET_FIGHTFORTICKETEVENT_INSTANCE			CSingleton<CFightForTicketMessageEvent>::GetInstance
#define	DESTROY_FIGHTFORTICKETEVENT_INSTANCE		CSingleton<CFightForTicketMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_FIGHTFORTICKET_H_ */
