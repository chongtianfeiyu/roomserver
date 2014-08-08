/*
 *  bll_timer_send_color.h
 *
 *  To do£º
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#ifndef BLL_TIMER_SEND_COLOR_H_
#define BLL_TIMER_SEND_COLOR_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"
#include "public_typedef.h"
#include "dal/dal_room.h"
#include "global.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CSendColorTimer : public ITimerEvent
{
public:
	CSendColorTimer();
	virtual ~CSendColorTimer();

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);
	int32_t SendColor(const RoleID nRobotRoleID,const RoomID nRoomID,RoleID arrPublicMic[],int32_t nPublicMicCount);
	uint32_t GetNextSendColorTime(CRoom* pRoom, int32_t nRebotCount);

private:
};

#define	CREATE_SENDCOLOR_TIMER_INSTANCE			CSingleton<CSendColorTimer>::CreateInstance
#define	GET_SENDCOLOR_TIMER_INSTANCE			CSingleton<CSendColorTimer>::GetInstance
#define	DESTROY_SENDCOLOR_TIMER_INSTANCE		CSingleton<CSendColorTimer>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_TIMER_SEND_COLOR_H_ */
