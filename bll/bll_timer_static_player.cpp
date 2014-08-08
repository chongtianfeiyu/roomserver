/*
 *  bll_timer_static_player.cpp
 *
 *  To do£º
 *  Created on: 	2012-2-21
 *  Author: 		luocj
 */

#include "bll_timer_static_player.h"
#include "ctl/server_datacenter.h"
#include "bll_base.h"
#include "config/someconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

static uint32_t nTimeOutCount = 0;
CStaticPlayerCountTimer::CStaticPlayerCountTimer()
{

}

CStaticPlayerCountTimer::~CStaticPlayerCountTimer()
{

}

int32_t CStaticPlayerCountTimer::OnTimerEvent(CFrameTimer *pTimer)
{
	int32_t ret = S_OK;
	nTimeOutCount += StaticPlayerTimeOutInterval;
	if(nTimeOutCount < g_SomeConfig.GetPlayerCountTimer())
	{
       return S_OK;
	}
	nTimeOutCount = 0;
	int32_t nRoomCount = 0;
	CRoom *arrRoomObject[MaxRoomCountPerServer] = { NULL };
	ret = g_RoomMgt.GetAllRoom(arrRoomObject, MaxRoomCountPerServer, nRoomCount);
	if(ret < 0)
	{
		//todo
	}
	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		int32_t nPlayerCount = 0;
		nPlayerCount = arrRoomObject[i]->GetCurUserCount();
		if(nPlayerCount == 0)
		{
			g_RoomMgt.DestroyRoom(arrRoomObject[i]->GetRoomID());
		}
		WRITE_STATIC_NOTICE_LOG("region %d channel %d room %d has %d player and %d robot interval %d ",arrRoomObject[i]->GetRegionID(),arrRoomObject[i]->GetChannelID(),arrRoomObject[i]->GetRoomID(),nPlayerCount,arrRoomObject[i]->GetRobotCount(),g_SomeConfig.GetPlayerCountTimer());
	}
	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END
