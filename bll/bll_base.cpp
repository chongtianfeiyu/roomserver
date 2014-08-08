/*
 * bll_base.cpp
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#include "bll_base.h"
#include "public_typedef.h"
#include "ctl/server_datacenter.h"
#include "common/common_codeengine.h"
#include "ctl/rebot_ctl.h"
#include <string.h>
#include "dal/from_server_message.h"
#include "hallserver_message_define.h"
#include "public_message_define.h"
#include "sysinfoserver_message_define.h"
#include "dal/dal_updatedatainfotable.h"
#include "requestsong/bll_timer_clearroomorderinfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define MaxPlayerCountUpdate   1000
#define MaxRoomCountUpdate     100

#define MAX_SEND_COUNT 5

//static ConnUin g_arrConnUin[MaxUserCountPerRoom];

int32_t CBllBase::SendMessage(MessageHeadSS* pMsgHead, IMsgBody *pMsgBody, const int32_t nOptionLen,
		const char *pOptionData, int32_t index, int32_t zoneid)
{
	if(pMsgHead == NULL || pMsgBody == NULL)
	{
		return E_NULLPOINTER;
	}

	g_Frame.PostMessage(pMsgHead, pMsgBody);

	UpdateRoomDataInfo();

	UpdatePlayerDataInfo();

	return S_OK;
}

int32_t CBllBase::UpdateRoomDataInfo()
{
	while(GET_UPDATEDATACTL_INSTANCE().IsHasRoomNeedUpdate())
	{
		int32_t nNeedUpdateRoomCount = 0;
		RoomID arrRoomID[MaxRoomCountUpdate];
		GET_UPDATEDATACTL_INSTANCE().GetUpdateRoomID(arrRoomID,MaxRoomCountUpdate,nNeedUpdateRoomCount);

		for(int32_t i = 0;i < nNeedUpdateRoomCount;i++)
		{
			//获取房间对象
			CRoom *pRoom = NULL;
			RoomIndex nRoomIndex = enmInvalidRoomIndex;
			int32_t ret = g_RoomMgt.GetRoom(arrRoomID[i], pRoom, nRoomIndex);
			if(ret < 0 || pRoom == NULL)
			{
				continue;
			}

			if(!pRoom->IsHaveUpdateData())
			{
				continue;
			}

			uint8_t *arrUpdateData[UpdateDataInfoTableSize];
			UpdateDataInfoTable arrUpdateTable[UpdateDataInfoTableSize];

			int32_t nUpdateCount = pRoom->GetUpdateDataInfoTable(arrUpdateData, arrUpdateTable);
			MessageHeadSS stUpdateMsgHead;
			stUpdateMsgHead.nMessageID = MSGID_RSRD_UPDATEDATA_NOTI;
			stUpdateMsgHead.nMessageType = enmMessageType_Notify;
			stUpdateMsgHead.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
			stUpdateMsgHead.nDestType = enmEntityType_RoomDispatcher;
			stUpdateMsgHead.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
			stUpdateMsgHead.nDestID = enmInvalidServerID;
			stUpdateMsgHead.nRoleID = enmInvalidRoleID;
			stUpdateMsgHead.nSequence = 0;
			stUpdateMsgHead.nTransType = enmTransType_Broadcast;
			stUpdateMsgHead.nSessionIndex = enmInvalidSessionIndex;
			stUpdateMsgHead.nRoomID = arrRoomID[i];

			CUpdateDataNoti stNoti;
			stNoti.nType = enmUpdateDataType_Room;
			stNoti.nValue = (int32_t)(arrRoomID[i]);
			stNoti.nStartPos = pRoom->GetStartPos();
			stNoti.nEndPos = pRoom->GetEndPos();
			for(int32_t j = 0; j < nUpdateCount; ++j)
			{
				if(arrUpdateData[j] == NULL)
				{
					continue;
				}
				int32_t nOffset = arrUpdateTable[j].nOffset;
				int32_t nDataSize = arrUpdateTable[j].nDataSize;
				while(nDataSize > MaxUpdateDataSize)
				{
					stNoti.nDataOffset = nOffset;
					stNoti.nDataSize = MaxUpdateDataSize;
					memcpy(stNoti.arrData, arrUpdateData[j], MaxUpdateDataSize);
					DumpMessage("update data noti : ",&stUpdateMsgHead, &stNoti);
					g_Frame.PostMessage(&stUpdateMsgHead, &stNoti);
					nOffset += MaxUpdateDataSize;
					nDataSize -= MaxUpdateDataSize;
					arrUpdateData[j] = arrUpdateData[j] + MaxUpdateDataSize;
					WRITE_DEBUG_LOG("update room data is too big");
				}
				stNoti.nDataOffset = nOffset;
				stNoti.nDataSize = nDataSize;
				memcpy(stNoti.arrData, arrUpdateData[j], nDataSize);
//				DumpMessage("update data noti : ",&stUpdateMsgHead, &stNoti);
				g_Frame.PostMessage(&stUpdateMsgHead, &stNoti);
			}
		}
	}
	return S_OK;
}

int32_t CBllBase::UpdatePlayerDataInfo()
{
	while(GET_UPDATEDATACTL_INSTANCE().IsHasPlayerNeedUpdate())
	{
		int32_t nNeedUpdatePlayerCount = 0;
		RoleID arrRoleID[MaxPlayerCountUpdate];
		GET_UPDATEDATACTL_INSTANCE().GetUpdateRoleID(arrRoleID,MaxPlayerCountUpdate,nNeedUpdatePlayerCount);

		for(int32_t i = 0;i < nNeedUpdatePlayerCount;i++)
		{
			//获取玩家对象
			CPlayer *pPlayer = NULL;
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			int32_t ret = g_PlayerMgt.GetPlayer(arrRoleID[i], pPlayer, nPlayerIndex);
			if(ret < 0 || pPlayer == NULL)
			{
				continue;
			}

			if(!pPlayer->IsHaveUpdateData())
			{
				continue;
			}

			uint8_t *arrUpdateData[UpdateDataInfoTableSize];
			UpdateDataInfoTable arrUpdateTable[UpdateDataInfoTableSize];

			int32_t nUpdateCount = pPlayer->GetUpdateDataInfoTable(arrUpdateData, arrUpdateTable);
			MessageHeadSS stUpdateMsgHead;
			stUpdateMsgHead.nMessageID = MSGID_RSRD_UPDATEDATA_NOTI;
			stUpdateMsgHead.nMessageType = enmMessageType_Notify;
			stUpdateMsgHead.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
			stUpdateMsgHead.nDestType = enmEntityType_RoomDispatcher;
			stUpdateMsgHead.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
			stUpdateMsgHead.nDestID = arrRoleID[i];
			stUpdateMsgHead.nRoleID = arrRoleID[i];
			stUpdateMsgHead.nSequence = 0;
			stUpdateMsgHead.nTransType = enmTransType_ByKey;
			stUpdateMsgHead.nSessionIndex = enmInvalidSessionIndex;
			stUpdateMsgHead.nRoomID = enmInvalidRoomID;

			CUpdateDataNoti stNoti;
			stNoti.nType = enmUpdateDataType_Player;
			stNoti.nValue = arrRoleID[i];
			stNoti.nStartPos = pPlayer->GetStartPos();
			stNoti.nEndPos = pPlayer->GetEndPos();
			for(int32_t j = 0; j < nUpdateCount; ++j)
			{
				if(arrUpdateData[j] == NULL)
				{
					continue;
				}
				int32_t nOffset = arrUpdateTable[j].nOffset;
				int32_t nDataSize = arrUpdateTable[j].nDataSize;
				while(nDataSize > MaxUpdateDataSize)
				{
					stNoti.nDataOffset = nOffset;
					stNoti.nDataSize = MaxUpdateDataSize;
					memcpy(stNoti.arrData, arrUpdateData[j], MaxUpdateDataSize);
					DumpMessage("update data noti : ",&stUpdateMsgHead, &stNoti);
					g_Frame.PostMessage(&stUpdateMsgHead, &stNoti);
					nOffset += MaxUpdateDataSize;
					nDataSize -= MaxUpdateDataSize;
					arrUpdateData[j] = arrUpdateData[j] + MaxUpdateDataSize;
					WRITE_DEBUG_LOG("update player data is too big");
				}
				stNoti.nDataOffset = nOffset;
				stNoti.nDataSize = nDataSize;
				memcpy(stNoti.arrData, arrUpdateData[j], nDataSize);
//				DumpMessage("update data noti : ",&stUpdateMsgHead, &stNoti);
				g_Frame.PostMessage(&stUpdateMsgHead, &stNoti);
			}
		}
	}
	return S_OK;
}
/*
 发送请求消息
 */
void CBllBase::SendMessageRequest(uint32_t nMsgID, IMsgBody* pMsgBody, const RoomID nRoomID, TransType nTransType,
		RoleID nRoleID, EntityType nDestType, const int32_t nDestID, const SessionIndex nSessionIndex,
		const uint16_t nOptionLen, const char *pOptionData, const char *szDumpContent)
{
	if(pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x}\n",pMsgBody);
		return ;
	}
	MessageHeadSS stSendMessage;
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Request;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = nDestID;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = nSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = 0;
	stSendMessage.nRouterIndex = 0;

	DumpMessage(szDumpContent, &stSendMessage, pMsgBody, nOptionLen, pOptionData);
	SendMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
}
/*
 发送相应消息
 */
void CBllBase::SendMessageResponse(uint32_t nMsgID,MessageHeadSS * pMsgHead,IMsgBody* pMsgBody,
		TransType nTransType, const uint16_t nOptionLen,
		const char *pOptionData,const char *szDumpContent)
{
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return;
	}

	MessageHeadSS stSendMessage;

	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Response;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = pMsgHead->nSourceType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = pMsgHead->nSourceID;
	stSendMessage.nRoleID = pMsgHead->nRoleID;
	stSendMessage.nSequence = pMsgHead->nSequence;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = pMsgHead->nSessionIndex;
	stSendMessage.nRoomID = pMsgHead->nRoomID;
	stSendMessage.nZoneID = pMsgHead->nZoneID;
	if(stSendMessage.nDestType == enmEntityType_Tunnel)
	{
		if(nOptionLen > 0)
		{
			ConnUin stInfo;
			uint32_t offset = 0;
			int32_t ret = stInfo.MessageDecode((uint8_t *)pOptionData, (uint32_t)nOptionLen, offset);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("send message response:decode connuin failed!{ret=0x%08x, nRoleID=%d}\n", ret, pMsgHead->nRoleID);
				return;
			}
			stSendMessage.nRouterIndex = g_Frame.GetRouterIndex(stInfo.nRouterAddress, stInfo.nRouterPort);
		}
		else
		{
			CPlayer *pPlayer = NULL;
			PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
			int32_t ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer ,nPlayerIndex);
			if(ret < 0)
			{
				WRITE_ERROR_LOG("get expect player object error!{RoomID=%d, ret=0x%08x}\n", pMsgHead->nRoomID, ret);
				return;
			}
			stSendMessage.nRouterIndex = g_Frame.GetRouterIndex(pPlayer->GetConnInfo().nRouterAddress, pPlayer->GetConnInfo().nRouterPort);
		}
	}
	else
	{
		stSendMessage.nRouterIndex = pMsgHead->nRouterIndex;
	}

	DumpMessage(szDumpContent,&stSendMessage, pMsgBody, nOptionLen, pOptionData);
	SendMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);

}

void CBllBase::SendMessageNotifyToServer(uint32_t nMsgID,IMsgBody* pMsgBody,const RoomID nRoomID,const TransType nTransType,
		const RoleID nRoleID,const EntityType nDestType,const int32_t nDestID,
		const uint16_t nOptionLen, const char *pOptionData,const char *szDumpContent)
{
	if(pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x}\n",pMsgBody);
		return;
	}
	MessageHeadSS stSendMessage;
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = nDestType;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	stSendMessage.nDestID = nDestID;
	stSendMessage.nRoleID = nRoleID;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = nTransType;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nZoneID = g_Frame.GetZoneID(enmRouterType_Sync);
	stSendMessage.nRouterIndex = 0;

	DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nOptionLen, pOptionData);
	SendMessage(&stSendMessage, pMsgBody, nOptionLen, pOptionData);
}

//发送广播消息给房间内所有玩家
void CBllBase::SendMessageNotifyToClient(uint32_t nMsgID, IMsgBody* pMsgBody, RoomID nRoomID, BroadcastType nType,
		RoleID nRoleID, const uint16_t nOptionLen,const char *pOptionData, const char *szDumpContent)
{
	if(pMsgBody == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x}\n",pMsgBody);
		return;
	}

	MessageHeadSS stSendMessage;
	memset(&stSendMessage, 0, sizeof(stSendMessage));
	stSendMessage.nMessageID = nMsgID;
	stSendMessage.nMessageType = enmMessageType_Notify;
	stSendMessage.nSourceType = g_FrameConfigMgt.GetFrameBaseConfig().GetServerType();
	stSendMessage.nDestType = enmEntityType_Tunnel;
	stSendMessage.nSourceID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
	//	stSendMessage.nDestID = 0;
	stSendMessage.nSequence = 0;
	stSendMessage.nTransType = enmTransType_P2P;
	stSendMessage.nSessionIndex = enmInvalidSessionIndex;
	stSendMessage.nRoomID = nRoomID;
	stSendMessage.nRoleID = enmInvalidRoleID;
	stSendMessage.nZoneID = g_Frame.GetZoneID(enmRouterType_Default);

	uint32_t nPlayerOptionLen = 0;
	char arrPlayerOptionData[MaxOptionDataSize];

	int32_t ret = S_OK;
	if(enmBroadcastType_ExpectPlayr == nType)
	{
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(nRoleID, pPlayer, nPlayerIndex);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("get expect player object error!{nRoleID=%d, ret=0x%08x}\n", nRoleID, ret);
			return;
		}
		stSendMessage.nRoleID = nRoleID;
		stSendMessage.nDestID = pPlayer->GetConnInfo().nServerID;
		stSendMessage.nRouterIndex = g_Frame.GetRouterIndex(pPlayer->GetConnInfo().nRouterAddress, pPlayer->GetConnInfo().nRouterPort);

		SendMessage(&stSendMessage, pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
	}
	else if(enmBroadcastType_ExceptPlayr == nType)
	{
		stSendMessage.nTransType = enmTransType_Broadcast;
		stSendMessage.nDestID = enmInvalidServerID;

		stSendMessage.nRoleID = nRoleID;
		stSendMessage.nRouterIndex = 0;

		SendMessage(&stSendMessage, pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
	}
	else if(enmBroadcastType_All == nType)
	{
		stSendMessage.nTransType = enmTransType_Broadcast;
		stSendMessage.nDestID = enmInvalidServerID;
		stSendMessage.nRouterIndex = 0;

		DumpMessage(szDumpContent,&stSendMessage,pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
		SendMessage(&stSendMessage, pMsgBody, nPlayerOptionLen, arrPlayerOptionData);
	}
}

/*
 打印接受，发送的消息
 */
void CBllBase::DumpMessage(const char* szContent, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const char *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return;
	}
	//判断等级是否足够
	uint32_t offset = 0;
	char szLog[enmMaxLogInfoLength];
	szLog[0] = 0;
	sprintf(szLog + offset, szContent);
	offset = (uint32_t)strlen(szLog);

	sprintf(szLog + offset, " MessageHead=");
	offset = (uint32_t)strlen(szLog);

	pMsgHead->Dump(szLog, enmMaxLogInfoLength, offset);

	sprintf(szLog + offset, " MessageBody=");
	offset = (uint32_t)strlen(szLog);
	pMsgBody->Dump(szLog, enmMaxLogInfoLength, offset);

	WRITE_NOTICE_LOG("%s\n", szLog);
}

int32_t CBllBase::SendEvent(const RoleID roleID, uint32_t nMsgID, IMsgBody* pMsgBody)
{
	return S_OK;

}

int32_t CBllBase::ExitShow(CPlayer *pPlayer,CRoom *pRoom,bool sendNotifyToClient,int32_t nMsgID,const uint16_t nOptionLen, const void *pOptionData,ExitShowType nExitShowType)
{
	if(NULL == pPlayer || NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pPlayer=0x%08x}\n",pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	RoleID nRoleID = pPlayer->GetRoleID();
	RoomID nRoomID = pRoom->GetRoomID();

	COffMicNoti stOffMicNoti;
	bool isOnMic = false;
	bool isOnPublicMic = false;
	bool isOnP2PMic = false;

	PlayerState nPlayerState = pPlayer->GetStateInRoom(nRoomID);
	if(enmInvalidPlayerState == nPlayerState)
	{
		WRITE_ERROR_LOG("get nPlayerState error!{ret=0x%08x} \n",ret);
		return ret;
	}
	if(nExitShowType == enmExitShowType_P2P)
	{
		if(nPlayerState != enmPlayerState_P2PMic&&nPlayerState != enmPlayerState_P2PMicAndMicQueue)
		{
			WRITE_ERROR_LOG("Player %d exit P2P mic error because he is no om p2p!\n",nRoleID);
			return S_OK;
		}
		isOnP2PMic = true;
		//todo 一对一下麦
		ret = ExitP2PShow(pPlayer,pRoom,sendNotifyToClient,nOptionLen,pOptionData);
		if(ret<0)
		{
			WRITE_ERROR_LOG("Player %d exit P2P mic error !{ret=0x%08x} \n",nRoleID,ret);
		}
	}
	else
	{
		switch(nPlayerState)
		{
			case enmPlayerState_PublicMic:
			{
				stOffMicNoti.nMicType = enmMicType_PublicMic;
				isOnMic = true;
				isOnPublicMic = true;
				uint8_t nMicIndex;
				pPlayer->SetPublicMicIndexInRoom(0,nRoomID);
				ret = pRoom->RemoveOnPublicMic(nRoleID,nMicIndex);
				if(ret<0)
				{
					WRITE_ERROR_LOG("remove on public mic error !{nRoomID=%d, nRoleID=%d, ret=0x%08x} \n",nRoomID,nRoleID,ret);
					return S_OK;
				}
				if(!pPlayer->IsReboot())
				{
					StaticOnMIcTime(pPlayer,pRoom);
					SendPlayerMicStatusToDB(pRoom,pPlayer,enmMicOperateType_Public_Dowm,nMsgID,0);
				}
				pPlayer->SetStateInRoom(enmPlayerState_InRoom,nRoomID);
			}
			break;
			case enmPlayerState_PrivateMic:
			{
				stOffMicNoti.nMicType = enmMicType_PrivateMic;
				isOnMic = true;
				ret = pRoom->RemoveFromPrivateMic(nRoleID);
				if(ret<0)
				{
					WRITE_ERROR_LOG("remove player from private mic error!{nRoomID=%d, nRoleID=%d, ret=0x%08x} \n",nRoomID,nRoleID,ret);
					return S_OK;
				}
				if(!pPlayer->IsReboot())
				{
					StaticOnMIcTime(pPlayer,pRoom);
					SendPlayerMicStatusToDB(pRoom,pPlayer,enmMicOperateType_Private_Down,nMsgID,0);
				}
				pPlayer->SetStateInRoom(enmPlayerState_InRoom,nRoomID);
			}
			break;
			case enmPlayerState_PublicMicQueue:
			{
				ret = pRoom->RemoveFromWaittingMic(nRoleID);
				if(ret<0)
				{
					WRITE_ERROR_LOG("remove from waitting mic error !{nRoomID=%d, nRoleID=%d, ret=0x%08x} \n",nRoomID,nRoleID,ret);
					return S_OK;
				}
				pPlayer->SetStateInRoom(enmPlayerState_InRoom,nRoomID);
			}
			break;
			case enmPlayerState_P2PMicAndMicQueue:
			{
				ret = pRoom->RemoveFromWaittingMic(nRoleID);
				if(ret<0)
				{
					WRITE_ERROR_LOG("remove from waitting mic error !{nRoomID=%d, nRoleID=%d, ret=0x%08x} \n",nRoomID,nRoleID,ret);
					return S_OK;
				}
				pPlayer->SetStateInRoom(enmPlayerState_P2PMic,nRoomID);
			}
			break;
			default:
			{
				WRITE_DEBUG_LOG("player is not row mic!{nRoomID=%d, nRoleID=%d}\n",nRoomID,nRoleID);
				return S_OK;
			}
			break;
		}
	}
	//广播房间里面的人
	CExitShowNoti stExitShowNoti;
	stExitShowNoti.nSrcRoleID = nRoleID;
	stExitShowNoti.nExitShowType = nExitShowType;
	if(sendNotifyToClient)
	{
		SendMessageNotifyToClient(MSGID_RSCL_EXITSHOW_NOTI, &stExitShowNoti, nRoomID, enmBroadcastType_All,
				nRoleID,nOptionLen,(char*)pOptionData, "send exit Mic notify");
	}
	WRITE_NOTICE_LOG("player exit Mic!{nRoomID=%d, nRoleID=%d, nMicState=%d}",nRoomID,nRoleID,nPlayerState);

	if(isOnPublicMic)
	{
		WaittingToPublic(pRoom);

		//如果是艺人
		IdentityType nIdentityType = pPlayer->GetIdentityType();
		if((nIdentityType & enmIdentityType_SONGER) ||
				(nIdentityType & enmIdentityType_DANCER) ||
				(nIdentityType & enmIdentityType_HOST))
		{
			g_ClearRoomOrderInfo.CreateTimer(nRoomID, nRoleID);
		}
	}
	if(isOnMic)
	{
		pPlayer->SetPlayerGlobalState(enmPlayerState_InRoom);
	}
	if(isOnP2PMic || !isOnMic || pPlayer->IsReboot())
	{
		return S_OK;
	}
	//发送下麦通知到hallserver
	stOffMicNoti.nOffMicRoleID = pPlayer->GetRoleID();
	SendMessageNotifyToServer(MSGID_RSHS_OFFMIC_NOTIFY, &stOffMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);

	//发送下麦通知给mediaserver
	SendDelTransmitUserNotifyToMediaServer(nRoleID, pRoom, NULL, 0);

//	CDelTransmitUserNotice stDelTransmitUserNotice;
//	//当为下麦时nRoleID=arrDelRoleID[0]、nDelCount = 1
//	stDelTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stDelTransmitUserNotice.nRoleID = nRoleID;
//	stDelTransmitUserNotice.nDelCount = 1;
//	stDelTransmitUserNotice.arrDelRoleID[0]=nRoleID;
//	stDelTransmitUserNotice.nRoomID = nRoomID;
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,nRoomID,enmTransType_P2P,nRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,nRoomID,enmTransType_P2P,nRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));

	return S_OK;
}
int32_t CBllBase::ExitP2PShow(CPlayer *pSrcPlayer,CRoom *pRoom,bool sendNotifyToClient,const uint16_t nOptionLen, const void *pOptionData)
{
	if(NULL == pSrcPlayer||NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x,  pPlayer=0x%08x}\n",pSrcPlayer,pRoom);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	RoleID nP2PDestRoleID = enmInvalidRoleID;
	ret = pSrcPlayer->GetP2PRoleIDInRoom(pRoom->GetRoomID(),nP2PDestRoleID);
	if(ret<0 || nP2PDestRoleID == enmInvalidRoleID)
	{
		WRITE_ERROR_LOG("get player P2PRoleId filed!{nRoleID=%d, nP2PRoleID=%d}\n",pSrcPlayer->GetRoleID(),nP2PDestRoleID);
		return ret;
	}
	CPlayer *pP2PDestPlayer = NULL;
	PlayerIndex nP2PPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nP2PDestRoleID, pP2PDestPlayer, nP2PPlayerIndex);
	//没有次玩家
	if(ret < 0 || pP2PDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist!{nRoleID=%d}",nP2PDestRoleID);
		return ret;
	}

	if(!(pRoom->IsPlayerInRoom(nP2PPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does not in room !{nP2PRoleID=%d, nRoomID=%d}\n",nP2PDestRoleID,pRoom->GetRoomID());
		return ret;
	}
	//如果在麦序
	if(pRoom->IsInWaittingMic(pSrcPlayer->GetRoleID()))
	{
		ret = pSrcPlayer->SetStateInRoom(enmPlayerState_PublicMicQueue,pRoom->GetRoomID());
	}
	else
	{
		//设置状态为在房间
		ret = pSrcPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
	}
	if(ret < 0)
	{
		WRITE_ERROR_LOG("player does not in room !{nRoleID=%d, nRoomID=%d}\n",pSrcPlayer->GetRoleID(),pRoom->GetRoomID());
		return ret;
	}

	if(pRoom->IsInWaittingMic(pP2PDestPlayer->GetRoleID()))
	{
		ret = pP2PDestPlayer->SetStateInRoom(enmPlayerState_PublicMicQueue,pRoom->GetRoomID());
	}
	else
	{
		ret = pP2PDestPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
	}
	if(ret<0)
	{
		WRITE_ERROR_LOG("player does not in room !{nP2PRoleID=%d, nRoomID=%d}\n",nP2PDestRoleID,pRoom->GetRoomID());
		return ret;
	}
	//设置一对一玩家为enmInvalidRoleID
	ret = pSrcPlayer->SetP2PRoleIDInRoom(pRoom->GetRoomID(),enmInvalidRoleID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("player does not in room !{nRoleID=%d,nRoomID=%d}\n",pSrcPlayer->GetRoleID(),pRoom->GetRoomID());
		return ret;
	}
	ret = pP2PDestPlayer->SetP2PRoleIDInRoom(pRoom->GetRoomID(),enmInvalidRoleID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("player does not in room !{nP2PRoleID=%d, nRoomID=%d}\n",nP2PDestRoleID,pRoom->GetRoomID());
		return ret;
	}

	//发送结束一对一视频通知给玩家(dest)
	if(sendNotifyToClient)
	{
		CExitShowNoti stExitShowNoti;
		stExitShowNoti.nSrcRoleID = pP2PDestPlayer->GetRoleID();
		stExitShowNoti.nExitShowType = enmExitShowType_P2P;
		SendMessageNotifyToClient(MSGID_RSCL_EXITSHOW_NOTI, &stExitShowNoti, pRoom->GetRoomID(), enmBroadcastType_All,
				nP2PDestRoleID,nOptionLen,(char*)pOptionData, "send exit p2P Mic notify");
	}
	pSrcPlayer->SetPlayerGlobalState(enmPlayerState_InRoom);
	pP2PDestPlayer->SetPlayerGlobalState(enmPlayerState_InRoom);
	//发送通知给mediaserver, P2P下麦
	SendDelTransmitUserNotifyToMediaServer(pSrcPlayer->GetRoleID(), pRoom, NULL, 0);
	SendDelTransmitUserNotifyToMediaServer(nP2PDestRoleID, pRoom, NULL, 0);

//	CDelTransmitUserNotice stDelTransmitUserNotice;
//	stDelTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stDelTransmitUserNotice.nRoleID = pSrcPlayer->GetRoleID();
//	stDelTransmitUserNotice.nDelCount = 1;
//	stDelTransmitUserNotice.arrDelRoleID[0]=nP2PDestRoleID;
//	stDelTransmitUserNotice.nRoomID = pRoom->GetRoomID();
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,pRoom->GetRoomID(),enmTransType_P2P,pSrcPlayer->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(pP2PDestPlayer->GetPlayerNetType()));
//	//这里可以完成积分的计算
//	stDelTransmitUserNotice.nRoleID = nP2PDestRoleID;
//	stDelTransmitUserNotice.arrDelRoleID[0]=pSrcPlayer->GetRoleID();
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,pRoom->GetRoomID(),enmTransType_P2P,nP2PDestRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(pSrcPlayer->GetPlayerNetType()));
	WRITE_NOTICE_LOG("player exit P2P Mic!{nRoleID=%d, nP2PRoleID=%d}\n",pSrcPlayer->GetRoleID(),nP2PDestRoleID);
	COffMicNoti stOffMicNoti;
	stOffMicNoti.nMicType = enmMicType_P2PMic;
	stOffMicNoti.nOffMicRoleID = pSrcPlayer->GetRoleID();
	SendMessageNotifyToServer(MSGID_RSHS_OFFMIC_NOTIFY, &stOffMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pSrcPlayer->GetRoleID(), enmEntityType_Hall);
	stOffMicNoti.nOffMicRoleID = nP2PDestRoleID;
	SendMessageNotifyToServer(MSGID_RSHS_OFFMIC_NOTIFY, &stOffMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, nP2PDestRoleID, enmEntityType_Hall);

	return ret;
}
int32_t CBllBase::WaittingToPublic(CRoom *pRoom)
{
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	if(!pRoom->IsFreeShowOpened())
	{
		return S_OK;
	}
	if(!pRoom->HasPublicMicIndex())
	{
		return S_OK;
	}
	int32_t nCanGoToMicCount = pRoom->GetCanGotoMicCount();
	for(int32_t i = 0;i < nCanGoToMicCount;i++)
	{
		WaittingListToPublicMic(pRoom);
	}
	return S_OK;
}

int32_t CBllBase::WaittingListToPublicMic(CRoom *pRoom)
{
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	RoleID nFirstRoleID = pRoom->GetFirstWaittingRoleID();
	bool isOnMic = true;
	while(isOnMic)
	{
		while(NULL == pPlayer && nFirstRoleID != enmInvalidRoleID)
		{
			ret = g_PlayerMgt.GetPlayer(nFirstRoleID, pPlayer, nPlayerIndex);
			//没有次玩家
			if(ret < 0 || pPlayer == NULL)
			{
				WRITE_ERROR_LOG("player does no exist!{nFirstRoleID=%d}\n",nFirstRoleID);
				nFirstRoleID = pRoom->GetFirstWaittingRoleID();
			}
			else if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
			{
				pPlayer = NULL;
				nFirstRoleID = pRoom->GetFirstWaittingRoleID();
			}
		}
		if(nFirstRoleID == enmInvalidRoleID)
		{
			WRITE_NOTICE_LOG("room no waitting!{nRoomID=%d} ",pRoom->GetRoomID());
			return S_OK;
		}
		//todo 判断玩家是否在一对一视频
		isOnMic = pPlayer->IsOnMic();
		if(isOnMic)
		{
			CRequestShowNoti stRequestShowNoti;
			stRequestShowNoti.nRequestShowResult = enmRequestShowResult_Wait_But_OnMic;
			stRequestShowNoti.nSrcRoleID = pPlayer->GetRoleID();
			SendMessageNotifyToClient(MSGID_RSCL_REQUESTSHOW_NOTI, &stRequestShowNoti, pRoom->GetRoomID(), enmBroadcastType_All,
					pPlayer->GetRoleID());
			// 如果是在麦序的状态，更新为在房间
			if(pPlayer->GetStateInRoom(pRoom->GetRoomID()) == enmPlayerState_PublicMicQueue)
			{
				ret = pPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
			}
			if(pPlayer->GetStateInRoom(pRoom->GetRoomID()) == enmPlayerState_P2PMicAndMicQueue)
			{
				ret = pPlayer->SetStateInRoom(enmPlayerState_P2PMic,pRoom->GetRoomID());
			}
			pPlayer = NULL;
			nFirstRoleID = pRoom->GetFirstWaittingRoleID();
		}
	}

	if(pPlayer == NULL)
	{
		WRITE_NOTICE_LOG("room no waitting!{nRoomID=%d} ",pRoom->GetRoomID());
		return S_OK;
	}
	uint8_t nMicIndex = 0;
	ret = pRoom->AddOnPublicMic(nFirstRoleID,nMicIndex);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("add player to public filed!{nFirstRoleID=%d}\n",nFirstRoleID);
		return ret;
	}
	ret = pPlayer->SetStateInRoom(enmPlayerState_PublicMic,pRoom->GetRoomID());
	if(ret < 0)
	{
		WRITE_ERROR_LOG("player does no in room!{nFirstRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
		return ret;
	}
	ret = pPlayer->SetPublicMicIndexInRoom(nMicIndex,pRoom->GetRoomID());
	if(ret < 0)
	{
		return ret;
	}
	RoleRank nRoleRank = pRoom->GetRoleRank(pPlayer->GetRoleID());
	uint32_t onMicTime = pRoom->GetMicTimeByRoleRank(nRoleRank);
	//给房间的所有人发通知
	CRequestShowNoti stRequestShowNoti;
	stRequestShowNoti.nRequestShowResult = enmRequestShowResult_Public;
	stRequestShowNoti.nSrcRoleID = pPlayer->GetRoleID();
	stRequestShowNoti.nIndex = nMicIndex;
	stRequestShowNoti.nTime = (uint16_t)onMicTime;
	SendMessageNotifyToClient(MSGID_RSCL_REQUESTSHOW_NOTI, &stRequestShowNoti, pRoom->GetRoomID(), enmBroadcastType_All,
			pPlayer->GetRoleID());

	WRITE_NOTICE_LOG("player push on  public mic from waitting!{nFirstRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
	//发送添加转发通知给mediaserver
	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	int32_t nPlayerCount = 0;
	ret = g_DataCenter.GetAllPlayers(pRoom->GetRoomID(), arrPlayer, MaxUserCountPerRoom, nPlayerCount);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("get room players object error{RoomID=%d, ret=0x%08x}", pRoom->GetRoomID(), ret);
		return ret;
	}

	//向Media发送上麦通知
	SendAddTransmitUserNotifyToMediaServer(pPlayer, arrPlayer, nPlayerCount, pRoom, true);

	//单发主播上麦
//	SendPlayerOnMicToMedia(pRoom,pPlayer);
//
//	CAddTransmitUserNotice stAddTransmitUserNotice;
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nRoleID = pPlayer->GetRoleID();
//	stAddTransmitUserNotice.nRoomID = pRoom->GetRoomID();
//	stAddTransmitUserNotice.arrAddRoleID[0] = stAddTransmitUserNotice.nRoleID; //上公麦SpeakerID与ReciverID一致即可
//	stAddTransmitUserNotice.nAddCount=1;
//	for(int32_t i = 0;i < nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//			continue;
//		}
//		if(pPlayer->GetRoleID() != arrPlayer[i]->GetRoleID() && arrPlayer[i]->GetPlayerNetType() == enmNetType_Tel)
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_Broadcast,pPlayer->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//	stAddTransmitUserNotice.nAddCount = 0;
//	for(int32_t i = 0;i < nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//			continue;
//		}
//		if(pPlayer->GetRoleID() != arrPlayer[i]->GetRoleID() && arrPlayer[i]->GetPlayerNetType() == enmNetType_Cnc)
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_Broadcast,pPlayer->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));

	SendPlayerMicStatusToDB(pRoom,pPlayer,enmMicOperateType_Public_Up,MSGID_RSCL_EXITSHOW_NOTI,0);

	pPlayer->SetPlayerGlobalState(enmPlayerState_PublicMic);

	//删除歌手的订单计时器
	g_ClearRoomOrderInfo.DeleteTimer(pPlayer->GetRoleID());

	COnMicNoti stOnMicNoti;
	stOnMicNoti.nOnMicRoleID = pPlayer->GetRoleID();
	stOnMicNoti.nMicType = enmMicType_PublicMic;
	SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);
	return ret;
}
int32_t CBllBase::ExitRoom(CPlayer *pPlayer,PlayerIndex nPlayerIndex,CRoom *pRoom,int32_t nMsgID,bool sendNotifyToClient,bool isNeedKickRobot)
{
	int32_t ret =S_OK;
	if(NULL == pPlayer || NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x,  pPlayer=0x%08x }\n",pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	//退出房间前的准备
	//下麦
	RoomID nRoomID = pRoom->GetRoomID();
	RoleID nRoleID = pPlayer->GetRoleID();
	RoleRank nRoleRank = pRoom->GetRoleRank(nRoleID);
	PlayerState nPlayerState = pPlayer->GetStateInRoom(nRoomID);
	if(nPlayerState == enmPlayerState_P2PMic || nPlayerState == enmPlayerState_P2PMicAndMicQueue)
	{
		ret = ExitShow(pPlayer,pRoom,sendNotifyToClient,nMsgID,0,NULL,enmExitShowType_P2P);
		if(ret<0)
		{
			WRITE_ERROR_LOG(" player exit room filed because exit show filed!{nRoleID=%d, nRoomID=%d}",nRoleID,nRoomID);
			return ret;
		}
	}
	ret = ExitShow(pPlayer,pRoom,sendNotifyToClient,nMsgID);
	if(ret < 0)
	{
		WRITE_ERROR_LOG(" player exit room filed because exit show filed!{nRoleID=%d, nRoomID=%d}",nRoleID,nRoomID);
		return ret;
	}
	//todo 计算在房间的在线时间
	{
		//超管，及机器人的管理员不统计
		ret = StaticInRoomTime(pPlayer,pRoom);
		if(ret < 0)
		{
			WRITE_ERROR_LOG(" player exit room filed because static in room time filed!{nRoleID=%d, nRoomID=%d}",nRoleID,nRoomID);
			return ret;
		}

	}
	//删除临时管理员以及超管的操作(错误不用返回，接着执行完退房)
	if(nRoleRank == enmRoleRank_Super || nRoleRank == enmRoleRank_TempAdmin)
	{
		ret = pRoom->DelMemberList(nRoleID,nRoleRank);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("del rank in room filed !{nRoleID=%d, nRomID=%d, ret=0x%08x}\n",nRoleID,nRoomID,ret);
		}
		ret = pPlayer->SetRoleRankInRoom(enmRoleRank_None,nRoomID);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("del rank in room filed !{nRoleID=%d, nRomID=%d, ret=0x%08x}\n",nRoleID,nRoomID,ret);
		}
		ret = pPlayer->DelAdminRoom(nRoomID);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("del rank in room filed !{nRoleID=%d, nRomID=%d, ret=0x%08x}\n",nRoleID,nRoomID,ret);
		}
		//发送通知到需要的服务器
	}
	pPlayer->ExitRoom(nRoomID);
	if(pPlayer->IsReboot())
	{
		ret = pRoom->DeleteRebotPlayer(nRoleID);
		if(ret < 0)
		{
			WRITE_ERROR_LOG("exit room filed because  deletePlayer player error !{nRoleID=%d, nRoomID=%d, ret=0x%08x}\n",nRoleID,nRoomID,ret);
		}
		//回收机器人的roleID
		if(pPlayer->GetCurEnterRoomCount() == 0)
		{
			WRITE_DEBUG_LOG("recover robot roleID %d!!!!!!!!",nRoleID);
			GET_REBOTCTL_INSTANCE().RecoverRoleID(nRoleID);
		}
	}
	else
	{
		ret = pRoom->DeletePlayer(nPlayerIndex,pPlayer->IsHideEnter());
		if(ret < 0)
		{
			WRITE_WARNING_LOG("exit room has error because  deletePlayer player error !{nRoleID=%d, nRoomID=%d, ret=0x%08x}\n",nRoleID,nRoomID,ret);
		}
		int32_t nNeedRobotCount = pRoom->GetNeedRobotCount();
		if(nNeedRobotCount < 0 && isNeedKickRobot)
		{
			KickRobot(pRoom,-nNeedRobotCount);
		}

		//pRoom->UpdateUserCountOnMedia(pPlayer->GetConnInfo().nServerID, -1);
	}
	WRITE_NOTICE_LOG("player exit room!{nRoleID=%d, nRoomID=%d}\n",nRoleID,nRoomID);
	//通知相应的server
	CExitRoomNotice stExitRoomNoti;
	stExitRoomNoti.nRoleID = nRoleID;
	stExitRoomNoti.nRoomID = nRoomID;
	stExitRoomNoti.nRoomType = pRoom->GetRoomType();
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Item);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Hall);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_RoomDispatcher);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Task);
	SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_HallDataCenter);
	if(!pPlayer->IsReboot())
	{
		//发给网关
		SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Tunnel,pPlayer->GetConnInfo().nServerID);

		//发过mediaserver
		SendMessageNotifyToServer(MSGID_RSMS_EXIT_ROOM_NOTICE, &stExitRoomNoti, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media, pPlayer->GetConnInfo().nServerID);
	}
	if((pRoom->GetCurUserCount() != 0) && sendNotifyToClient)
	{
		CLogoutNoti stLogoutNoti;
		stLogoutNoti.nRoleID = nRoleID;
		SendMessageNotifyToClient(MSGID_RSCL_LOGOUT_NOTI, &stLogoutNoti, nRoomID, enmBroadcastType_All,
				nRoleID, 0, NULL, "send logout room notify message");
	}
	//更新在房间的玩家到memcache
	CStaPlayerCount stStaPlayerCount;
	stStaPlayerCount.nRoomID = pRoom->GetRoomID();
	stStaPlayerCount.nPlayerCount = pRoom->GetCurUserCount();
	stStaPlayerCount.nRobotCount = pRoom->GetRobotCount();
	SendMessageNotifyToServer(MSGID_RSDB_STA_PLAYER_COUNT_NOTIFY, &stStaPlayerCount, pRoom->GetRoomID(), enmTransType_ByKey, pRoom->GetRoomID(), enmEntityType_DBProxy);
	//判断房间里是否还有人，没有则销毁房间
//	if((pRoom->GetCurUserCount() == 0) && (pRoom->GetRobotCount() == 0) && !(pPlayer->IsReboot()))
//	{
//		WRITE_DEBUG_LOG("room not player so destroy room!{nRoomID=%d}\n",nRoomID);
//		CDestoryRoomNotice stDestoryRoomNotice;
//		stDestoryRoomNotice.nRoomID = nRoomID;
//		stDestoryRoomNotice.nServerID = g_Frame.GetServerConfigImpl()->GetServerID();
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Item);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_RoomDispatcher);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Hall);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_Broadcast, nRoleID, enmEntityType_Tunnel);
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//		SendMessageNotifyToServer(MSGID_RSMS_DESTORY_ROOM_NOTICE, &stDestoryRoomNotice, nRoomID, enmTransType_P2P, nRoleID, enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));
//		g_RoomMgt.DestroyRoom(nRoomID);
//	}
	//销毁玩家对象
	if(pPlayer->GetCurEnterRoomCount() == 0)
	{
		WRITE_DEBUG_LOG("player is exit all room so destroy Player!{nRoleID=%d}\n",nRoleID);
		g_PlayerMgt.DestroyPlayer(nRoleID);
	}
	return S_OK;
}

int32_t CBllBase::KickRobot(CRoom *pRoom,uint32_t nKickCount)
{
	int32_t ret = S_OK;
	if(NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	for(uint32_t i = 0;i < nKickCount;i++)
	{
		RoleID nRoleID = pRoom->GetKickRobotRoleID();
		//优化代码(机器人出房间，从用户列表)
		if(nRoleID == enmInvalidRoleID)
		{
			WRITE_ERROR_LOG("get kick roleid filed\n");
			continue;
		}
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(nRoleID, pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("get robot player object error!{nRoleID=%d, ret=0x%08x}\n",nRoleID,ret);
			continue;
		}
		ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,0,true);
		if(ret<0)
		{
			WRITE_ERROR_LOG(" robot player exit room filed!{nRoleID=%d,nRoomID=%d}\n",nRoleID,pRoom->GetRoomID());
			continue;
		}
	}

	return ret;
}

int32_t CBllBase::StaticInRoomTime(CPlayer *pPlayer,CRoom *pRoom)
{
	int32_t ret = S_OK;
	if(NULL == pPlayer || pRoom == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x,  pPlayer=0x%08x }\n",pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	if(pPlayer->IsReboot() || pPlayer->GetVipLevel() == enmVipLevel_NONE)
	{
		WRITE_NOTICE_LOG("player is robot or visitor do not need static!{nRoleID=%d}\n",pPlayer->GetRoleID());
		return ret;
	}
	RoomID nRoomID = pRoom->GetRoomID();
	RoleID nRoleID = pPlayer->GetRoleID();
	uint32_t nLastStaTime = pPlayer->GetLastStaTime(nRoomID);
	//判断上次统计时间和现在的时间是不是在同一天
	CDateTime stLastDate(nLastStaTime*US_PER_SECOND);
	uint16_t nCurDay = CDateTime::CurrentDateTime().Day();
	uint16_t nMonth = CDateTime::CurrentDateTime().Month();
	uint16_t nYear = CDateTime::CurrentDateTime().Year();
	CAddAdmInRoomOnLineTimeNotice stAddAdmInRoomOnLineTimeNotice;
	stAddAdmInRoomOnLineTimeNotice.nRoleID = nRoleID;
	stAddAdmInRoomOnLineTimeNotice.nRoomID = nRoomID;
	//如果不是同一天
	if(nCurDay != stLastDate.Day())
	{
		//更新上一天的表
		//取得几天0点0时0分的时间
		CDateTime stZeroDate (nYear,nMonth,nCurDay);
		uint32_t nZeroTime = stZeroDate.Seconds();
		uint32_t nAddTotleTime = (nZeroTime-nLastStaTime)/60;
		if(nAddTotleTime != 0)
		{
			pRoom->addAdminInRoomTime(nRoleID,nAddTotleTime);
			//通知数据库修改
			stAddAdmInRoomOnLineTimeNotice.nYear = stLastDate.Year();
			stAddAdmInRoomOnLineTimeNotice.nMonth = stLastDate.Month();
			stAddAdmInRoomOnLineTimeNotice.nDay = stLastDate.Day();
			stAddAdmInRoomOnLineTimeNotice.nAddTime = nAddTotleTime;
			stAddAdmInRoomOnLineTimeNotice.nRoleRank = pRoom->GetRoleRank(nRoleID);
			SendMessageNotifyToServer(MSGID_RSDB_ADD_ONLINE_TIME_INROOM_NOTICE, &stAddAdmInRoomOnLineTimeNotice, nRoomID, enmTransType_ByKey, nRoleID, enmEntityType_DBProxy);
			WRITE_DEBUG_LOG("player in room add in room time !{nRoleID=%d, nRoomID=%d, nAddTotleTime=%d, year=%d, month=%d, day=%d}\n",nRoleID,nRoomID,nAddTotleTime,stLastDate.Year(),stLastDate.Month(),stLastDate.Day());
			pPlayer->SetLastStaTime(nRoomID,nZeroTime);
		}

	}
	//更新今天的表
	nLastStaTime = pPlayer->GetLastStaTime(nRoomID);
	uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
	uint32_t nAddTotleTime = (nCurTime-nLastStaTime)/60;
	if(nAddTotleTime != 0)
	{
		pRoom->addAdminInRoomTime(nRoleID,nAddTotleTime);
		WRITE_DEBUG_LOG("player in room add in room time !{nRoleID=%d, nRoomID=%d, nAddTotleTime=%d, year=%d, month=%d, day=%d}\n",nRoleID,nRoomID,nAddTotleTime,nYear,nMonth,nCurDay);
		//通知数据库修改
		stAddAdmInRoomOnLineTimeNotice.nYear = nYear;
		stAddAdmInRoomOnLineTimeNotice.nMonth = nMonth;
		stAddAdmInRoomOnLineTimeNotice.nDay = nCurDay;
		stAddAdmInRoomOnLineTimeNotice.nAddTime = nAddTotleTime;
		stAddAdmInRoomOnLineTimeNotice.nRoleRank = pRoom->GetRoleRank(nRoleID);
		SendMessageNotifyToServer(MSGID_RSDB_ADD_ONLINE_TIME_INROOM_NOTICE, &stAddAdmInRoomOnLineTimeNotice, nRoomID, enmTransType_ByKey, nRoleID, enmEntityType_DBProxy);
		pPlayer->SetLastStaTime(nRoomID,nCurTime);
	}

	return ret;
}
//统计玩家的在mic时间
int32_t CBllBase::StaticOnMIcTime(CPlayer *pPlayer,CRoom *pRoom)
{
	int32_t ret = S_OK;
	if(NULL == pPlayer || NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x,  pPlayer=0x%08x}\n",pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	if(pPlayer->IsReboot() || pPlayer->GetVipLevel() == enmVipLevel_NONE)
	{
		WRITE_NOTICE_LOG("player is robot or visitor do not need static!{nRoleID=%d}\n",pPlayer->GetRoleID());
		return ret;
	}
	RoomID nRoomID = pRoom->GetRoomID();
	RoleID nRoleID = pPlayer->GetRoleID();
	uint32_t nLastStaTime = pPlayer->GetLastStaOnMicTime(nRoomID);
	//判断上次统计时间和现在的时间是不是在同一天
	CDateTime stLastDate(nLastStaTime*US_PER_SECOND);
	uint16_t nCurDay = CDateTime::CurrentDateTime().Day();
	uint16_t nMonth = CDateTime::CurrentDateTime().Month();
	uint16_t nYear = CDateTime::CurrentDateTime().Year();
	CAddOnMicTimeNotice stAddOnMicTimeNotice;
	stAddOnMicTimeNotice.nRoleID = nRoleID;
	stAddOnMicTimeNotice.nRoomID = nRoomID;
	//如果不是同一天
	if(nCurDay != stLastDate.Day())
	{
		//更新上一天的表
		//取得几天0点0时0分的时间
		CDateTime stZeroDate (nYear,nMonth,nCurDay);
		uint32_t nZeroTime = stZeroDate.Seconds();
		uint32_t nAddTotleTime = (nZeroTime-nLastStaTime)/60;
		if((nAddTotleTime) != 0)
		{
			pRoom->addAdminOnMicTime(nRoleID,nAddTotleTime);
			//通知数据库修改
			stAddOnMicTimeNotice.nYear = stLastDate.Year();
			stAddOnMicTimeNotice.nMonth = stLastDate.Month();
			stAddOnMicTimeNotice.nDay = stLastDate.Day();
			stAddOnMicTimeNotice.nAddTime = nAddTotleTime;
			SendMessageNotifyToServer(MSGID_RSDB_ADD_ONMIC_TIME_INROOM_NOTICE, &stAddOnMicTimeNotice, nRoomID, enmTransType_ByKey, nRoleID, enmEntityType_DBProxy);
			WRITE_DEBUG_LOG("player in room add on mic time !{nRoleID=%d, nRoomID=%d, nAddTotleTime=%d, year=%d, month=%d, day=%d}\n",nRoleID,nRoomID,nAddTotleTime,stLastDate.Year(),stLastDate.Month(),stLastDate.Day());
			pPlayer->SetLastStaOnMicTime(nRoomID,nZeroTime);
		}

	}
	//更新今天的表
	nLastStaTime = pPlayer->GetLastStaOnMicTime(nRoomID);
	uint32_t nCurTime = CTimeValue::CurrentTime().Seconds();
	uint32_t nAddTotleTime = (uint32_t)(nCurTime-nLastStaTime)/60;
	if((nAddTotleTime) != 0)
	{
		pRoom->addAdminOnMicTime(nRoleID,nAddTotleTime);
		//通知数据库修改
		stAddOnMicTimeNotice.nYear = nYear;
		stAddOnMicTimeNotice.nMonth = nMonth;
		stAddOnMicTimeNotice.nDay = nCurDay;
		stAddOnMicTimeNotice.nAddTime = nAddTotleTime;
		SendMessageNotifyToServer(MSGID_RSDB_ADD_ONMIC_TIME_INROOM_NOTICE, &stAddOnMicTimeNotice, nRoomID, enmTransType_ByKey, nRoleID, enmEntityType_DBProxy);
		WRITE_DEBUG_LOG("player in room add on mic time !{nRoleID=%d, nRoomID=%d, nAddTotleTime=%d, year=%d, month=%d, day=%d}\n",nRoleID,nRoomID,nAddTotleTime,nYear,nMonth,nCurDay);
		pPlayer->SetLastStaOnMicTime(nRoomID,nCurTime);
	}

	return ret;
}

bool CBllBase::ConnInfoIsOK(CPlayer *pPlayer,const uint16_t nOptionLen, const void *pOptionData)
{
	uint32_t nPlayerOptionLen = 0;
	char arrPlayerOptionData[MaxOptionDataSize];

	ConnUin stConnUin = pPlayer->GetConnInfo();

	stConnUin.MessageEncode((uint8_t *)arrPlayerOptionData, MaxOptionDataSize, nPlayerOptionLen);

	return (memcmp((char *)pOptionData,arrPlayerOptionData, nPlayerOptionLen)==0);
}

void CBllBase::SendColorToPalyer(CRoom *pRoom,CPlayer *pPlayer)
{
	if(NULL == pPlayer || NULL == pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x,  pPlayer=0x%08x}\n",pPlayer,pRoom);
		return;
	}
	//获取房间现在的机器人
	int32_t nPlayerCount = 0;
	RoleID arrRoleID[MaxUserCountPerRoom] = {0};
	//获取房间里的机器人
	if(!(pRoom->IsSendColorOpen()))
	{
		WRITE_DEBUG_LOG("room close color send{nRoomID=%d}",pRoom->GetRoomID());
		return;
	}
	pRoom->GetAllRebotPlayers(arrRoleID,MaxUserCountPerRoom,nPlayerCount);
	int32_t nSendCount = Random2(MAX_SEND_COUNT*1000)/1000+1;
	if(nSendCount > nPlayerCount)
	{
		nSendCount = nPlayerCount;
	}
	for(int32_t i = 0;i < nSendCount;i++)
	{
		//随机出一个玩家
		int32_t nIndex = Random2(nPlayerCount*1000)/1000;
		if(nIndex < 0 || nIndex >= nPlayerCount)
		{
			continue;
		}
		RoleID nRobotRoleID = arrRoleID[nIndex];
		CSendTextNoti stSendTextNoti;
		stSendTextNoti.nTextType = enmTextType_Public;
		stSendTextNoti.nSrcID = nRobotRoleID;
		stSendTextNoti.nDestID = pPlayer->GetRoleID();
		char strTemp[20] = {'\0'};
		sprintf(strTemp,"%d",GET_REBOTCTL_INSTANCE().GetColorType());
		stSendTextNoti.strMessage = strTemp;
		stSendTextNoti.nFontSize = 0;
		stSendTextNoti.nFontColor = 0;
		stSendTextNoti.nFontStyle = enmFontStyleType_COLORTEXT;
		stSendTextNoti.strFontType = "send";
		GET_BLLBASE_INSTANCE().SendMessageNotifyToClient(MSGID_RSCL_SENDTEXT_NOTI, &stSendTextNoti, pRoom->GetRoomID(), enmBroadcastType_All,
				nRobotRoleID);
	}

}

int32_t CBllBase::ReBulidMediaServer(CSetMediaInfoNoti* pSetMediaInfoNoti)
{
	if(NULL == pSetMediaInfoNoti)
	{
		WRITE_ERROR_LOG("null pointer:{pSetMediaInfoNoti=0x%08x}\n",pSetMediaInfoNoti);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pSetMediaInfoNoti->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x} \n",pSetMediaInfoNoti->nRoomID,ret);
		return ret;
	}
	//设置房间新的media
	uint32_t arrMediaAddress[enmNetType_Count];
	uint16_t arrMediaPort[enmNetType_Count];
	ServerID arrMediaServerID[enmNetType_Count];
	arrMediaAddress[enmNetType_Tel] = pSetMediaInfoNoti->nTelMediaIP;
	arrMediaPort[enmNetType_Tel] = pSetMediaInfoNoti->nTelMediaPort;
	arrMediaAddress[enmNetType_Cnc] = pSetMediaInfoNoti->nCncMediaIP;
	arrMediaPort[enmNetType_Cnc] = pSetMediaInfoNoti->nCncMediaPort;
	arrMediaServerID[enmNetType_Tel] = pSetMediaInfoNoti->nTelServerID;
	arrMediaServerID[enmNetType_Cnc] = pSetMediaInfoNoti->nCncServerID;

	pRoom->SetMediaAddressPort(arrMediaAddress, arrMediaPort);
	pRoom->SetMediaServerID(arrMediaServerID);

	WRITE_NOTICE_LOG("room reBulid media!{nRoomID=%d,telMedia=%d,cncMedia=%d}\n",pSetMediaInfoNoti->nRoomID,arrMediaServerID[enmNetType_Cnc],arrMediaServerID[enmNetType_Tel]);
	if(arrMediaServerID[enmNetType_Tel]==enmInvalidServerID||arrMediaServerID[enmNetType_Cnc]==enmInvalidServerID)
	{
		return S_OK;
	}
	//通知玩家进房
	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	int32_t nPlayerCount = 0;
	ret = g_DataCenter.GetAllPlayers(pSetMediaInfoNoti->nRoomID, arrPlayer, MaxUserCountPerRoom, nPlayerCount);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("get room players object error{nRoomID=%d,ret=0x%08x}", pSetMediaInfoNoti->nRoomID, ret);
		return ret;
	}
	if(nPlayerCount == 0)
	{
		WRITE_NOTICE_LOG("room not has player!{nRoomID=%d}",pSetMediaInfoNoti->nRoomID);
		return ret;
	}
	//通知玩家进入房间
	for(int32_t i = 0;i < nPlayerCount;++i)
	{
		//进入房间通知
		CEnterRoomNotice stEnterRoomNotice;
		stEnterRoomNotice.nRoleID = arrPlayer[i]->GetRoleID();
		stEnterRoomNotice.nRoomID = pSetMediaInfoNoti->nRoomID;
		stEnterRoomNotice.nServerID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();
		stEnterRoomNotice.nPlayerType = enmPlayerType_Normal;
		stEnterRoomNotice.nRoleRank = pRoom->GetRoleRank(arrPlayer[i]->GetRoleID());
		stEnterRoomNotice.strRoleName = arrPlayer[i]->GetRoleName();
		stEnterRoomNotice.nAccountID = arrPlayer[i]->GetAccountID();
		stEnterRoomNotice.nRoomType = pRoom->GetRoomType();
		stEnterRoomNotice.nIdentityType = arrPlayer[i]->GetIdentityType();
		stEnterRoomNotice.nKey = arrPlayer[i]->GetKey(pRoom->GetRoomID());
		WRITE_NOTICE_LOG("redispatch media player enter room!{nRoleID=%d, nRoomID=%d}\n",arrPlayer[i]->GetRoleID(),pSetMediaInfoNoti->nRoomID);

		SendMessageNotifyToServer(MSGID_RSMS_ENTER_ROOM_NOTICE, &stEnterRoomNotice, pSetMediaInfoNoti->nRoomID, enmTransType_P2P, arrPlayer[i]->GetRoleID(),
				enmEntityType_Media, arrPlayer[i]->GetConnInfo().nServerID);
	}
	//一对一的玩家先通知
	for(int32_t i = 0;i < nPlayerCount;i++)
	{
		//看是否在一对一麦上
		RoleID nP2PRoleID = enmInvalidRoleID;
		arrPlayer[i]->GetP2PRoleIDInRoom(pSetMediaInfoNoti->nRoomID,nP2PRoleID);
		if(nP2PRoleID != enmInvalidRoleID)
		{
			//获取玩家
			CPlayer *pP2PPlayer = NULL;
			PlayerIndex nP2PPlayerIndex = enmInvalidPlayerIndex;
			ret = g_PlayerMgt.GetPlayer(nP2PRoleID, pP2PPlayer, nP2PPlayerIndex);
			if(ret < 0 || pP2PPlayer == NULL)
			{
				//没有次玩家
				WRITE_ERROR_LOG("player does no exist!{nP2PRoleID=%d}",nP2PRoleID);
				continue;
			}

//			SendPlayerOnMicToMedia(pRoom,arrPlayer[i]);
//			SendPlayerOnMicToMedia(pRoom,pP2PPlayer);

			//发送两条通知消息到mediaServer(p2p上麦)
			CPlayer* arrReciver[1] = {pP2PPlayer};
			SendAddTransmitUserNotifyToMediaServer(arrPlayer[i], arrReciver, 1, pRoom, false);
			arrReciver[0] = arrPlayer[i];
			SendAddTransmitUserNotifyToMediaServer(pP2PPlayer, arrReciver, 1, pRoom, false);

//			//发送两条通知消息到mediaServer(p2p上麦)
//			CAddTransmitUserNotice stP2PAddTransmitUserNotice;
//			stP2PAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//			stP2PAddTransmitUserNotice.nRoleID = arrPlayer[i]->GetRoleID();
//			stP2PAddTransmitUserNotice.nAddCount = 1;
//			stP2PAddTransmitUserNotice.arrAddRoleID[0]=nP2PRoleID;
//			stP2PAddTransmitUserNotice.nRoomID = pSetMediaInfoNoti->nRoomID;
//			//发送请求者的转发添加通知
//			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stP2PAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_Broadcast,arrPlayer[i]->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(pP2PPlayer->GetPlayerNetType()));
//			//发送接受者的转发添加通知
//			stP2PAddTransmitUserNotice.nRoleID = nP2PRoleID;
//			stP2PAddTransmitUserNotice.arrAddRoleID[0]=arrPlayer[i]->GetRoleID();
//			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stP2PAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_Broadcast,nP2PRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(arrPlayer[i]->GetPlayerNetType()));

			WRITE_NOTICE_LOG("redispatch media player and player p2pmic!{nRoleID=%d, nP2PRoleID=%d}\n",arrPlayer[i]->GetRoleID(),nP2PRoleID);
		}
	}
	//获得房间的公卖
	RoleID arrPublicMic[MaxPublicMicCount];
	uint16_t nPublicMicCount = 0;
	pRoom->GetOnPublicMic(arrPublicMic,MaxPublicMicCount,nPublicMicCount);
	for(int32_t j = 0;j < nPublicMicCount;j++)
	{
		//获取玩家
		CPlayer *pOnPublicPlayer = NULL;
		PlayerIndex nOnPublicPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(arrPublicMic[j], pOnPublicPlayer, nOnPublicPlayerIndex);
		if(ret < 0 || pOnPublicPlayer == NULL)
		{
			//没有次玩家
			WRITE_ERROR_LOG("player does no exist!{nPublicMic=%d}",arrPublicMic[j]);
			continue;
		}

//		SendPlayerOnMicToMedia(pRoom,pOnPublicPlayer);
		//公卖的转发
		SendAddTransmitUserNotifyToMediaServer(pOnPublicPlayer, arrPlayer, nPlayerCount, pRoom, true);

//		CAddTransmitUserNotice stAddTransmitUserNotice;
//		stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//		stAddTransmitUserNotice.nRoleID = arrPublicMic[j];
//		stAddTransmitUserNotice.nRoomID = pSetMediaInfoNoti->nRoomID;
//		stAddTransmitUserNotice.nAddCount = 0;
//		for(int32_t i = 0;i < nPlayerCount;++i)
//		{
//			if(NULL == arrPlayer[i])
//			{
//				continue;
//			}
//			if(arrPublicMic[j]!=arrPlayer[i]->GetRoleID()&&arrPlayer[i]->GetPlayerNetType()==enmNetType_Tel)
//			{
//				stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//				stAddTransmitUserNotice.nAddCount++;
//			}
//		}
//		//通知电信的转发（公卖上麦）
//		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_P2P,arrPublicMic[j],enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//		stAddTransmitUserNotice.nAddCount = 0;
//		for(int32_t i = 0;i < nPlayerCount;++i)
//		{
//			if(NULL==arrPlayer[i])
//			{
//				continue;
//			}
//			if(arrPublicMic[j]!=arrPlayer[i]->GetRoleID()&&arrPlayer[i]->GetPlayerNetType()==enmNetType_Cnc)
//			{
//				stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//				stAddTransmitUserNotice.nAddCount++;
//			}
//		}
//		//通知网通的转发（公卖上麦）
//		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_P2P,arrPublicMic[j],enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));
//		WRITE_NOTICE_LOG("redispatch media player in room on public mic{nRoleID=%d, nRoomID=%d]",arrPublicMic[j],pSetMediaInfoNoti->nRoomID);
	}

	//通知开始私麦的转发
	RoleID arrPrivateMic[MaxPrivateMicCount];
	uint16_t nPrivateMicCount = 0;
	pRoom->GetOnPrivateMic(arrPrivateMic,MaxPrivateMicCount,nPrivateMicCount);
	for(int32_t i = 0;i < nPrivateMicCount;i++)
	{
		//获取玩家
		CPlayer *pPlayer = NULL;
		PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
		ret = g_PlayerMgt.GetPlayer(arrPrivateMic[i], pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			//没有次玩家
			WRITE_ERROR_LOG("player does no exist!{nP2PRoleID=%d}",arrPrivateMic[i]);
			continue;
		}

//		SendPlayerOnMicToMedia(pRoom,pPlayer);

		RoleID arrWatchRoleID[MaxWatchPrivateMicCount];
		int32_t nWatchCount = 0;
		pPlayer->GetWatchPrivateMic(arrWatchRoleID,MaxWatchPrivateMicCount,nWatchCount,pSetMediaInfoNoti->nRoomID);
		//添加私麦的转发
		SendAddTransmitUserNotifyToMediaServer(arrPrivateMic[i], arrWatchRoleID, nWatchCount, pRoom, false);

//		for(int32_t j = 0;j < nWatchCount;j++)
//		{
//			//获取玩家
//			CPlayer *pDestPlayer = NULL;
//			PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
//			ret = g_PlayerMgt.GetPlayer(arrWatchRoleID[j], pDestPlayer, nDestPlayerIndex);
//			if(ret < 0 || pDestPlayer == NULL)
//			{
//				//没有次玩家
//				WRITE_ERROR_LOG("player does no exist!{nP2PRoleID=%d}",arrWatchRoleID[j]);
//				continue;
//			}
//			//发送通知给mediaserver
//			CAddTransmitUserNotice stPrivateAddTransmitUserNotice;
//
//			stPrivateAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//			stPrivateAddTransmitUserNotice.nRoleID = arrPrivateMic[i];
//			stPrivateAddTransmitUserNotice.nAddCount = 1;
//			stPrivateAddTransmitUserNotice.arrAddRoleID[0]=arrWatchRoleID[j];
//			stPrivateAddTransmitUserNotice.nRoomID = pSetMediaInfoNoti->nRoomID;
//
//			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stPrivateAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_Broadcast,arrPrivateMic[i],enmEntityType_Media);
////			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stPrivateAddTransmitUserNotice,pSetMediaInfoNoti->nRoomID,enmTransType_P2P,arrPrivateMic[i],enmEntityType_Media,pRoom->GetMediaServerIDByType(pDestPlayer->GetPlayerNetType()));
//			WRITE_NOTICE_LOG("redispatch media player watch private mic !{nRoleID=%d,nPrivateMic=%d]",arrWatchRoleID[j],arrPrivateMic[i],pSetMediaInfoNoti->nRoomID);
//		}
	}
	//通知玩家房间的mediaserver变了
	CReSetMediaNotify stReSetMediaNotify;
	stReSetMediaNotify.nRoomID = pSetMediaInfoNoti->nRoomID;
	stReSetMediaNotify.nTelMediaIP = pSetMediaInfoNoti->nTelMediaIP;
	stReSetMediaNotify.nTelMediaPort = pSetMediaInfoNoti->nTelMediaPort;
	stReSetMediaNotify.nCncMediaIP = pSetMediaInfoNoti->nCncMediaIP;
	stReSetMediaNotify.nCncMediaPort = pSetMediaInfoNoti->nCncMediaPort;
	SendMessageNotifyToClient(MSGID_RSCL_RESET_MEDIA_NOTI, &stReSetMediaNotify,pSetMediaInfoNoti->nRoomID, enmBroadcastType_All,
			enmInvalidRoleID,0,NULL, "send reset media notify");
	return S_OK;

}

//向Media发送上麦通知
void CBllBase::SendAddTransmitUserNotifyToMediaServer(RoleID nSpeakerID, RoleID arrReciverID[], int32_t nReciverCount, CRoom *pRoom, bool bIsPublicMic)
{
	if(pRoom == NULL)
	{
		return;
	}
	CPlayer *pSpeaker = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	g_PlayerMgt.GetPlayer(nSpeakerID, pSpeaker, nPlayerIndex);

	int32_t nExistReciverCount = 0;
	CPlayer * arrExistReciver[nReciverCount];

	int32_t ret;
	CPlayer *pPlayer = NULL;
	for(int32_t i = 0; i < nReciverCount; i++)
	{
		ret = g_PlayerMgt.GetPlayer(arrReciverID[i], pPlayer, nPlayerIndex);
		if(ret < 0 || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("player does no exist{nRoleID=%d}", arrReciverID[i]);
			continue;
		}
		arrExistReciver[nExistReciverCount] = pPlayer;
		nExistReciverCount++;
	}
	SendAddTransmitUserNotifyToMediaServer(pSpeaker, arrExistReciver, nExistReciverCount, pRoom, bIsPublicMic);
}

void CBllBase::SendAddTransmitUserNotifyToMediaServer(CPlayer* pSpeaker, CPlayer* arrReciver[], int32_t nReciverCount, CRoom *pRoom, bool bIsPublicMic)
{
	if(pSpeaker == NULL || pRoom == NULL)
	{
		return;
	}

	CAddTransmitUserNotice stAddTransmitUserNotice;
	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
	stAddTransmitUserNotice.nRoleID = pSpeaker->GetRoleID();
	stAddTransmitUserNotice.nRoomID = pRoom->GetRoomID();

	if(bIsPublicMic)
	{
		stAddTransmitUserNotice.arrAddRoleID[0] = pSpeaker->GetRoleID(); //上公麦SpeakerID与ReciverID一致即可
		stAddTransmitUserNotice.nAddCount = 1;
	}
	else
	{
		uint32_t nAddCount = 0;
		for(int32_t i = 0; i < nReciverCount; i++)
		{
			if(arrReciver[i] == NULL)
			{
				continue;
			}
			stAddTransmitUserNotice.arrAddRoleID[nAddCount] = arrReciver[i]->GetRoleID();
			nAddCount++;
		}
		stAddTransmitUserNotice.nAddCount = nAddCount;
	}

	//对主播直连的Media发送上麦通知
	bool nRet = pRoom->InsertMediaMap(pSpeaker->GetRoleID(), pSpeaker->GetConnInfo().nServerID, bIsPublicMic);
	if((nRet && bIsPublicMic) || !bIsPublicMic)
	{
		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI, &stAddTransmitUserNotice, enmInvalidRoomID,
				enmTransType_P2P, enmInvalidRoleID, enmEntityType_Media, pSpeaker->GetConnInfo().nServerID);
	}

	//对收看者直连的Media发送上麦通知
	for(int32_t i = 0; i < nReciverCount; i++)
	{
		if(arrReciver[i] == NULL)
		{
			continue;
		}

		nRet = pRoom->InsertMediaMap(pSpeaker->GetRoleID(), arrReciver[i]->GetConnInfo().nServerID, bIsPublicMic);
		if((nRet && bIsPublicMic) || !bIsPublicMic)
		{
			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI, &stAddTransmitUserNotice, enmInvalidRoomID,
					enmTransType_P2P, enmInvalidRoleID, enmEntityType_Media, arrReciver[i]->GetConnInfo().nServerID);
		}
	}
}

//主播下麦或者收看者关闭视频; 主播下麦nReciverCount填0
void CBllBase::SendDelTransmitUserNotifyToMediaServer(RoleID nSpeakerID, CRoom* pRoom, RoleID arrReciverID[], int32_t nReciverCount)
{
	CPlayer *pSpeaker = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t nRet = g_PlayerMgt.GetPlayer(nSpeakerID, pSpeaker, nPlayerIndex);
	if(nRet != S_OK || pSpeaker == NULL)
	{
		return;
	}

	CPlayer *pReciver;
	CPlayer* arrReciver[nReciverCount];
	for(int32_t i = 0; i < nReciverCount; i++)
	{
		g_PlayerMgt.GetPlayer(arrReciverID[i], pReciver, nPlayerIndex);
		arrReciver[i] = pReciver;
	}

	SendDelTransmitUserNotifyToMediaServer(pSpeaker, pRoom, arrReciver, nReciverCount);
}

//主播下麦或者收看者关闭视频; 主播下麦nReciverCount填0
void CBllBase::SendDelTransmitUserNotifyToMediaServer(CPlayer* pSpeaker, CRoom* pRoom, CPlayer* arrReciver[], int32_t nReciverCount)
{
	if(pSpeaker == NULL || pRoom == NULL)
	{
		return;
	}

	CDelTransmitUserNotice_Public stNotify;
	stNotify.nUserStatus = enmInvalidUserStatus;
	stNotify.nRoleID = pSpeaker->GetRoleID();
	stNotify.nRoomID = pRoom->GetRoomID();

	if(nReciverCount == 0) //主播下麦
	{
		stNotify.arrDelRoleID[0] = pSpeaker->GetRoleID();
		stNotify.nDelCount = 1;

		ServerSet stServerSet;
		pRoom->EraseMediaMap(pSpeaker->GetRoleID(), stServerSet);
		for(ServerSet::iterator it = stServerSet.begin(); it != stServerSet.end(); it++)
		{
			SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI, &stNotify, enmInvalidRoomID,
					enmTransType_P2P, enmInvalidRoleID, enmEntityType_Media, *it);
		}
	}
	else //玩家关闭私麦
	{
		if(nReciverCount != 1 || arrReciver[0] == NULL)
		{
			WRITE_ERROR_LOG("Close private mic error: nReciverCount=%d, PlayerPoint=%x\n", nReciverCount, arrReciver[0]);
			return;
		}

		stNotify.arrDelRoleID[0] = arrReciver[0]->GetRoleID();
		stNotify.nDelCount = 1;

		SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI, &stNotify, enmInvalidRoomID,
				enmTransType_P2P, enmInvalidRoleID, enmEntityType_Media, arrReciver[0]->GetConnInfo().nServerID);

//		//整理要发送的Mediaserver
//		MediaserverMap stMediaMap;
//		ServerID nMediaID;
//		for(int32_t i = 0; i < nReciverCount; i++)
//		{
//			if(arrReciver[i] == NULL || arrReciver[i]->GetRoleID() == pSpeaker->GetRoleID())
//			{
//				continue;
//			}
//			nMediaID = arrReciver[i]->GetConnInfo().nServerID;
//			stMediaMap[nMediaID].insert(arrReciver[i]->GetRoleID());
//		}
//
//		for(MediaserverMap::iterator it = stMediaMap.begin(); it != stMediaMap.end(); it++)
//		{
//			int32_t nCount = 0;
//			for(ServerSet::iterator set_it = it->second.begin(); set_it != it->second.end(); set_it++)
//			{
//				stNotify.arrDelRoleID[nCount] = *set_it;
//				nCount++;
//			}
//			stNotify.nDelCount = nCount;
//
//			if(pRoom->EraseMediaMap(pSpeaker->GetRoleID(), it->first))
//			{
//				SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI, &stNotify, enmInvalidRoomID,
//						enmTransType_P2P, enmInvalidRoleID, enmEntityType_Media, it->first);
//			}
//		}
	}
}

FRAME_ROOMSERVER_NAMESPACE_END

