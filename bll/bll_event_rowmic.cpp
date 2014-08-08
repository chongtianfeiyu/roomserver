/*
 *  bll_event_rowmic.cpp
 *
 *  To do：
 *  Created on: 	2011-12-26
 *  Author: 		luocj
 */

#include "bll_event_rowmic.h"
#include "requestsong/bll_timer_clearroomorderinfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CRowMicEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CRowMicEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CRowMicEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_REQUESTSHOW_REQ:
		OnEventRequestShow(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		case MSGID_CLRS_EXITSHOW_REQ:
		OnEventExitShow(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * 排麦请求（来自客户端）
 */
int32_t CRowMicEvent::OnEventRequestShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有次玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	CRequestShowReq *pRequestShowReq = dynamic_cast<CRequestShowReq *>(pMsgBody);
	if(pRequestShowReq==NULL)
	{
		WRITE_ERROR_LOG("rowmic:pRequestShowReq null{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rowmic:rcev player req{nRoleID=%d, nRoomID=%d,nRequestShowType=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pRequestShowReq->nRequestShowType);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d,ret=0x%08x}\n",ret);
		return S_OK;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	RoleRank nRoleRank = pPlayer->GetRoleRankInRoom(pMsgHead->nRoomID);
	//获取房间状态
	PlayerState nPlayerState = pPlayer->GetStateInRoom(pMsgHead->nRoomID);
	bool bCanNotRowMic = pPlayer->IsOnMicInRoom(pMsgHead->nRoomID)||(nPlayerState==enmPlayerState_PublicMicQueue);
	if(bCanNotRowMic)
	{
		SendRespToClient(pMsgHead,enmRequestShowResult_IsRowShowed,nOptionLen,pOptionData);
		WRITE_NOTICE_LOG("rowmic:player cannot rowMic{nRoleID=%d, nPlayerState=%d}",pPlayer->GetRoleID(),nPlayerState);
		return ret;
	}
	bool isManager = (nRoleRank > enmRoleRank_TempAdmin);
	//房间私麦是否打开
	if(pRoom->IsPrivateMicOpened())
	{
		if(isManager)
		{
			switch(pRequestShowReq->nRequestShowType)
			{
				//直接到后面
				case enmRequestShowType_Public:
				break;
				//直接上私麦
				case enmRequestShowType_Private:
				{
					if(pPlayer->IsOnMic())
					{
						WRITE_NOTICE_LOG("rowmic:player on mic in other room{nRoleID=%d}!\n",pPlayer->GetRoleID());
						SendRespToClient(pMsgHead,enmRequestShowResult_IsOnMicInOhterRoom,nOptionLen,pOptionData);
						return ret;
					}
					ret = PushOnPrivateMic(pMsgHead,pPlayer,pRoom,nOptionLen,pOptionData);
					return ret;
				}
				break;
				default:
				{
					//返回请选择公麦或者私麦的回应
					SendRespToClient(pMsgHead,enmRequestShowResult_Need_Select,nOptionLen,pOptionData);
					WRITE_NOTICE_LOG("rowmic:player need select private or public mic{nRoleID=%d}",pPlayer->GetRoleID());
					return ret;
				}
				break;
			}
		}
	}
	if(isManager && !(pRoom->IsPrivateMicOpened()) && (pRequestShowReq->nRequestShowType == enmRequestShowType_Private))
	{
		SendRespToClient(pMsgHead,enmRequestShowResult_Private_Closed,nOptionLen,pOptionData);
		WRITE_NOTICE_LOG("rowmic:room close private mic or has not private mic{nRoomID=%d}",pRoom->GetRoomID());
		return ret;
	}
	//这里控制是从那个判断走到这里的判断自由排麦
	if(!isManager && !(pRoom->IsFreeShowOpened()))
	{
		//排麦序,发回应通知
		ret = PushOnWaittingMic(pMsgHead,pPlayer,pRoom,false,nOptionLen,pOptionData);
		if(ret>=0)
		{
			WRITE_NOTICE_LOG("rowmic:player is not mamager and free show chose so go to waitting {nRoleID=%d, nRoomID=%d, nRoleRank=%d}\n",pPlayer->GetRoleID(),pRoom->GetRoomID(),nRoleRank);
		}
		return ret;
	}
	//麦上有没有空位
	if(pRoom->HasPublicMicIndex())
	{
		if(pPlayer->IsOnMic())
		{
			ret = PushOnWaittingMic(pMsgHead,pPlayer,pRoom,pRoom->IsFreeShowOpened(),nOptionLen,pOptionData);
			if(ret>=0)
			{
				WRITE_NOTICE_LOG("rowmic:player on mic in other room so go to waitting {nRoleID=%d}\n",pPlayer->GetRoleID());
			}
			return ret;
		}
		ret = PushOnPublicMic(pMsgHead,pPlayer,pRoom,nOptionLen,pOptionData);
	}
	else
	{
		//排麦序,发回应通知
		ret = PushOnWaittingMic(pMsgHead,pPlayer,pRoom,pRoom->IsFreeShowOpened(),nOptionLen,pOptionData);
		if(ret>=0)
		{
			WRITE_NOTICE_LOG("rowmic:room has not micIndex so player go to waitting{nRoomID=%d, nRoleID=%d}\n",pRoom->GetRoomID(),pPlayer->GetRoleID());
		}
	}

	return ret;
}

/*
 * 下麦请求（来自客户端）
 */
int32_t CRowMicEvent::OnEventExitShow(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CExitShowReq *pExitShowReq = dynamic_cast<CExitShowReq *>(pMsgBody);
	if(pExitShowReq == NULL)
	{
		WRITE_ERROR_LOG("rowmic:pExitShowReq null{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("exit show:rcev player req{nRoleID=%d, nRoomID=%d, nExitShowType=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pExitShowReq->nExitShowType);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有次玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID,ret);
		return S_OK;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	return ExitShow(pPlayer,pRoom,true,pMsgHead->nMessageID,nOptionLen,pOptionData,pExitShowReq->nExitShowType);
}
int32_t CRowMicEvent::PushOnPrivateMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgHead == NULL || pPlayer == NULL || pRoom == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pPlayer=0x%08x, pRoom=NULL}\n",pMsgHead,pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	if(!(pPlayer->IsStateInRoom(enmPlayerState_PublicMic,pRoom->GetRoomID())&&pPlayer->IsStateInRoom(enmPlayerState_P2PMic,pRoom->GetRoomID())))
	{
		RequestShowResult nRequestShowResult;
		ret = pPlayer->SetStateInRoom(enmPlayerState_PrivateMic,pRoom->GetRoomID());
		if(ret<0)
		{
			WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			return ret;
		}
		else
		{
			ret = pRoom->AddOnPrivateMic(pMsgHead->nRoleID);
			nRequestShowResult = enmRequestShowResult_Private;
		}
		if(ret == (int32_t)E_RS_PRIVATEMICISFULL)
		{
			WRITE_NOTICE_LOG("rowmic:private mic is full in room{nRoomID=%d}",pRoom->GetRoomID());
			nRequestShowResult = enmRequestShowResult_Full;
		}
		if(ret == (int32_t)E_RS_INPRIVATEMIC)
		{
			WRITE_ERROR_LOG("rowmic:player is On private mic{nRoleID=%d}",pPlayer->GetRoleID());
			nRequestShowResult = enmRequestShowResult_IsRowShowed;
		}

		SendRespToClient(pMsgHead,nRequestShowResult,nOptionLen,pOptionData);
		if(nRequestShowResult == enmRequestShowResult_Full || nRequestShowResult == enmRequestShowResult_IsRowShowed)
		{
			pPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
			return S_OK;
		}
		//给房间的所有人发通知
		SendRequestShowNotify(enmRequestShowResult_Private,pRoom->GetRoomID(),pPlayer->GetRoleID(),nOptionLen,pOptionData);
		WRITE_NOTICE_LOG("rowmic:player pushOn  privatemic{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
		pPlayer->SetPlayerGlobalState(enmPlayerState_PrivateMic);

		//通知media上麦
		SendPlayerOnMicToMedia(pRoom,pPlayer);
		SendPlayerMicStatusToDB(pRoom,pPlayer,enmMicOperateType_Private_Up,pMsgHead->nMessageID,nRequestShowResult);
		//发送上麦通知到hallserver
		COnMicNoti stOnMicNoti;
		stOnMicNoti.nOnMicRoleID = pPlayer->GetRoleID();
		stOnMicNoti.nMicType = enmMicType_PrivateMic;
		SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);
		return ret;
	}
	//发送错误回应
	SendRespToClient(pMsgHead,enmRequestShowResult_Fall,nOptionLen,pOptionData);
	WRITE_NOTICE_LOG("rowmic:player request show fall{nRoleID=%d,nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
	return ret;
}

int32_t CRowMicEvent::PushOnPublicMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgHead == NULL || pPlayer == NULL || pRoom == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pPlayer=0x%08x, pRoom=NULL}\n",pMsgHead,pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	if(!(pPlayer->IsStateInRoom(enmPlayerState_PrivateMic,pRoom->GetRoomID())&&pPlayer->IsStateInRoom(enmPlayerState_P2PMic,pRoom->GetRoomID())))
	{
		uint8_t nMicIndex = 0;
		RequestShowResult nRequestShowResult;
		ret = pPlayer->SetStateInRoom(enmPlayerState_PublicMic,pRoom->GetRoomID());
		if(ret<0)
		{
			WRITE_ERROR_LOG("rowmic:player does no in room{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			return ret;
		}
		else
		{
			ret = pRoom->AddOnPublicMic(pMsgHead->nRoleID,nMicIndex);
			nRequestShowResult = enmRequestShowResult_Public;
		}
		if(ret == (int32_t)E_RS_PUBLICINDEXISEMPTY)
		{
			WRITE_ERROR_LOG("rowmic:public mic is full in room{nRoomID=%d}",pRoom->GetRoomID());
			nRequestShowResult = enmRequestShowResult_Full;
		}
		if(ret == (int32_t)E_RS_INPUBLICMIC)
		{
			WRITE_ERROR_LOG("rowmic:player is on public mic{nRoleID=%d}",pPlayer->GetRoleID());
			nRequestShowResult = enmRequestShowResult_IsRowShowed;
		}
		RoleRank nRoleRank = pRoom->GetRoleRank(pPlayer->GetRoleID());
		uint32_t nOnMicTime = pRoom->GetMicTimeByRoleRank(nRoleRank);
		//给客户端发回应
		SendRespToClient(pMsgHead,nRequestShowResult,nOptionLen,pOptionData,nMicIndex,nOnMicTime);
		if(nRequestShowResult == enmRequestShowResult_Full || nRequestShowResult == enmRequestShowResult_IsRowShowed)
		{
			pPlayer->SetStateInRoom(enmPlayerState_InRoom,pRoom->GetRoomID());
			return S_OK;
		}
		ret = pPlayer->SetPublicMicIndexInRoom(nMicIndex,pRoom->GetRoomID());
		if(ret<0)
		{
			return ret;
		}
		//给房间的所有人发通知
		SendRequestShowNotify(enmRequestShowResult_Public,pRoom->GetRoomID(),pPlayer->GetRoleID(),nOptionLen,pOptionData,nMicIndex,nOnMicTime);
		WRITE_NOTICE_LOG("rowmic:player pushOn  public mic{nRoleID=%d, nRoomID=%d, nMicIndex=%d, nOnMicTime=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID(),nMicIndex,nOnMicTime);

		//发送添加转发通知给mediaserver
		SendPlayerOnMicToMedia(pRoom,pPlayer);
		SendPlayerMicStatusToDB(pRoom,pPlayer,enmMicOperateType_Public_Up,pMsgHead->nMessageID,nRequestShowResult);

		ret = SendNotifyToMediaServer(pMsgHead,pRoom);
		pPlayer->SetPlayerGlobalState(enmPlayerState_PublicMic);

		//删除歌手的订单计时器
		g_ClearRoomOrderInfo.DeleteTimer(pPlayer->GetRoleID());

		//发送上麦通知到hallserver
		COnMicNoti stOnMicNoti;
		stOnMicNoti.nOnMicRoleID = pPlayer->GetRoleID();
		stOnMicNoti.nMicType = enmMicType_PublicMic;
		SendMessageNotifyToServer(MSGID_RSHS_ONMIC_NOTIFY, &stOnMicNoti, pRoom->GetRoomID(), enmTransType_Broadcast, pPlayer->GetRoleID(), enmEntityType_Hall);
		return ret;
	}
	//发送错误回应
	SendRespToClient(pMsgHead,enmRequestShowResult_Fall,nOptionLen,pOptionData);
	WRITE_NOTICE_LOG("rowmic:player requestShow fall{nRoleID=%d}",pPlayer->GetRoleID());
	return ret;
}

int32_t CRowMicEvent::PushOnWaittingMic(MessageHeadSS *pMsgHead,CPlayer *pPlayer,CRoom *pRoom,bool isFreeOpened,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgHead == NULL || pPlayer == NULL || pRoom == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pPlayer=0x%08x, pRoom=NULL}\n",pMsgHead,pPlayer,pRoom);
		return E_NULLPOINTER;
	}
	if(!(pPlayer->IsStateInRoom(enmPlayerState_PublicMic,pRoom->GetRoomID())&&pPlayer->IsStateInRoom(enmPlayerState_P2PMic,pRoom->GetRoomID())
					&&pPlayer->IsStateInRoom(enmPlayerState_PrivateMic,pRoom->GetRoomID())))
	{
		PlayerState nTempPlayerState = pPlayer->GetStateInRoom(pRoom->GetRoomID());

		RequestShowResult nRequestShowResult;
		uint8_t nFreeShowOpened;
		ret = pPlayer->SetStateInRoom(enmPlayerState_PublicMicQueue,pRoom->GetRoomID());
		if(ret < 0)
		{
			WRITE_ERROR_LOG("rowmic:player does no in room{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			return ret;
		}
		else
		{
			ret = pRoom->AddToWaittingMic(pPlayer->GetRoleID());
			nRequestShowResult = enmRequestShowResult_Waittinglist;
			if(pPlayer->IsOnMic())
			{
				nRequestShowResult = enmRequestShowResult_WaittingInOtherRoomMic;
			}
		}
		if(ret < 0)
		{
			WRITE_NOTICE_LOG("rowmic:waitting is full in room{nRoomID=%d}",pRoom->GetRoomID());
			nRequestShowResult = enmRequestShowResult_Full;
		}
		if(isFreeOpened)
		{
			nFreeShowOpened = enmFreeShowOpenedType_Open;
		}
		else
		{
			nFreeShowOpened = enmFreeShowOpenedType_Close;
		}
		//发送回应
		SendRespToClient(pMsgHead, nRequestShowResult,nOptionLen,pOptionData, 0,0,nFreeShowOpened);

		if(nRequestShowResult == enmRequestShowResult_Full)
		{
			pPlayer->SetStateInRoom(nTempPlayerState,pRoom->GetRoomID());
			return S_OK;
		}
		//给房间的所有人发通知
		SendRequestShowNotify(enmRequestShowResult_Waittinglist,pRoom->GetRoomID(),pPlayer->GetRoleID(),nOptionLen,pOptionData);
		WRITE_NOTICE_LOG("rowmic:player pushOn waitting list{nRoleID=%d}",pPlayer->GetRoleID());
		return ret;
	}
	//发送错误回应
	SendRespToClient(pMsgHead, enmRequestShowResult_Fall,nOptionLen,pOptionData);
	WRITE_NOTICE_LOG("rowmic:player %d RequestShow fall",pPlayer->GetRoleID());
	return ret;
}

int32_t CRowMicEvent::SendNotifyToMediaServer(MessageHeadSS *pMsgHead,CRoom *pRoom)
{
	if(NULL == pMsgHead || NULL == pRoom)
	{
		return S_OK;
	}
	int32_t ret = S_OK;
	CPlayer *arrPlayer[MaxUserCountPerRoom] = {NULL};
	int32_t nPlayerCount = 0;
	ret = g_DataCenter.GetAllPlayers(pMsgHead->nRoomID, arrPlayer, MaxUserCountPerRoom, nPlayerCount);
	if(ret < 0)
	{
		WRITE_ERROR_LOG("get room players object error{RoomID=%d, ret=0x%08x}", pMsgHead->nRoomID, ret);
		return ret;
	}

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}

	//发送上麦通知消息到mediaServer
	g_BllBase.SendAddTransmitUserNotifyToMediaServer(pPlayer, arrPlayer, nPlayerCount, pRoom, true);
	return S_OK;

//	CAddTransmitUserNotice stAddTransmitUserNotice;
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nRoleID = pMsgHead->nRoleID;
//	stAddTransmitUserNotice.nRoomID = pMsgHead->nRoomID;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.arrAddRoleID[0] = pMsgHead->nRoleID;
//	for(int32_t i = 0;i < nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//			continue;
//		}
//		if(pMsgHead->nRoleID != arrPlayer[i]->GetRoleID())
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_Broadcast,pMsgHead->nRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//	stAddTransmitUserNotice.nAddCount = 0;
//	for(int32_t i = 0;i < nPlayerCount;++i)
//	{
//		if(NULL == arrPlayer[i])
//		{
//			continue;
//		}
//		if(pMsgHead->nRoleID != arrPlayer[i]->GetRoleID() && arrPlayer[i]->GetPlayerNetType() == enmNetType_Cnc)
//		{
//			stAddTransmitUserNotice.arrAddRoleID[stAddTransmitUserNotice.nAddCount]=arrPlayer[i]->GetRoleID();
//			stAddTransmitUserNotice.nAddCount++;
//		}
//	}
//	SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pMsgHead->nRoomID,enmTransType_Broadcast,pMsgHead->nRoleID,enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));
//	return ret;
}

int32_t CRowMicEvent::SendRespToClient(MessageHeadSS *pMsgHead,RequestShowResult nRequestShowResult,const uint16_t nOptionLen , const void *pOptionData,uint8_t nIndex,uint32_t nTime,uint8_t nFreeShowOpened )
{
	int32_t ret = S_OK;
	CRequestShowResp stRequestShowResp;
	stRequestShowResp.nRequestShowResult = nRequestShowResult;
	stRequestShowResp.nIndex = nIndex;
	stRequestShowResp.nTime = (uint16_t)nTime;
	stRequestShowResp.nFreeShowOpened = nFreeShowOpened;
	SendMessageResponse(MSGID_RSCL_REQUESTSHOW_RESP, pMsgHead, &stRequestShowResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return ret;
}

int32_t CRowMicEvent::SendRequestShowNotify(RequestShowResult nRequestShowResult,RoomID nRoomID,RoleID nRoleID,const uint16_t nOptionLen , const void *pOptionData,uint8_t nIndex/*=0*/,uint32_t nTime /*= 0*/)
{
	int32_t ret = S_OK;
	CRequestShowNoti stRequestShowNoti;
	stRequestShowNoti.nRequestShowResult = nRequestShowResult;
	stRequestShowNoti.nSrcRoleID = nRoleID;
	stRequestShowNoti.nIndex = nIndex;
	stRequestShowNoti.nTime = (uint16_t)nTime;
	SendMessageNotifyToClient(MSGID_RSCL_REQUESTSHOW_NOTI, &stRequestShowNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nRoleID,nOptionLen,(char*)pOptionData, "send req show notify");
	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END

