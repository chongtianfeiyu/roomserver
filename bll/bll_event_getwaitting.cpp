/*
 *  bll_event_getwaitting.cpp
 *
 *  To do：
 *  Created on: 	2012-2-9
 *  Author: 		luocj
 */

#include "bll_event_getwaitting.h"
#include "ctl/program_mgt.h"
#include "ctl/song_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CGetWaittingEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CGetWaittingEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CGetWaittingEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_GETWAITTINGLIST_REQ:
		OnEventGetWaitting(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CGetWaittingEvent::OnEventGetWaitting(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player get waiting list req {nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
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
		WRITE_ERROR_LOG("get room error!{ret=0x%08x} \n",ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//发送回应道客户端
	SendResponseToUser(pMsgHead,pRoom,nOptionLen,pOptionData);

	ret = SrvPrivateShow(pRoom,pPlayer);
	if(ret<0)
	{
		WRITE_NOTICE_LOG("player srv private mic filed!{nRoleID=%d}\n",pPlayer->GetRoleID());
	}

	//发送节目信息
//	SendProgramInfo(pMsgHead, pRoom);

	return ret;
}

/*
 * 发送响应到客户端
 */
int32_t CGetWaittingEvent::SendResponseToUser(MessageHeadSS *pMsgHead,CRoom *pRoom,const uint16_t nOptionLen , const void *pOptionData)
{
	CGetWaittingListResp stGetWaittingListResp;
	RoleID arrWaittingMic[MaxWaittingMicCount];
	uint16_t nWaittingMicCount;
	pRoom->GetWaittingMic(arrWaittingMic,MaxWaittingMicCount,nWaittingMicCount);
	stGetWaittingListResp.nWaittingCount = nWaittingMicCount;
	for(uint16_t i = 0;i < nWaittingMicCount;i++)
	{
		stGetWaittingListResp.arrWaittingList[i] = arrWaittingMic[i];
	}
	SendMessageResponse(MSGID_RSCL_GETWAITTINGLIST_RESP, pMsgHead, &stGetWaittingListResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

/*
 * 进房分配私麦
 */
int32_t CGetWaittingEvent::SrvPrivateShow(CRoom *pRoom, CPlayer *pPlayer,const uint16_t nOptionLen , const void *pOptionData )
{
	if(NULL == pRoom || NULL == pPlayer)
	{
		WRITE_ERROR_LOG("pRoom==NULL or pPlayer==NULL [ret=0x%08x]\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	int32_t ret = S_OK;
	VipLevel nVipLevel = pPlayer->GetVipLevel();
	CVipInfo nVipInfo = g_SomeConfig.GetVipInfo(nVipLevel);
	uint16_t nMaxWatchPrivateMicCount = nVipInfo.nMaxWatchCount;
	if(nMaxWatchPrivateMicCount <= 0)
	{
		WRITE_DEBUG_LOG(" player can not watch private mic{nRoleID=%d, nVipLevel=%d}",pPlayer->GetRoleID(),nVipLevel);
		return S_OK;
	}
	uint16_t nPrivateMicCount = 0;
	RoleID arrPrivateMic[MaxPrivateMicCount] = {0}; //正在私麦上的玩家
	pRoom->GetOnPrivateMic(arrPrivateMic,MaxPrivateMicCount,nPrivateMicCount);
	//没有可以分配的
	if(nPrivateMicCount <= 0)
	{
		return S_OK;
	}
//	//通知mediaserver
//	CAddTransmitUserNotice stAddTransmitUserNotice;
//	stAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stAddTransmitUserNotice.nAddCount = 1;
//	stAddTransmitUserNotice.nRoomID = pRoom->GetRoomID();
//	stAddTransmitUserNotice.arrAddRoleID[0] = pPlayer->GetRoleID();

	RoleID arrReciverID[1] = {pPlayer->GetRoleID()};

	//通知客户端的消息
	CSrvPrivateMicNotify stSrvPrivateMicNotify;
	if(nPrivateMicCount <= nMaxWatchPrivateMicCount)
	{
		stSrvPrivateMicNotify.nCount = nPrivateMicCount;
		for(int32_t i = 0;i < nPrivateMicCount;i++)
		{
			ret = pPlayer->AddWatchPrivateMic(arrPrivateMic[i],pRoom->GetRoomID());
			if(ret<0)
			{
				WRITE_NOTICE_LOG("player add watch private mic filed {nRoleID=%d, ret=0x%08x}\n",pPlayer->GetRoleID(),ret);
				return ret;
			}

			g_BllBase.SendAddTransmitUserNotifyToMediaServer(arrPrivateMic[i], arrReciverID, 1, pRoom, false);
//			stAddTransmitUserNotice.nRoleID = arrPrivateMic[i];
//			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_Broadcast,arrPrivateMic[i],enmEntityType_Media,pRoom->GetMediaServerIDByType(pPlayer->GetPlayerNetType()));
			stSrvPrivateMicNotify.arrPrivateMicRoleID[i] = arrPrivateMic[i];
			WRITE_DEBUG_LOG(" player watch private mic{nSrcRoleID=%d, nDestRoleID=%d} ",pPlayer->GetRoleID(),arrPrivateMic[i]);
		}
	}
	else
	{
		for(int32_t i = 0;i < nMaxWatchPrivateMicCount;i++)
		{
			//当现有私麦的数量大于玩家可以观看的个数时随机
			int32_t nIndex = Random(nPrivateMicCount-1);
			if(nIndex<0)
			{
				nIndex = -nIndex;
			}
			stSrvPrivateMicNotify.nCount++;
			ret = pPlayer->AddWatchPrivateMic(arrPrivateMic[nIndex],pRoom->GetRoomID());
			if(ret<0)
			{
				WRITE_NOTICE_LOG("player %d add watch private mic filed ret=0x%08x!\n",pPlayer->GetRoleID(),ret);
				return ret;
			}

			g_BllBase.SendAddTransmitUserNotifyToMediaServer(arrPrivateMic[nIndex], arrReciverID, 1, pRoom, false);
//			stAddTransmitUserNotice.nRoleID = arrPrivateMic[nIndex];
//			SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_Broadcast,arrPrivateMic[nIndex],enmEntityType_Media,pRoom->GetMediaServerIDByType(pPlayer->GetPlayerNetType()));
			stSrvPrivateMicNotify.arrPrivateMicRoleID[i] = arrPrivateMic[nIndex];
			//删除已经分配过的私麦
			arrPrivateMic[nIndex] = arrPrivateMic[nPrivateMicCount-1];
			arrPrivateMic[nPrivateMicCount-1] = enmInvalidRoleID;
			nPrivateMicCount--;
			WRITE_DEBUG_LOG(" player watch private mic{nSrcRoleID=%d, nDestRoleID=%d} ",pPlayer->GetRoleID(),arrPrivateMic[nIndex]);
		}
	}
	//给客户端发通知
	SendMessageNotifyToClient(MSGID_RSCL_SRV_PRIVATE_MIC_NOTI, &stSrvPrivateMicNotify, pRoom->GetRoomID(), enmBroadcastType_ExpectPlayr,
			pPlayer->GetRoleID(),nOptionLen,(char*)pOptionData, "send watch private mic notify");
	return ret;

}

int32_t CGetWaittingEvent::SendProgramInfo(MessageHeadSS * pMsgHead, CRoom *pRoom)
{
	SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(pMsgHead->nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_ERROR_LOG("it's not found song program info!{RoomID=%d, nRoleID=%d}\n", pMsgHead->nRoomID, pMsgHead->nRoleID);
		return S_OK;
	}

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pMsgHead->nRoomID, pSongProgramInfo->nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("it's not found song order info!{nRoomID=%d, nRoleID=%d}\n", pMsgHead->nRoomID, pMsgHead->nRoleID);
		return S_OK;
	}

	if(pSongProgramInfo->nProgramStatus != enmProgramStatus_WaitingForStart)
	{
		//发送当前节目信息
		CCurSongInfoNoti stCurSongInfoNoti;
		stCurSongInfoNoti.nOrderID = pSongOrderInfo->nOrderID;
		stCurSongInfoNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
		stCurSongInfoNoti.strSongerName = pSongOrderInfo->strSongerName;
		stCurSongInfoNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
		stCurSongInfoNoti.strRequestName = pSongOrderInfo->strRequestName;
		stCurSongInfoNoti.strSongName = pSongOrderInfo->strSongName;

		SendMessageNotifyToClient(MSGID_RSCL_CURSONGINFO_NOTI, &stCurSongInfoNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, pMsgHead->nRoleID);
	}

	switch(pSongProgramInfo->nProgramStatus)
	{
	case enmProgramStatus_FightingForTicket:
	{
		CTicketCountUpdateNoti stTicketCountUpdateNoti;
		stTicketCountUpdateNoti.nOrderID = pSongOrderInfo->nOrderID;
		stTicketCountUpdateNoti.nTotalTicketCount = pSongProgramInfo->nTotalTicketCount;
		stTicketCountUpdateNoti.nLeftTicketCount = pSongProgramInfo->nTotalTicketCount - pSongProgramInfo->nHaveTicketPlayerCount;
		stTicketCountUpdateNoti.nEndLeftTime = pSongProgramInfo->nStatusEndTime - CDateTime::CurrentDateTime().Seconds();
		stTicketCountUpdateNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pMsgHead->nRoomID, pMsgHead->nRoleID);
		//如果是点歌者
		if(pMsgHead->nRoleID == pSongProgramInfo->nRequesterRoleID)
		{
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		//如果是大众评审
		else if(pSongProgramInfo->IsHaveTicket(pMsgHead->nRoleID))
		{
			//stTicketCountUpdateNoti.nHaveTicket = 1;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		//如果是歌手或者是室主
		else if((pMsgHead->nRoleID == pSongOrderInfo->nSongerRoleID) ||
				(pMsgHead->nRoleID == pRoom->GetHostID()))
		{
			//stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		else
		{
			//stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = (pSongProgramInfo->nTotalTicketCount <= pSongProgramInfo->nHaveTicketPlayerCount ? 0 : 1);
		}
		SendMessageNotifyToClient(MSGID_RSCL_TICKETCOUNTUPDATE_NOTI, &stTicketCountUpdateNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, pMsgHead->nRoleID);
	}
		break;
	case enmProgramStatus_Enjoying:
	{
		CEnjoyingNoti stEnjoyingNoti;
		stEnjoyingNoti.nOrderID = pSongOrderInfo->nOrderID;
		stEnjoyingNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pMsgHead->nRoomID, pMsgHead->nRoleID);
		SendMessageNotifyToClient(MSGID_RSCL_ENJOYING_NOTI, &stEnjoyingNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, pMsgHead->nRoleID);
	}
		break;
//	case enmProgramStatus_Voteing:
//	{
//		CSongVoteUpdateNoti stSongVoteUpdateNoti;
//		stSongVoteUpdateNoti.nOrderID = pSongOrderInfo->nOrderID;
//		stSongVoteUpdateNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pMsgHead->nRoomID, pMsgHead->nRoleID);;
//		//如果是点歌者
//		if(pMsgHead->nRoleID == pSongProgramInfo->nRequesterRoleID)
//		{
//			stSongVoteUpdateNoti.nHaveMark = (pSongProgramInfo->IsVoted(pMsgHead->nRoleID) ? 1 : 0);
//		}
//		//如果是大众评审
//		else if(pSongProgramInfo->IsHaveTicket(pMsgHead->nRoleID))
//		{
//			//stSongVoteUpdateNoti.nHaveTicket = 1;
//			stSongVoteUpdateNoti.nHaveMark = (pSongProgramInfo->IsVoted(pMsgHead->nRoleID) ? 1 : 0);
//		}
//		else
//		{
//			//stSongVoteUpdateNoti.nHaveTicket = 0;
//			stSongVoteUpdateNoti.nHaveMark = 0;
//		}
//
//		stSongVoteUpdateNoti.nMarkEnd = 0;
//		stSongVoteUpdateNoti.nRequesterGoodCount = pSongProgramInfo->nRequesterGoodCount;
//		stSongVoteUpdateNoti.nRequesterBadCount = pSongProgramInfo->nRequesterBadCount;
//		stSongVoteUpdateNoti.nRequesterGiveupCount = pSongProgramInfo->nRequesterGiveupCount;
//		stSongVoteUpdateNoti.nGerneralGoodCount = pSongProgramInfo->nGerneralGoodCount;
//		stSongVoteUpdateNoti.nGerneralBadCount = pSongProgramInfo->nGerneralBadCount;
//		stSongVoteUpdateNoti.nGerneralGiveupCount = pSongProgramInfo->nGerneralGiveupCount;
//		stSongVoteUpdateNoti.nEndLeftTime = pSongProgramInfo->nStatusEndTime - CDateTime::CurrentDateTime().Seconds();
//		SendMessageNotifyToClient(MSGID_RSCL_SONGVOTEUPDATE_NOTI, &stSongVoteUpdateNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, pMsgHead->nRoleID);
//	}
//		break;
//	case enmProgramStatus_ShowResult:
//	{
//		CSongVoteUpdateNoti stSongVoteUpdateNoti;
//		stSongVoteUpdateNoti.nOrderID = pSongOrderInfo->nOrderID;
//		stSongVoteUpdateNoti.nHaveTicket = 0;
//		stSongVoteUpdateNoti.nHaveMark = 0;
//		stSongVoteUpdateNoti.nMarkEnd = 1;
//		stSongVoteUpdateNoti.nRequesterGoodCount = pSongProgramInfo->nRequesterGoodCount;
//		stSongVoteUpdateNoti.nRequesterBadCount = pSongProgramInfo->nRequesterBadCount;
//		stSongVoteUpdateNoti.nRequesterGiveupCount = pSongProgramInfo->nRequesterGiveupCount;
//		stSongVoteUpdateNoti.nGerneralGoodCount = pSongProgramInfo->nGerneralGoodCount;
//		stSongVoteUpdateNoti.nGerneralBadCount = pSongProgramInfo->nGerneralBadCount;
//		stSongVoteUpdateNoti.nGerneralGiveupCount = pSongProgramInfo->nGerneralGiveupCount;
//		SendMessageNotifyToClient(MSGID_RSCL_SONGVOTEUPDATE_NOTI, &stSongVoteUpdateNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr, pMsgHead->nRoleID);
//	}
//		break;
	default:
		break;
	}

	return S_OK;
}


FRAME_ROOMSERVER_NAMESPACE_END
