/*
 *  bll_timer_sta_inroomtime.h
 *
 *  To do£º
 *  Created on: 	2012-2-22
 *  Author: 		luocj
 */

#ifndef BLL_TIMER_STA_INROOMTIME_H_
#define BLL_TIMER_STA_INROOMTIME_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CStaticInRoomTimeTimer : public ITimerEvent
{
public:
	CStaticInRoomTimeTimer();
	virtual ~CStaticInRoomTimeTimer();

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);

private:
};

#define	CREATE_STATICINROOMTIME_TIMER_INSTANCE		CSingleton<CStaticInRoomTimeTimer>::CreateInstance
#define	GET_STATICINROOMTIME_TIMER_INSTANCE			CSingleton<CStaticInRoomTimeTimer>::GetInstance
#define	DESTROY_STATICINROOMTIME_TIMER_INSTANCE		CSingleton<CStaticInRoomTimeTimer>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_STA_INROOMTIME_H_ */
