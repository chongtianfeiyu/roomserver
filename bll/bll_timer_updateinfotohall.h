/*
 * bll_timer_updateinfotohall.h
 *
 *  Created on: 2012-1-6
 *      Author: jimm
 */

#ifndef BLL_TIMER_UPDATEINFOTOHALL_H_
#define BLL_TIMER_UPDATEINFOTOHALL_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CUpdateInfoToHallTimer : public ITimerEvent
{
public:
	CUpdateInfoToHallTimer();
	virtual ~CUpdateInfoToHallTimer();

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);

private:
};

#define	CREATE_UPDATEINFOTOHALL_TIMER_INSTANCE		CSingleton<CUpdateInfoToHallTimer>::CreateInstance
#define	GET_UPDATEINFOTOHALL_TIMER_INSTANCE			CSingleton<CUpdateInfoToHallTimer>::GetInstance
#define	DESTROY_UPDATEINFOTOHALL_TIMER_INSTANCE		CSingleton<CUpdateInfoToHallTimer>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_UPDATEINFOTOHALL_H_ */
