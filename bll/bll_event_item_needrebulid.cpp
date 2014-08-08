/*
 *  bll_event_item_needrebulid.cpp
 *
 *  To do：
 *  Created on: 	2012-5-18
 *  Author: 		luocj
 */

#include "bll_event_item_needrebulid.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "itemserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define MaxRebulidCountPer   200

int32_t CItemRebulidEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CItemRebulidEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("pSession NULL ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CServerNeedRebulidSessionData * pData =(CServerNeedRebulidSessionData *)(pSession->GetSessionData());
    if(pData == NULL)
    {
    	WRITE_ERROR_LOG("pData NULL ret=0x%08x \n",E_NULLPOINTER);
    	return E_NULLPOINTER;
    }
    WRITE_NOTICE_LOG("rebulid:start send enter room motice %d",pData->nEntityType);

    int32_t ret = SendEnterRoomNotice(pData->nEntityType);
    if(ret < 0)
    {
    	CItemRebulidCtl &stItemRebulidCtl = GET_ITEMREBULIDCTL_INSTANCE();
    	stItemRebulidCtl.SetItemRebulidSuccess(pData->nEntityType);
    }
	return S_OK;
}
int32_t CItemRebulidEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	WRITE_NOTICE_LOG("rebulid :recv need rebulid  noti serverType = %d",pMsgHead->nSourceType);
	CItemRebulidCtl &stItemRebulidCtl = GET_ITEMREBULIDCTL_INSTANCE();
	stItemRebulidCtl.SetItemRebulid(pMsgHead->nSourceType);
	//发送房间的创建通知
	if(pMsgHead->nSourceType == enmEntityType_Item)
	{
		ret = SendCreatRoomNotice();
		if(ret < 0)
		{
			stItemRebulidCtl.SetItemRebulidSuccess(pMsgHead->nSourceType);
			return ret;
		}
	}
	//获取所有玩家到item_rebulid_ctl中
	ret = GetNeedRebulidPlayer(pMsgHead->nSourceType);
	if(ret<0)
	{
		stItemRebulidCtl.SetItemRebulidSuccess(pMsgHead->nSourceType);
		return ret;
	}
	//创建定时器，超时则开始更新玩家
	ret = StartTimer(pMsgHead->nSourceType);
	if (ret < 0)
	{
		stItemRebulidCtl.SetItemRebulidSuccess(pMsgHead->nSourceType);
		return ret;
	}
	return S_OK;
}

int32_t CItemRebulidEvent::SendCreatRoomNotice()
{
	int32_t nRoomCount = 0;
	CRoom *arrRoomObject[MaxRoomCountPerServer] = {NULL};
	int32_t ret = g_RoomMgt.GetAllRoom(arrRoomObject, MaxRoomCountPerServer, nRoomCount);
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
		//房间ID不合法的时候不创建
		if(arrRoomObject[i]->GetRoomID() == enmInvalidRoomID)
		{
			continue;
		}
		//发送创建房间通知
		CCreateRoomNotice stCreateRoomNotice;
		stCreateRoomNotice.nRegionType = arrRoomObject[i]->GetRegionType();
		stCreateRoomNotice.nRegionID = arrRoomObject[i]->GetRegionID();
		stCreateRoomNotice.strRegionName = arrRoomObject[i]->GetRegionName();
		stCreateRoomNotice.nChannelType = arrRoomObject[i]->GetChannelType();
		stCreateRoomNotice.nChannelID = arrRoomObject[i]->GetChannelID();
		stCreateRoomNotice.strChannelName = arrRoomObject[i]->GetChannelName();
		stCreateRoomNotice.nRoomType = arrRoomObject[i]->GetRoomType();
		stCreateRoomNotice.nRoomID = arrRoomObject[i]->GetRoomID();
		stCreateRoomNotice.strRoomName = arrRoomObject[i]->GetRoomName();
		stCreateRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
		arrRoomObject[i]->GetMemberList(stCreateRoomNotice.nMemberCount,stCreateRoomNotice.arrRoleID,stCreateRoomNotice.arrRoleRank,MaxAdminCountPerRoom);
		stCreateRoomNotice.nHostID = arrRoomObject[i]->GetHostID();
		stCreateRoomNotice.strHostName = arrRoomObject[i]->GetHostName();
		WRITE_NOTICE_LOG("rebulid itemserver:send rebulid creat room to item [roomid= %d]",arrRoomObject[i]->GetRoomID());
		SendMessageNotifyToServer(MSGID_RSMS_CREATE_ROOM_NOTICE, &stCreateRoomNotice, arrRoomObject[i]->GetRoomID(), enmTransType_Broadcast, enmInvalidRoleID, enmEntityType_Item);
	}
	return S_OK;
}

int32_t CItemRebulidEvent::GetNeedRebulidPlayer(EntityType nEntityType)
{
	int32_t nPlayerCount = 0;
	RoleID arrRoleID[MaxPlayerCountPerRoomServer];
	int32_t ret = g_PlayerMgt.GetAllPlayer(arrRoleID,MaxPlayerCountPerRoomServer,nPlayerCount);
	if(ret < 0)
	{
		return ret;
	}
	if(nPlayerCount <= 0)
	{
		return S_OK;
	}
	CItemRebulidCtl &stItemRebulidCtl = GET_ITEMREBULIDCTL_INSTANCE();
	stItemRebulidCtl.SetRebulidRoleID(arrRoleID,nPlayerCount,nEntityType);
	return S_OK;
}

int32_t CItemRebulidEvent::StartTimer(EntityType nEntityType)
{
	CFrameSession *pSession = NULL;
	int32_t ret = g_Frame.CreateSession(0, enmSessionType_RebulidItemDaly, this, enmRebulidItemDelayPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("rebulid :start timer failed serverType = %d!",nEntityType);
		return ret;
	}
	CServerNeedRebulidSessionData *pData = new(pSession->GetSessionData())CServerNeedRebulidSessionData();
	pData->nEntityType = nEntityType;

	return S_OK;
}

int32_t CItemRebulidEvent::SendEnterRoomNotice(EntityType nEntityType)
{
	int32_t ret = S_OK;
	CItemRebulidCtl &stItemRebulidCtl = GET_ITEMREBULIDCTL_INSTANCE();
	int32_t nPlayerCount = 0;
	RoleID arrRoleID[MaxRebulidCountPer];
	stItemRebulidCtl.GetRebulidRoleID(arrRoleID,MaxRebulidCountPer,nPlayerCount,nEntityType);

	for(int32_t i = 0;i < nPlayerCount;i++)
	{
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(arrRoleID[i], pPlayer, nPlayerIndex);
		//没有此玩家
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("rebulid itemserver:player does no exist{nRoleID=%d}",arrRoleID[i]);
			continue;
		}
		//进入房间通知
		CEnterRoomNotice stEnterRoomNotice;
		stEnterRoomNotice.nRoleID = pPlayer->GetRoleID();
		stEnterRoomNotice.strRoleName = pPlayer->GetRoleName();
		stEnterRoomNotice.nAccountID = pPlayer->GetAccountID();
		stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
		stEnterRoomNotice.nPlayerType = pPlayer->GetPlayerType();
		stEnterRoomNotice.nIdentityType = pPlayer->GetIdentityType();
		if(pPlayer->IsHideEnter())
		{
			stEnterRoomNotice.nEnterRoomType = enmEnterRoomType_Hid;
		}

		int32_t nEnterCount = 0;
		RoomID arrEnterRoomID[MaxEnterRoomCount];
		pPlayer->GetEnterRoomInfo(arrEnterRoomID,nEnterCount,MaxEnterRoomCount);

		for(int32_t j = 0;j < nEnterCount;j++)
		{
			CRoom *pRoom = NULL;
			RoomIndex nRoomIndex = enmInvalidRoomIndex;
			ret = g_RoomMgt.GetRoom(arrEnterRoomID[j], pRoom, nRoomIndex);
			if(ret < 0 || pRoom == NULL)
			{
				WRITE_ERROR_LOG("get room error!ret=0x%08x{roomid=%d}\n",ret,arrEnterRoomID[j]);
				continue;
			}
			stEnterRoomNotice.nRoomID = arrEnterRoomID[j];
			stEnterRoomNotice.nRoleRank = pRoom->GetRoleRank(pPlayer->GetRoleID());
			stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
			WRITE_NOTICE_LOG("rebulid itemserver:send rebulid enter room notice to item {nRoomID=%d, nRoleID=%d}",arrEnterRoomID[j],pPlayer->GetRoleID());
			SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice,arrEnterRoomID[j] , enmTransType_Broadcast, pPlayer->GetRoleID(), nEntityType);
		}
	}
	if(nPlayerCount == MaxRebulidCountPer)
	{
		WRITE_NOTICE_LOG("rebulid :start other timer to rebulid %d!",nEntityType );
		ret = StartTimer(nEntityType);
	}
	else
	{
		WRITE_NOTICE_LOG("rebulid :rebulid %d ok!",nEntityType);
		stItemRebulidCtl.SetItemRebulidSuccess(nEntityType);
	}
	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
