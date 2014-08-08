/*
 *  bll_timer_send_color.cpp
 *
 *  To do：
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#include "bll_timer_send_color.h"
#include "ctl/server_datacenter.h"
#include "bll_base.h"
#include "dal/to_client_message.h"
#include "ctl/rebot_ctl.h"
#include "main_frame.h"
#include "def/dal_def.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//static int32_t nEnterCount = 0;
//static int32_t nNextStartTime = 10;

CSendColorTimer::CSendColorTimer()
{
}

CSendColorTimer::~CSendColorTimer()
{

}

int32_t CSendColorTimer::OnTimerEvent(CFrameTimer *pTimer)
{
	int32_t ret = S_OK;
	//获取所有的房间
	int32_t nRoomCount = 0;
	CRoom *arrRoomObject[MaxRoomCountPerServer] = {NULL};
	ret = g_RoomMgt.GetAllRoom(arrRoomObject, MaxRoomCountPerServer, nRoomCount);
	if(ret < 0)
	{
		return ret;
	}

	//检查是否有人禁言到时
	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		if(arrRoomObject[i] == NULL)
		{
			continue;
		}

		RoleID arrRoleID[MaxRoomProhibitSpeakSize] = {enmInvalidRoleID};
		int32_t nDelCount = 0;

		arrRoomObject[i]->DelTimerOutProhibitList(arrRoleID,nDelCount,MaxRoomProhibitSpeakSize);
		for(int32_t j = 0 ;j < nDelCount;j++)
		{
			CPlayer *pPlayer = NULL;
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			ret = g_PlayerMgt.GetPlayer(arrRoleID[j], pPlayer, nPlayerIndex);
			//没有此玩家
			if(ret < 0 || pPlayer == NULL)
			{
				WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",arrRoleID[j]);
				continue;
			}
			if(arrRoomObject[i]->IsPlayerInRoom(nPlayerIndex) || arrRoomObject[i]->IsRebotPlayerInRoom(arrRoleID[j]))
			{
				CUnLockShutUpNoti stUnLockShutUpNoti;
				stUnLockShutUpNoti.nSrcRoleID = enmInvalidRoleID;
				stUnLockShutUpNoti.nDestRoleID = arrRoleID[j];

				//发送给除发起禁言的其他人
				GET_BLLBASE_INSTANCE().SendMessageNotifyToClient(MSGID_RSCL_UNLOCK_SHUT_UP_NOTI, &stUnLockShutUpNoti, arrRoomObject[i]->GetRoomID(), enmBroadcastType_All,
						arrRoleID[j],0,NULL, "send unshut up user  notify");
			}
		}
	}

//	nEnterCount += RobotSendColorTimeOutInterval;
//	if(nEnterCount < nNextStartTime)
//	{
//		return S_OK;
//	}
//	nEnterCount = 0;
//	nNextStartTime = GET_REBOT_CONFIG().GetNextSentTime();

	for(int32_t i = 0; i < nRoomCount; ++i)
	{
		if(arrRoomObject[i] == NULL)
		{
			continue;
		}
		int32_t nPlayerCount = 0;
		RoleID arrRoleID[MaxUserCountPerRoom] = {0};
		//获取房间里的机器人
		if(!(arrRoomObject[i]->IsSendColorOpen()))
		{
			continue;
		}
		arrRoomObject[i]->GetAllRebotPlayers(arrRoleID,MaxUserCountPerRoom,nPlayerCount);
		if(nPlayerCount == 0)
		{
			continue;
		}
		if(arrRoomObject[i]->GetRobotSendPercent() <= 0)
		{
			continue;
		}
		if(time(NULL) <= arrRoomObject[i]->GetRebotNextColorTime())
		{
			continue;
		}
		uint32_t nNextRebotColorTime = GetNextSendColorTime(arrRoomObject[i], nPlayerCount);
		arrRoomObject[i]->SetRebotNextColorTime(nNextRebotColorTime);

		//获得房间的公卖
		RoleID arrPublicMic[MaxPublicMicCount];
		uint16_t nPublicMicCount = 0;
		arrRoomObject[i]->GetOnPublicMic(arrPublicMic,MaxPublicMicCount,nPublicMicCount);

		int32_t nPlayerIndex = Random2(nPlayerCount);
		if(nPlayerIndex < 0)
		{
			nPlayerIndex = -nPlayerIndex;
		}
		if(nPlayerIndex >= nPlayerCount)
		{
			nPlayerIndex = nPlayerCount-1;
		}

		if(arrRoomObject[i]->IsInProhibitList(arrRoleID[nPlayerIndex]))
		{
			continue;
		}
		//构造彩条信息发送彩条
		SendColor(arrRoleID[nPlayerIndex],arrRoomObject[i]->GetRoomID(),arrPublicMic,nPublicMicCount);
	}
	return S_OK;
}

uint32_t CSendColorTimer::GetNextSendColorTime(CRoom* pRoom, int32_t nRebotCount)
{
	if(pRoom == NULL || nRebotCount <= 0)
	{
		return 0;
	}
	if(pRoom->GetRobotSendPercent() <= 0)
	{
		return 0;
	}

	//配置文件中的发彩条间隔时间随机值
	int32_t nConfigRandomTime = GET_REBOT_CONFIG().GetNextSentTime();
	int32_t nSendColorRebotCount = nRebotCount * pRoom->GetRobotSendPercent() /100;
	if(nSendColorRebotCount <= 0)
	{
		nSendColorRebotCount = 1;
	}
	int32_t nRebotSendColorTime = nConfigRandomTime / nSendColorRebotCount;
	if(nRebotSendColorTime <= 0)
	{
		nRebotSendColorTime = 1;
	}
	return time(NULL) + nRebotSendColorTime;
}

int32_t CSendColorTimer::SendColor(const RoleID nRobotRoleID,const RoomID nRoomID,RoleID arrPublicMic[],int32_t nPublicMicCount)
{
	int32_t ret = S_OK;
	CSendTextNoti stSendTextNoti;
	stSendTextNoti.nTextType = enmTextType_Public;
	stSendTextNoti.nSrcID = nRobotRoleID;
	stSendTextNoti.nDestID = 0;
	if(GET_REBOTCTL_INSTANCE().IsSendToPublicMic())
	{
		int32_t nIndex = Random2(nPublicMicCount*1000)/1000;
		if(nIndex < 0)
		{
			nIndex = -nIndex;
		}
		if(nIndex >= nPublicMicCount)
		{
			nIndex = nPublicMicCount-1;
		}
		stSendTextNoti.nDestID = arrPublicMic[nIndex];
	}
    char strTemp[20] = {'\0'};
    sprintf(strTemp,"%d",GET_REBOTCTL_INSTANCE().GetColorType());
	stSendTextNoti.strMessage = strTemp;
	stSendTextNoti.nFontSize = 0;
	stSendTextNoti.nFontColor = 0;
	stSendTextNoti.nFontStyle = enmFontStyleType_COLORTEXT;
	stSendTextNoti.strFontType = "send";
	GET_BLLBASE_INSTANCE().SendMessageNotifyToClient(MSGID_RSCL_SENDTEXT_NOTI, &stSendTextNoti, nRoomID, enmBroadcastType_All,
			nRobotRoleID);
	WRITE_DEBUG_LOG("role in room send color{nRobotRoleID=%d, nRoomID=%d}\n" ,nRobotRoleID,nRoomID);
	return ret;

}
FRAME_ROOMSERVER_NAMESPACE_END
