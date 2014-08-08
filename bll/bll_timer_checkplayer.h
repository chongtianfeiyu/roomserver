/*
 *  bll_timer_checkplayer.h
 *
 *  To do£º
 *  Created on: 	2012-5-10
 *  Author: 		luocj
 */

#ifndef BLL_TIMER_CHECKPLAYER_H_
#define BLL_TIMER_CHECKPLAYER_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"
#include "public_typedef.h"
#include "global.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CCheckPlayerTimer : public ITimerEvent
{
public:
	CCheckPlayerTimer();
	virtual ~CCheckPlayerTimer();

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);
private:
};

#define	CREATE_CHECKPLAYER_TIMER_INSTANCE			CSingleton<CCheckPlayerTimer>::CreateInstance
#define	GET_CHECKPLAYER_TIMER_INSTANCE			CSingleton<CCheckPlayerTimer>::GetInstance
#define	DESTROY_CHECKPLAYER_TIMER_INSTANCE		CSingleton<CCheckPlayerTimer>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_CHECKPLAYER_H_ */
