/*
 *  bll_timer_sta_inroomtime.cpp
 *
 *  To do��
 *  Created on: 	2012-2-22
 *  Author: 		luocj
 */

#include "bll_timer_sta_inroomtime.h"
#include "ctl/server_datacenter.h"
#include "bll_base.h"
#include "config/someconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

CStaticInRoomTimeTimer::CStaticInRoomTimeTimer()
{

}

CStaticInRoomTimeTimer::~CStaticInRoomTimeTimer()
{

}

int32_t CStaticInRoomTimeTimer::OnTimerEvent(CFrameTimer *pTimer)
{
	int32_t ret = S_OK;
	uint16_t nCurDay = CDateTime::CurrentDateTime().Day();
	uint16_t nMonth = CDateTime::CurrentDateTime().Month();
	uint16_t nYear = CDateTime::CurrentDateTime().Year();
	uint16_t nHour = (uint16_t)g_SomeConfig.GetStaRunTime();
	CDateTime stRunDateTime (nYear,nMonth,nCurDay,nHour,(uint16_t)0,(uint16_t)0,(uint16_t)0);
	uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
	//ִ��ʱ�����Ҳ�����Сʱ
	if(nCurTime < (stRunDateTime.Seconds()-30*60) || nCurTime > (stRunDateTime.Seconds()+30*60))
	{
		WRITE_DEBUG_LOG("it is not time to static in room time!\n");
		return ret;
	}
	int32_t nRoomCount = 0;
	CRoom *arrRoomObject[MaxRoomCountPerServer] = {NULL};
	ret = g_RoomMgt.GetAllRoom(arrRoomObject, MaxRoomCountPerServer, nRoomCount);
	if(ret < 0)
	{
		//todo
		return ret;
	}

	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		if(arrRoomObject[i] == NULL)
		{
			continue;
		}

		int32_t nPlayerCount = 0;
		CPlayer *arrPlayer[MaxUserCountPerRoom];
		g_DataCenter.GetAllPlayers(arrRoomObject[i], arrPlayer, MaxUserCountPerRoom, nPlayerCount);
		for(int32_t j = 0;j < nPlayerCount;j++)
		{
			//ͳ����micʱ��
			if(arrPlayer[j]->GetStateInRoom(arrRoomObject[i]->GetRoomID()) == enmPlayerState_PublicMic || arrPlayer[j]->GetStateInRoom(arrRoomObject[i]->GetRoomID()) == enmPlayerState_PrivateMic)
			{
				g_BllBase.StaticOnMIcTime(arrPlayer[j],arrRoomObject[i]);
			}

			//ͳ���ڷ���ʱ��
			ret = g_BllBase.StaticInRoomTime(arrPlayer[j],arrRoomObject[i]);
			if(ret<0)
			{
				WRITE_ERROR_LOG(" static player in room time filed{nRoleID=%d, nRoomID=%d}",arrPlayer[j]->GetRoleID(),arrRoomObject[i]->GetRoomID());
				continue;
			}
		}
	}

	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END
