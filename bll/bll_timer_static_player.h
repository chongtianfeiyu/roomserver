/*
 *  bll_timer_static_player.h
 *
 *  To do£º
 *  Created on: 	2012-2-21
 *  Author: 		luocj
 */

#ifndef BLL_TIMER_STATIC_PLAYER_H_
#define BLL_TIMER_STATIC_PLAYER_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CStaticPlayerCountTimer : public ITimerEvent
{
public:
	CStaticPlayerCountTimer();
	virtual ~CStaticPlayerCountTimer();

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);

private:
};

#define	CREATE_STATICPLAYERCOUNT_TIMER_INSTANCE		CSingleton<CStaticPlayerCountTimer>::CreateInstance
#define	GET_STATICPLAYERCOUNT_TIMER_INSTANCE			CSingleton<CStaticPlayerCountTimer>::GetInstance
#define	DESTROY_STATICPLAYERCOUNT_TIMER_INSTANCE		CSingleton<CStaticPlayerCountTimer>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_STATIC_PLAYER_H_ */
