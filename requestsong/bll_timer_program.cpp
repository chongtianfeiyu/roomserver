/*
 * bll_timer_program.cpp
 *
 *  Created on: 2013-3-23
 *      Author: jimm
 */

#include "bll_timer_program.h"
#include "ctl/server_datacenter.h"
#include "public_message_define.h"
#include "dal/to_client_message.h"
#include "ctl/program_mgt.h"
#include "ctl/song_mgt.h"
#include "config/someconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//消息事件
int32_t CProgramTimer::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}

//会话事件激活
int32_t CProgramTimer::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	switch(pSession->GetSessionType())
	{
	default:
		break;
	}
	return S_OK;
}

int32_t CProgramTimer::OnSessionTimeOut(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	switch(pSession->GetSessionType())
	{
	case enmSessionType_FightForTicket:
		FightForTicketTimeout(pSession);
		break;
	case enmSessionType_EnjoyProgram:
		EnjoyProgramTimeout(pSession);
		break;
	case enmSessionType_Voteing:
		VoteingTimeout(pSession);
		break;
	case enmSessionType_ShowResult:
		ShowResultTimeout(pSession);
		break;
	default:
		break;
	}

	return nRet;
}

int32_t CProgramTimer::FightForTicketTimeout(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CProgramSessionData * pData =(CProgramSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("stop fight for ticket!{RoomID=%d, OrderID=%u}\n", pData->nRoomID, pData->nOrderID);

	//删除定时器的信息
	DeleteProgramTimerInfo(pData->nRoomID, pData->nOrderID);

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pData->nRoomID, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	//开始欣赏通知
	CEnjoyingNoti stEnjoyingNoti;
	stEnjoyingNoti.nOrderID = pData->nOrderID;

	for(int32_t i = 0; i < nPlayerCount; ++i)
	{
		RoleID nRoleID = arrPlayer[i]->GetRoleID();

		stEnjoyingNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pData->nRoomID, nRoleID);

		SendMessageNotifyToClient(MSGID_RSCL_ENJOYING_NOTI, &stEnjoyingNoti, pData->nRoomID, enmBroadcastType_ExpectPlayr,
				nRoleID);
	}

	if(nPlayerCount > 0)
	{
		//开启节目欣赏定时器
		CreateProgramTimer(pData->nRoomID, pData->nOrderID, enmProgramStatus_Enjoying);
	}

	return nRet;
}

int32_t CProgramTimer::EnjoyProgramTimeout(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CProgramSessionData * pData =(CProgramSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("enjoy program finish, start voting!{RoomID=%d, OrderID=%u}\n", pData->nRoomID, pData->nOrderID);

	//删除定时器的信息
	DeleteProgramTimerInfo(pData->nRoomID, pData->nOrderID);

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pData->nRoomID, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	//开始评价通知
//	CSongVoteUpdateNoti stSongVoteUpdateNoti;
//	stSongVoteUpdateNoti.nOrderID = pData->nOrderID;
//	stSongVoteUpdateNoti.nHaveMark = 0;
//	stSongVoteUpdateNoti.nMarkEnd = 0;
//	stSongVoteUpdateNoti.nGerneralGoodCount = 0;
//	stSongVoteUpdateNoti.nGerneralBadCount = 0;
//	stSongVoteUpdateNoti.nGerneralGiveupCount = 0;
//	stSongVoteUpdateNoti.nRequesterGoodCount = 0;
//	stSongVoteUpdateNoti.nRequesterBadCount = 0;
//	stSongVoteUpdateNoti.nRequesterGiveupCount = 0;
//	stSongVoteUpdateNoti.nEndLeftTime = g_SomeConfig.GetProgramSectionTime(enmProgramStatus_Voteing);
//
//	for(int32_t i = 0; i < nPlayerCount; ++i)
//	{
//		RoleID nRoleID = arrPlayer[i]->GetRoleID();
//
//		stSongVoteUpdateNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(pData->nRoomID, nRoleID);
//
//		SendMessageNotifyToClient(MSGID_RSCL_SONGVOTEUPDATE_NOTI, &stSongVoteUpdateNoti, pData->nRoomID,
//				enmBroadcastType_ExpectPlayr, nRoleID);
//	}
//
//	//开启节目投票定时器
//	CreateProgramTimer(pData->nRoomID, pData->nOrderID, enmProgramStatus_Voteing);

	return nRet;
}

int32_t CProgramTimer::VoteingTimeout(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CProgramSessionData * pData =(CProgramSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("vote program finish, start show result!{RoomID=%d, OrderID=%u}\n", pData->nRoomID, pData->nOrderID);

	//删除定时器的信息
	DeleteProgramTimerInfo(pData->nRoomID, pData->nOrderID);

	SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(pData->nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_ERROR_LOG("it's not found song program info!{RoomID=%d, OrderID=%u}\n", pData->nRoomID, pData->nOrderID);
		return E_NULLPOINTER;
	}

	//超时的都算弃权
	for(uint16_t i = 0; i < pSongProgramInfo->nHaveTicketPlayerCount; ++i)
	{
		if(pSongProgramInfo->arrVoteResult[i] == enmSongVoteResult_NotVote)
		{
			pSongProgramInfo->arrVoteResult[i] = enmSongVoteResult_Giveup;
			++pSongProgramInfo->nGerneralGiveupCount;
		}
	}

	if(pSongProgramInfo->nRequesterBadCount == 0 && pSongProgramInfo->nRequesterGoodCount == 0)
	{
		++pSongProgramInfo->nRequesterGiveupCount;
	}
//	//评价通知
//	CSongVoteUpdateNoti stSongVoteUpdateNoti;
//	stSongVoteUpdateNoti.nOrderID = pData->nOrderID;
//	stSongVoteUpdateNoti.nHaveTicket = 0;
//	stSongVoteUpdateNoti.nHaveMark = 0;
//	stSongVoteUpdateNoti.nMarkEnd = 1;
//	stSongVoteUpdateNoti.nGerneralGoodCount = pSongProgramInfo->nGerneralGoodCount;
//	stSongVoteUpdateNoti.nGerneralBadCount = pSongProgramInfo->nGerneralBadCount;
//	stSongVoteUpdateNoti.nGerneralGiveupCount = pSongProgramInfo->nGerneralGiveupCount;
//	stSongVoteUpdateNoti.nRequesterGoodCount = pSongProgramInfo->nRequesterGoodCount;
//	stSongVoteUpdateNoti.nRequesterBadCount = pSongProgramInfo->nRequesterBadCount;
//	stSongVoteUpdateNoti.nRequesterGiveupCount = pSongProgramInfo->nRequesterGiveupCount;
//
//	SendMessageNotifyToClient(MSGID_RSCL_SONGVOTEUPDATE_NOTI, &stSongVoteUpdateNoti, pData->nRoomID,
//			enmBroadcastType_All, enmInvalidRoleID);

	//开启展示投票结果定时器
	CreateProgramTimer(pData->nRoomID, pData->nOrderID, enmProgramStatus_ShowResult);

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(pData->nRoomID, pData->nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("it's not found song order info!{nRoomID=%d, nOrderID=%u}\n",pData->nRoomID, pData->nOrderID);
		return E_NULLPOINTER;
	}

	//更新数据库记录
	CUpdateSongOrderInfoNoti stUpdateSongOrderInfoNoti;
	stUpdateSongOrderInfoNoti.strTransID = pSongOrderInfo->strTransID;
	stUpdateSongOrderInfoNoti.nRoomID = pSongOrderInfo->nRoomID;
	stUpdateSongOrderInfoNoti.nOrderStartTime = pSongOrderInfo->nOrderStartTime;
	stUpdateSongOrderInfoNoti.nOrderEndTime = pSongOrderInfo->nOrderEndTime;
	stUpdateSongOrderInfoNoti.nOrderStatus = pSongOrderInfo->nOrderStatus;
	stUpdateSongOrderInfoNoti.strSongName = pSongOrderInfo->strSongName;
	stUpdateSongOrderInfoNoti.nSongerRoleID = pSongOrderInfo->nSongerRoleID;
	stUpdateSongOrderInfoNoti.strSongerName = pSongOrderInfo->strSongerName;
	stUpdateSongOrderInfoNoti.nRequestRoleID = pSongOrderInfo->nRequestRoleID;
	stUpdateSongOrderInfoNoti.strRequestName = pSongOrderInfo->strRequestName;
	stUpdateSongOrderInfoNoti.strWishWords = pSongOrderInfo->strWishWords;
	stUpdateSongOrderInfoNoti.nOrderCost = pSongOrderInfo->nOrderCost;
	stUpdateSongOrderInfoNoti.nSongCount = pSongOrderInfo->nSongCount;
	stUpdateSongOrderInfoNoti.nTotalTicketCount = pSongProgramInfo->nTotalTicketCount;
	stUpdateSongOrderInfoNoti.nGerneralGiveupCount = pSongProgramInfo->nGerneralGiveupCount;
	stUpdateSongOrderInfoNoti.nGerneralGoodCount = pSongProgramInfo->nGerneralGoodCount;
	stUpdateSongOrderInfoNoti.nGerneralBadCount = pSongProgramInfo->nGerneralBadCount;
	stUpdateSongOrderInfoNoti.nRequesterGiveupCount = pSongProgramInfo->nRequesterGiveupCount;
	stUpdateSongOrderInfoNoti.nRequesterGoodCount = pSongProgramInfo->nRequesterGoodCount;
	stUpdateSongOrderInfoNoti.nRequesterBadCount = pSongProgramInfo->nRequesterBadCount;
	stUpdateSongOrderInfoNoti.nMarkResult = pSongProgramInfo->nMarkResult;

	SendMessageNotifyToServer(MSGID_RSDB_UPDATESONGORDERINFO_NOTI, &stUpdateSongOrderInfoNoti, pData->nRoomID,
			enmTransType_ByKey, pSongOrderInfo->nSongerRoleID, enmEntityType_DBProxy, pSongOrderInfo->nSongerRoleID);

	return nRet;
}

int32_t CProgramTimer::ShowResultTimeout(CFrameSession *pSession)
{
	int32_t nRet = S_OK;

	CProgramSessionData * pData =(CProgramSessionData *)(pSession->GetSessionData());

	WRITE_INFO_LOG("show result finish!{RoomID=%d, OrderID=%u}\n", pData->nRoomID, pData->nOrderID);

	//删除定时器的信息
	DeleteProgramTimerInfo(pData->nRoomID, pData->nOrderID);

	//清除结果通知
	CWaitingForSongNoti stWaitingForSongNoti;
	stWaitingForSongNoti.nOrderID = pData->nOrderID;

	SendMessageNotifyToClient(MSGID_RSCL_WAITINGFORPROGRAM_NOTI, &stWaitingForSongNoti, pData->nRoomID,
			enmBroadcastType_All, enmInvalidRoleID);

	SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(pData->nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_ERROR_LOG("it's not found song program info!{RoomID=%d, nOrderID=%u, nProgramStatus=%d}\n",
				pData->nRoomID, pData->nOrderID, pData->nProgramStatus);
		return E_NULLPOINTER;
	}

	g_RoomSongProgramMgt.StopProgram(pData->nRoomID);

	return nRet;
}

int32_t CProgramTimer::CreateProgramTimer(RoomID nRoomID, uint32_t nOrderID, ProgramStatus nProgramStatus)
{
	return S_OK;

	WRITE_INFO_LOG("create program timer!{RoomID=%d, nOrderID=%u, nProgramStatus=%d}\n",
			nRoomID, nOrderID, nProgramStatus);

	CFrameSession *pSession = NULL;
	int32_t nRet = S_OK;

	SongProgramInfo *pSongProgramInfo = g_RoomSongProgramMgt.GetSongProgramInfo(nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_ERROR_LOG("create program timer failed, because get song program info error!"
				"{RoomID=%d, nOrderID=%u, nProgramStatus=%d}\n", nRoomID, nOrderID, nProgramStatus);
		return E_NULLPOINTER;
	}

	int64_t nCurTime = CDateTime::CurrentDateTime().Seconds();

	int64_t nTime = g_SomeConfig.GetProgramSectionTime(nProgramStatus) * US_PER_SECOND;
	switch(nProgramStatus)
	{
		case enmProgramStatus_FightingForTicket:
		{
			nRet = g_Frame.CreateSession(0, enmSessionType_FightForTicket, this, nTime, pSession);
			if(nRet < 0 || pSession == NULL)
			{
				WRITE_ERROR_LOG("create fight for ticket timer failed!{nRet=0x08x}\n", nRet);
				return E_NULLPOINTER;
			}

			break;
		}
		case enmProgramStatus_Enjoying:
		{
			nRet = g_Frame.CreateSession(0, enmSessionType_EnjoyProgram, this, nTime, pSession);
			if(nRet < 0 || pSession == NULL)
			{
				WRITE_ERROR_LOG("create enjoy program timer failed!{nRet=0x08x}\n", nRet);
				return E_NULLPOINTER;
			}
			break;
		}
		case enmProgramStatus_Voteing:
		{
			nRet = g_Frame.CreateSession(0, enmSessionType_Voteing, this, nTime, pSession);
			if(nRet < 0 || pSession == NULL)
			{
				WRITE_ERROR_LOG("create voteing timer failed!{nRet=0x08x}\n", nRet);
				return E_NULLPOINTER;
			}
			break;
		}
		case enmProgramStatus_ShowResult:
		{
			nRet = g_Frame.CreateSession(0, enmSessionType_ShowResult, this, nTime, pSession);
			if(nRet < 0 || pSession == NULL)
			{
				WRITE_ERROR_LOG("create voteing timer failed!{nRet=0x08x}\n", nRet);
				return E_NULLPOINTER;
			}

			pSongProgramInfo->nShowResultStartTime = nCurTime;
			break;
		}
		default:
		{
			WRITE_ERROR_LOG("create program timer failed, because program status is error!{RoomID=%d, nOrderID=%u, nProgramStatus=%d}\n",
					nRoomID, nOrderID, nProgramStatus);
			return S_OK;
		}
	}

	pSongProgramInfo->nProgramStatus = nProgramStatus;
	pSongProgramInfo->nStatusEndTime = nCurTime + (nTime / US_PER_SECOND);

	CProgramSessionData *pData = new (pSession->GetSessionData())CProgramSessionData();
	pData->nRoomID = nRoomID;
	pData->nProgramStatus = nProgramStatus;
	pData->nOrderID = nOrderID;

	if(m_nProgramTimerCount < MaxProgramTimerCount)
	{
		m_arrProgramTimerInfo[m_nProgramTimerCount].nRoomID = nRoomID;
		m_arrProgramTimerInfo[m_nProgramTimerCount].nProgramStatus = nProgramStatus;
		m_arrProgramTimerInfo[m_nProgramTimerCount].nOrderID = nOrderID;
		m_arrProgramTimerInfo[m_nProgramTimerCount].nSessionIndex = pSession->GetSessionIndex();
		++m_nProgramTimerCount;
	}

	return S_OK;
}

//删除节目定时器
int32_t CProgramTimer::DeleteProgramTimer(RoomID nRoomID, uint32_t nOrderID)
{
	return S_OK;

	WRITE_INFO_LOG("delete program timer!{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);

	int32_t nDelIndex = -1;
	for(int32_t i = 0; i < m_nProgramTimerCount; ++i)
	{
		if((m_arrProgramTimerInfo[i].nRoomID == nRoomID) &&
				(m_arrProgramTimerInfo[i].nOrderID == nOrderID))
		{
			g_Frame.RemoveSession(m_arrProgramTimerInfo[i].nSessionIndex);
			nDelIndex = i;
			break;
		}
	}

	if(nDelIndex >= 0)
	{
		//表示不是最后一个元素
		if(nDelIndex < m_nProgramTimerCount - 1)
		{
			m_arrProgramTimerInfo[nDelIndex] = m_arrProgramTimerInfo[m_nProgramTimerCount - 1];
		}

		--m_nProgramTimerCount;
	}

	return S_OK;
}

//删除节目定时器信息，没有移除会话的语句
int32_t CProgramTimer::DeleteProgramTimerInfo(RoomID nRoomID, uint32_t nOrderID)
{
	WRITE_INFO_LOG("delete program timer info!{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);

	int32_t nDelIndex = -1;
	for(int32_t i = 0; i < m_nProgramTimerCount; ++i)
	{
		if((m_arrProgramTimerInfo[i].nRoomID == nRoomID) &&
				(m_arrProgramTimerInfo[i].nOrderID == nOrderID))
		{
			nDelIndex = i;
			break;
		}
	}

	if(nDelIndex >= 0)
	{
		//表示不是最后一个元素
		if(nDelIndex < m_nProgramTimerCount - 1)
		{
			m_arrProgramTimerInfo[nDelIndex] = m_arrProgramTimerInfo[m_nProgramTimerCount - 1];
		}

		--m_nProgramTimerCount;
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

