/*
 * bll_timer_updateinfotohall.cpp
 *
 *  Created on: 2012-1-6
 *      Author: jimm
 */

#include "bll_timer_updateinfotohall.h"
#include "ctl/server_datacenter.h"
#include "bll_base.h"
#include "dal/to_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

CUpdateInfoToHallTimer::CUpdateInfoToHallTimer()
{

}

CUpdateInfoToHallTimer::~CUpdateInfoToHallTimer()
{

}

int32_t CUpdateInfoToHallTimer::OnTimerEvent(CFrameTimer *pTimer)
{
	int32_t ret = S_OK;

	int32_t nRoomCount = 0;
	CRoom *arrRoomObject[MaxRoomCountPerServer] = { NULL };
	ret = g_RoomMgt.GetAllRoom(arrRoomObject, MaxRoomCountPerServer, nRoomCount);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("get all room filed!\n");
		return ret;
	}

	CUpdateInfoToHallNotice stNotice;
	int32_t nRoomCountTemp = 0;
	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		if(arrRoomObject[i] == NULL || arrRoomObject[i]->GetRoomID() == enmInvalidRoomID)
		{
			continue;
		}

		stNotice.arrRoomID[nRoomCountTemp] = arrRoomObject[i]->GetRoomID();
		int32_t nPlayerCount = 0;
		g_DataCenter.GetAllPlayers(arrRoomObject[i], stNotice.arrRoleID[nRoomCountTemp], MaxUserCountPerRoom, nPlayerCount);
		stNotice.arrPlayerCount[nRoomCountTemp] = nPlayerCount;
		nRoomCountTemp++;
	}
	stNotice.nRoomCount = nRoomCountTemp;
	stNotice.nServerAddress = 0;
	stNotice.nServerPort = 0;
	stNotice.nServerNetType = 0;
	stNotice.nRecvPacketsPerTenMins = 0;

	GET_BLLBASE_INSTANCE().SendMessageNotifyToServer(MSGID_RSHS_UPDATEINFOTOHALL_NOTICE, &stNotice, enmInvalidRoomID,
			enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_RoomDispatcher, 0);

	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END

