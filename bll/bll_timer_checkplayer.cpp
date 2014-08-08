/*
 *  bll_timer_checkplayer.cpp
 *
 *  To do：
 *  Created on: 	2012-5-10
 *  Author: 		luocj
 */

#include "bll_timer_checkplayer.h"
#include "ctl/server_datacenter.h"
#include "bll_base.h"
#include "dal/to_client_message.h"
#include "main_frame.h"
#include "def/dal_def.h"
#include "ctl/program_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

CCheckPlayerTimer::CCheckPlayerTimer()
{
}

CCheckPlayerTimer::~CCheckPlayerTimer()
{

}

int32_t CCheckPlayerTimer::OnTimerEvent(CFrameTimer *pTimer)
{
//	g_RoomSuperVoteMgt.ClearAllVoteInfo();

	int32_t ret = S_OK;
	//获取服务器上的所有玩家
	int32_t nPlayerCount = 0;
	CPlayer *pPlayerObject[MaxPlayerCountPerRoomServer];
	ret = g_PlayerMgt.GetAllPlayer(pPlayerObject,MaxPlayerCountPerRoomServer,nPlayerCount);
	if(ret<0)
	{
		return ret;
	}
	for(int32_t i = 0;i < nPlayerCount;i++)
	{
		if(pPlayerObject[i]==NULL)
		{
			continue;
		}
		if(pPlayerObject[i]->GetPlayerType() == enmPlayerType_Rebot)
		{
			continue;
		}
		int32_t nTimeOutCount = 0;
		RoomID arrTimeOutRoomID[MaxEnterRoomCount];
		pPlayerObject[i]->GetTimeOutRoomID(arrTimeOutRoomID,MaxEnterRoomCount,nTimeOutCount);
		for(int32_t j = 0;j < nTimeOutCount;j++)
		{
			WRITE_NOTICE_LOG("player time out in room!{nRoleID=%d, nRoomID=%d}\n",pPlayerObject[i]->GetRoleID(),arrTimeOutRoomID[j]);
			//执行退房的相关操作
			CRoom *pRoom = NULL;
			RoomIndex nRoomIndex = enmInvalidRoomIndex;
			ret = g_RoomMgt.GetRoom(arrTimeOutRoomID[j],pRoom,nRoomIndex);
			if(pRoom == NULL || ret < 0)
			{
				continue;
			}
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			g_PlayerMgt.GetPlayerIndex(pPlayerObject[i]->GetRoleID(),nPlayerIndex);
			if(nPlayerIndex == enmInvalidPlayerIndex)
			{
				continue;
			}
			g_BllBase.ExitRoom(pPlayerObject[i],nPlayerIndex,pRoom,0x00018005,true,true);
		}
	}
	return S_OK;
}
FRAME_ROOMSERVER_NAMESPACE_END
