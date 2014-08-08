/*
 * program_mgt.cpp
 *
 *  Created on: 2013-3-22
 *      Author: jimm
 */

#include "frame_errordef.h"
#include "def/server_errordef.h"
#include "song_mgt.h"
#include "program_mgt.h"
#include "server_datacenter.h"
#include "config/someconfig.h"
#include "bll/bll_base.h"
#include "requestsong/bll_timer_program.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//�Ƿ��Ѿ���Ʊ��
bool SongProgramInfo::IsHaveTicket(RoleID nRoleID)
{
	for(int32_t i = 0; i < nHaveTicketPlayerCount; ++i)
	{
		if(arrVoter[i] == nRoleID)
		{
			return true;
		}
	}

	if(nRoleID == nRequesterRoleID)
	{
		return true;
	}

	return false;
}

//�Ƿ�Ͷ��Ʊ��
bool SongProgramInfo::IsVoted(RoleID nRoleID)
{
	if(nRoleID == nRequesterRoleID)
	{
		if(nRequesterGoodCount > 0 || nRequesterBadCount > 0 || nRequesterGiveupCount > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	for(int32_t i = 0; i < nHaveTicketPlayerCount; ++i)
	{
		if(arrVoter[i] != nRoleID)
		{
			continue;
		}

		if(arrVoteResult[i] != enmSongVoteResult_NotVote)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

//�Ƿ�ȫ��Ͷ��Ʊ��
bool SongProgramInfo::IsAllVoted()
{
	for(int32_t i = 0; i < nHaveTicketPlayerCount; ++i)
	{
		if(arrVoteResult[i] == enmSongVoteResult_NotVote)
		{
			return false;
		}
	}

	if((nRequesterGoodCount == 0) && (nRequesterBadCount == 0) && (nRequesterGiveupCount == 0))
	{
		return false;
	}

	return true;
}

//������
SongVoteResult SongProgramInfo::ComputeResult()
{
	if(nProgramStatus == enmProgramStatus_Voteing)
	{
		uint16_t nRequesterTicketValue = g_SomeConfig.GetRequesterTicketValue();
		//ʹ���˳���Ʊ,����ʹ�ó���Ʊ�˽�Ŀ�������Ч
		if(nRequesterGoodCount == nRequesterTicketValue)
		{
			if(nRequesterGoodCount > nGerneralBadCount)
			{
				nMarkResult = enmSongVoteResult_Good;
			}
			else
			{
				nMarkResult = enmSongVoteResult_Bad;
			}
		}
		else if(nRequesterBadCount == nRequesterTicketValue)
		{
			if(nGerneralGoodCount > nRequesterBadCount)
			{
				nMarkResult = enmSongVoteResult_Good;
			}
			else
			{
				nMarkResult = enmSongVoteResult_Bad;
			}
		}
	}

	return nMarkResult;
}

//ͶƱ����
bool SongProgramInfo::Vote(RoleID nRoleID, SongVoteResult nResult, uint8_t nTicketCount)
{
	bool bIsSuccessed = false;

	if(IsVoted(nRoleID))
	{
		return bIsSuccessed;
	}

	if(nRequesterRoleID == nRoleID)
	{
		uint8_t nTicketValue = 1;
		//��ʾ�ó�����ί���
		if(nTicketCount > 1)
		{
			nTicketValue = g_RoomSuperVoteMgt.UpdatePlayerVoteInfo(nRoomID, nRoleID);
		}

		if(nResult == enmSongVoteResult_Good)
		{
			nRequesterGoodCount = nTicketValue;
		}
		else
		{
			nRequesterBadCount = nTicketValue;
		}

		bIsSuccessed = true;
	}
	else if(IsHaveTicket(nRoleID))
	{
		if(nResult == enmSongVoteResult_Good)
		{
			++nGerneralGoodCount;
		}
		else
		{
			++nGerneralBadCount;
		}

		for(int32_t i = 0; i < nHaveTicketPlayerCount; ++i)
		{
			if(arrVoter[i] != nRoleID)
			{
				continue;
			}

			arrVoteResult[i] = nResult;
			bIsSuccessed = true;
		}
	}

	return bIsSuccessed;
}
//����ʣ��Ʊ��
uint16_t SongProgramInfo::AddVoter(RoleID nRoleID)
{
	if(nHaveTicketPlayerCount < nTotalTicketCount)
	{
		arrVoter[nHaveTicketPlayerCount] = nRoleID;
		++nHaveTicketPlayerCount;

		return nTotalTicketCount - nHaveTicketPlayerCount;
	}

	return 0;
}

CRoomSongProgramMgt::CRoomSongProgramMgt()
{

}

CRoomSongProgramMgt::~CRoomSongProgramMgt()
{

}

//����������Ŀ��Ϣ
SongProgramInfo *CRoomSongProgramMgt::CreateSongProgramInfo()
{
	return new SongProgramInfo;
}

//���ٸ�����Ŀ��Ϣ
int32_t CRoomSongProgramMgt::DestroySongProgramInfo(RoomID nRoomID)
{
	return S_OK;
}

//���¸�����Ŀ��Ϣ
int32_t CRoomSongProgramMgt::UpdateSongProgramInfo(RoomID nRoomID, SongProgramInfo *pInfo)
{
	m_stRoomSongVoteMgt[nRoomID] = pInfo;
	return S_OK;
}

//��ȡ������Ŀ��Ϣ
SongProgramInfo *CRoomSongProgramMgt::GetSongProgramInfo(RoomID nRoomID)
{
	if(m_stRoomSongVoteMgt.count(nRoomID) > 0)
	{
		return m_stRoomSongVoteMgt[nRoomID];
	}

	return NULL;
}

//��ȡ��ҵ�Ʊ��
uint16_t CRoomSongProgramMgt::GetPlayerTicketCount(RoomID nRoomID, RoleID nRoleID)
{
	SongProgramInfo *pSongProgramInfo = GetSongProgramInfo(nRoomID);
	if(pSongProgramInfo == NULL)
	{
		return 0;
	}

	if(pSongProgramInfo->IsHaveTicket(nRoleID))
	{
		if(pSongProgramInfo->nRequesterRoleID == nRoleID)
		{
			PlayerVoteInfo *pPlayerVoteInfo = g_RoomSuperVoteMgt.GetPlayerVoteInfo(nRoomID, nRoleID);
			if(pPlayerVoteInfo == NULL)
			{
				return 1;
			}

			int32_t nLeftCount = g_SomeConfig.GetRequesterChance() - pPlayerVoteInfo->nSuperVoteCount;
			if(nLeftCount > 0)
			{
				return nLeftCount * g_SomeConfig.GetRequesterTicketValue();
			}
		}
		else
		{
			return 1;
		}
	}

	return 0;
}

//��ʼ��Ŀ
int32_t CRoomSongProgramMgt::StartProgram(RoomID nRoomID, uint32_t nOrderID)
{
	WRITE_INFO_LOG("start program!{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);

	SongProgramInfo *pSongProgramInfo = GetSongProgramInfo(nRoomID);
	if(pSongProgramInfo == NULL)
	{
		WRITE_INFO_LOG("start program, but it's not found song program info, so we'll create it!"
				"{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);

		pSongProgramInfo = CreateSongProgramInfo();
		if(pSongProgramInfo == NULL)
		{
			WRITE_WARNING_LOG("start program, create song program info failed!"
					"{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);
			return E_NULLPOINTER;
		}

		UpdateSongProgramInfo(nRoomID, pSongProgramInfo);
	}

	if(pSongProgramInfo->nProgramStatus != enmProgramStatus_WaitingForStart)
	{
		WRITE_INFO_LOG("current program is not waiting for start, so we must stop it!"
				"{nRoomID=%d, nOrderID=%u, ProgramStatus=%d}\n", nRoomID, nOrderID, pSongProgramInfo->nProgramStatus);

		g_ProgramTimer.DeleteProgramTimer(nRoomID, pSongProgramInfo->nOrderID);

//		StopProgram(nRoomID);
	}

	SongOrderInfo *pSongOrderInfo = g_RoomSongOrderListMgt.GetSongOrderInfo(nRoomID, nOrderID);
	if(pSongOrderInfo == NULL)
	{
		WRITE_WARNING_LOG("start program, but it's not found song order info!"
				"{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);
		return E_NULLPOINTER;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	int32_t nRet = g_RoomMgt.GetRoom(nRoomID, pRoom, nRoomIndex);
	if(nRet < 0 || pRoom == NULL)
	{
		WRITE_WARNING_LOG("start program is failed, because it's not found room object!"
				"{nRoomID=%d, nOrderID=%u}\n", nRoomID, nOrderID);
		return E_NULLPOINTER;
	}

	int32_t nRealUserCount = pRoom->GetCurUserCount();
	//��������
	CPlayer *pHost = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	nRet = g_PlayerMgt.GetPlayer(pRoom->GetHostID(), pHost, nPlayerIndex);
	if(pHost != NULL)
	{
		--nRealUserCount;
	}

	//����ߺͱ�����ֲ���
	nRealUserCount -= 2;
	//�οͲ���
	nRealUserCount -= pRoom->GetCurVisitorCount();
	if(nRealUserCount < 0)
	{
		nRealUserCount = 0;
	}

	uint16_t nTotalTicketCount = g_SomeConfig.GetValidTicketsCount(nRealUserCount);
//	if(nTotalTicketCount <= 0)
//	{
//		return S_OK;
//	}

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	int32_t nPlayerCount = 0;
	g_DataCenter.GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	CTicketCountUpdateNoti stTicketCountUpdateNoti;
	stTicketCountUpdateNoti.nOrderID = nOrderID;
	stTicketCountUpdateNoti.nTotalTicketCount = nTotalTicketCount;
	stTicketCountUpdateNoti.nLeftTicketCount = nTotalTicketCount;
	stTicketCountUpdateNoti.nEndLeftTime = g_SomeConfig.GetProgramSectionTime(enmProgramStatus_FightingForTicket);

	//��ʼ����֪ͨ
	for(int32_t i = 0; i < nPlayerCount; ++i)
	{
		RoleID nRoleID = arrPlayer[i]->GetRoleID();
		//���������,����������,��û��ͶƱȨ��
		if((nRoleID == pSongOrderInfo->nSongerRoleID) ||
				(nRoleID == pRoom->GetHostID()))
		{
			stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		//����ǵ����,�Ѿ�����ͶƱȨ�����ǲ�����Ʊ
		else if(nRoleID == pSongOrderInfo->nRequestRoleID)
		{
			stTicketCountUpdateNoti.nHaveTicket = 1;
			stTicketCountUpdateNoti.nCanGetTicket = 0;
		}
		else
		{
			stTicketCountUpdateNoti.nHaveTicket = 0;
			stTicketCountUpdateNoti.nCanGetTicket = 1;
		}

		g_BllBase.SendMessageNotifyToClient(MSGID_RSCL_TICKETCOUNTUPDATE_NOTI, &stTicketCountUpdateNoti,
				nRoomID, enmBroadcastType_ExpectPlayr, nRoleID);
	}

	//��ʼ����Ŀ��Ϣ
	pSongProgramInfo->nRoomID = nRoomID;
	pSongProgramInfo->nOrderID = nOrderID;
	pSongProgramInfo->nProgramStatus = enmProgramStatus_FightingForTicket;
//	pSongProgramInfo->nStatusEndTime = CDateTime::CurrentDateTime().Seconds() + g_SomeConfig.GetProgramSectionTime(enmProgramStatus_FightingForTicket);
	pSongProgramInfo->nTotalTicketCount = nTotalTicketCount;
	pSongProgramInfo->nRequesterRoleID = pSongOrderInfo->nRequestRoleID;

	if(nPlayerCount > 0)
	{
		if(nTotalTicketCount > 0)
		{
			//������Ʊ��ʱ��
			g_ProgramTimer.CreateProgramTimer(nRoomID, nOrderID, enmProgramStatus_FightingForTicket);
		}
		else
		{
			//��ʼ����֪ͨ
			CEnjoyingNoti stEnjoyingNoti;
			stEnjoyingNoti.nOrderID = nOrderID;

			for(int32_t i = 0; i < nPlayerCount; ++i)
			{
				RoleID nRoleID = arrPlayer[i]->GetRoleID();

				stEnjoyingNoti.nHaveTicket = g_RoomSongProgramMgt.GetPlayerTicketCount(nRoomID, nRoleID);

				g_BllBase.SendMessageNotifyToClient(MSGID_RSCL_ENJOYING_NOTI, &stEnjoyingNoti, nRoomID, enmBroadcastType_ExpectPlayr,
						nRoleID);
			}

			//������Ŀ���Ͷ�ʱ��
			g_ProgramTimer.CreateProgramTimer(nRoomID, nOrderID, enmProgramStatus_Enjoying);
		}
	}

	return S_OK;
}

//������Ŀ
int32_t CRoomSongProgramMgt::StopProgram(RoomID nRoomID)
{
	WRITE_INFO_LOG("stop program!{nRoomID=%d}\n", nRoomID);

	SongProgramInfo *pSongProgramInfo = GetSongProgramInfo(nRoomID);
	if(pSongProgramInfo != NULL)
	{
		pSongProgramInfo->Reset();
	}

	return S_OK;
}

CRoomSuperVoteMgt::CRoomSuperVoteMgt()
{

}

CRoomSuperVoteMgt::~CRoomSuperVoteMgt()
{

}

//����������ίͶƱ��Ϣ������
PlayerVoteMgt *CRoomSuperVoteMgt::CreatePlayerVoteMgt()
{
	return new PlayerVoteMgt;
}

PlayerVoteInfo *CRoomSuperVoteMgt::CreatePlayerVoteInfo()
{
	return new PlayerVoteInfo;
}

PlayerVoteInfo *CRoomSuperVoteMgt::GetPlayerVoteInfo(RoomID nRoomID, RoleID nRoleID)
{
	PlayerVoteInfo *pPlayerVoteInfo = NULL;
	PlayerVoteMgt *pPlayerVoteMgt = NULL;

	if(m_stRoomSuperVoteMgt.count(nRoomID) <= 0)
	{
		pPlayerVoteMgt = CreatePlayerVoteMgt();
		if(pPlayerVoteMgt == NULL)
		{
			return NULL;
		}

		m_stRoomSuperVoteMgt[nRoomID] = pPlayerVoteMgt;
	}

	RoomSuperVoteMgt::iterator room_iter = m_stRoomSuperVoteMgt.find(nRoomID);

	pPlayerVoteMgt = room_iter->second;
	PlayerVoteMgt::iterator player_iter = pPlayerVoteMgt->find(nRoleID);
	if(player_iter == pPlayerVoteMgt->end())
	{
		pPlayerVoteInfo = CreatePlayerVoteInfo();
		if(pPlayerVoteInfo == NULL)
		{
			return NULL;
		}

		pPlayerVoteInfo->nRoleID = nRoleID;
		pPlayerVoteMgt->insert(make_pair(nRoleID, pPlayerVoteInfo));
	}
	else
	{
		pPlayerVoteInfo = player_iter->second;
	}

	FlushPlayerVoteInfo(pPlayerVoteInfo);

	return pPlayerVoteInfo;
}

//ˢ��ͶƱ��Ϣ
void CRoomSuperVoteMgt::FlushPlayerVoteInfo(PlayerVoteInfo *pPlayerVoteInfo)
{
	if(pPlayerVoteInfo != NULL)
	{
		uint16_t nCurYear = CDateTime::CurrentDateTime().Year();
		uint16_t nCurMonth = CDateTime::CurrentDateTime().Month();
		uint16_t nCurDay = CDateTime::CurrentDateTime().Day();

		//��ʾ���ڼ�¼����Ϣ�Ѿ�������
		if((nCurYear > pPlayerVoteInfo->nYear) ||
				(nCurMonth > pPlayerVoteInfo->nMonth) ||
				(nCurDay > pPlayerVoteInfo->nDay))
		{
			pPlayerVoteInfo->nYear = nCurYear;
			pPlayerVoteInfo->nMonth = nCurMonth;
			pPlayerVoteInfo->nDay = nCurDay;
			pPlayerVoteInfo->nSuperVoteCount = 0;
		}
	}
}

//������ҵ�ͶƱ����(��Ϊ������ί)
uint16_t CRoomSuperVoteMgt::UpdatePlayerVoteInfo(RoomID nRoomID, RoleID nRoleID)
{
	uint16_t nTicketValue = 1;

	PlayerVoteInfo *pPlayerVoteInfo = GetPlayerVoteInfo(nRoomID, nRoleID);
	if(pPlayerVoteInfo != NULL)
	{
		FlushPlayerVoteInfo(pPlayerVoteInfo);

		if(pPlayerVoteInfo->nSuperVoteCount < g_SomeConfig.GetRequesterChance())
		{
			nTicketValue = g_SomeConfig.GetRequesterTicketValue();
			++pPlayerVoteInfo->nSuperVoteCount;
		}
	}

	return nTicketValue;
}

int32_t CRoomSuperVoteMgt::ClearAllVoteInfo()
{
	RoomSuperVoteMgt::iterator it = m_stRoomSuperVoteMgt.begin();
	for(; it != m_stRoomSuperVoteMgt.end(); it++)
	{
		ClearRoomPlayerVoteInfo(it->first);
	}

	return S_OK;
}

int32_t CRoomSuperVoteMgt::ClearRoomPlayerVoteInfo(RoomID nRoomID)
{
	RoomSuperVoteMgt::iterator room_iter = m_stRoomSuperVoteMgt.find(nRoomID);
	if(room_iter != m_stRoomSuperVoteMgt.end())
	{
		uint16_t nCurYear = CDateTime::CurrentDateTime().Year();
		uint16_t nCurMonth = CDateTime::CurrentDateTime().Month();
		uint16_t nCurDay = CDateTime::CurrentDateTime().Day();

		WRITE_DEBUG_LOG("ClearRoomPlayerVoteInfo!{nRoomID=%d, nCurYear=%d, nCurMonth=%d, nCurDay=%d}\n",
				nRoomID, nCurYear, nCurMonth, nCurDay);

		PlayerVoteMgt *pPlayerVoteMgt = room_iter->second;
		PlayerVoteMgt::iterator player_iter = pPlayerVoteMgt->begin();
		for(; player_iter != pPlayerVoteMgt->end(); player_iter++)
		{
			PlayerVoteInfo *pPlayerVoteInfo = player_iter->second;
			if(pPlayerVoteInfo != NULL)
			{
				//��ʾ���ڼ�¼����Ϣ�Ѿ�������
				if((nCurYear > pPlayerVoteInfo->nYear) ||
						(nCurMonth > pPlayerVoteInfo->nMonth) ||
						(nCurDay > pPlayerVoteInfo->nDay))
				{
					WRITE_DEBUG_LOG("delete expried player vote info!{nRoomID=%d, nRoleID=%d}\n",
							nRoomID, pPlayerVoteInfo->nRoleID);

					delete pPlayerVoteInfo;
					pPlayerVoteMgt->erase(player_iter);
				}
			}
		}
	}

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
