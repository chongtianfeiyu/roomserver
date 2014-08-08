/*
 * bll_timer_getmediaserverinfo.h
 *
 *  Created on: 2013-4-1
 *      Author: gaox
 */

#ifndef BLL_TIMER_GETMEDIASERVERINFO_H_
#define BLL_TIMER_GETMEDIASERVERINFO_H_

#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CGetMediaServerInfo : public ITimerEvent
{
public:
	CGetMediaServerInfo()
	{

	}
	virtual ~CGetMediaServerInfo()
	{

	}

	//¶¨Ê±Æ÷º¯Êý
	int32_t OnTimerEvent(CFrameTimer *pTimer);

private:
};

#define	CREATE_GETMEDIASERVERINFO_TIMER_INSTANCE		CSingleton<CGetMediaServerInfo>::CreateInstance
#define	GET_GETMEDIASERVERINFO_TIMER_INSTANCE			CSingleton<CGetMediaServerInfo>::GetInstance
#define	DESTROY_GETMEDIASERVERINFO_TIMER_INSTANCE		CSingleton<CGetMediaServerInfo>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_GETMEDIASERVERINFO_H_ */
