/*
 * dal_room.cpp
 *
 *  Created on: 2011-12-14
 *      Author: jimm
 */

#include "def/server_errordef.h"
#include "dal_room.h"
#include "bll/bll_base.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CRoom::AddPlayer(const PlayerIndex nPlayerIndex, VipLevel nVipLevel, bool isHideEnter)
{
	//由于隐身进房不带机器人（可能引起越界，用这个做保护）
	if(isHideEnter && m_nHideEnterOfficer >= (MaxUserCountPerRoom - m_nMaxRealUserLimit))
	{
		return E_RS_ROOMISFULL;
	}
	if(m_sRoomPlayerIndexList.ObjectCount() >= m_nMaxRealUserLimit && !isHideEnter)
	{
		return E_RS_ROOMISFULL;
	}

	RoomPlayerIndexList::Key key = MakeKey(nPlayerIndex);
	RoomPlayerIndexList::CIndex* pIndex = m_sRoomPlayerIndexList.Insert(key, nPlayerIndex);
	if(NULL==pIndex)
	{
		return E_RS_ROOMISFULL;
	}
	if(isHideEnter)
	{
		m_nHideEnterOfficer++;
		if(m_nHideEnterOfficer >=3 )
		{
			m_nHideEnterOfficer = 3;
		}
	}
	if(nVipLevel == enmVipLevel_NONE)
	{
		AddVisitorCount();
	}
	return S_OK;
}

int32_t CRoom::DeletePlayer(const PlayerIndex nPlayerIndex, VipLevel nVipLevel, bool isHideEnter)
{
	RoomPlayerIndexList::Key key = MakeKey(nPlayerIndex);
	int32_t ret = m_sRoomPlayerIndexList.Erase(key);

	if(isHideEnter&&m_nHideEnterOfficer>0)
	{
		m_nHideEnterOfficer--;
	}
	if(nVipLevel == enmVipLevel_NONE)
	{
		DelVisitorCount();
	}
	return ret;
}

bool CRoom::IsPlayerInRoom(const PlayerIndex nPlayerIndex)
{
	RoomPlayerIndexList::Key key = MakeKey(nPlayerIndex);
	RoomPlayerIndexList::CIndex *pIndex = m_sRoomPlayerIndexList.Find(key);

	return NULL != pIndex;
}

int32_t CRoom::GetAllPlayers(PlayerIndex arrPlayerIndex[], const int32_t arrSize, int32_t &nPlayerCount)
{
	nPlayerCount = 0;
	RoomPlayerIndexList::CIndex *pIndex = m_sRoomPlayerIndexList.First();

	while(NULL != pIndex && nPlayerCount < arrSize)
	{
		arrPlayerIndex[nPlayerCount++] = pIndex->Object();
		pIndex = pIndex->Next();
	}

	return S_OK;
}

RoomPlayerIndexList::Key CRoom::MakeKey(const PlayerIndex nPlayerIndex) const
{
	RoomPlayerIndexList::Key key = {0};
	key.nKeyLength = (uint32_t)sizeof(PlayerIndex);
	*(PlayerIndex*)key.arrKey = nPlayerIndex;

	return key;
}
int32_t CRoom::AddRebotPlayer(const RoleID nRoleID)
{
	if(m_sRoomRebotIndexList.ObjectCount() > m_nMaxRealUserLimit)
	{
		return E_RS_ROOMISFULL;
	}
	RoomPlayerIndexList::Key key = MakeRebotPlayerKey(nRoleID);
	RoomPlayerIndexList::CIndex* pIndex = m_sRoomRebotIndexList.Insert(key, nRoleID);
	if(NULL==pIndex)
	{
		if(m_sRoomRebotIndexList.Find(key) != NULL)
		{
			return E_RS_KEYEXIST;
		}
		return E_RS_ROOMISFULL;
	}
	return S_OK;
}

int32_t CRoom::DeleteRebotPlayer(const RoleID nRoleID)
{
	RoomPlayerIndexList::Key key = MakeRebotPlayerKey(nRoleID);
	int32_t ret = m_sRoomRebotIndexList.Erase(key);

	//把机器人从房间的禁言列表中删除
	DelProhibitList(nRoleID);
	return ret;
}

bool CRoom::IsRebotPlayerInRoom(const RoleID nRoleID)
{
	RoomPlayerIndexList::Key key = MakeRebotPlayerKey(nRoleID);
	RoomPlayerIndexList::CIndex *pIndex = m_sRoomRebotIndexList.Find(key);

	return NULL != pIndex;
}

int32_t CRoom::GetAllRebotPlayers(RoleID arrRoleID[], const int32_t arrSize, int32_t &nPlayerCount)
{
	nPlayerCount = 0;
	RoomPlayerIndexList::CIndex *pIndex = m_sRoomRebotIndexList.First();

	while(NULL != pIndex && nPlayerCount < arrSize)
	{
		arrRoleID[nPlayerCount++] = pIndex->Object();
		pIndex = pIndex->Next();
	}

	return S_OK;
}

RoomPlayerIndexList::Key CRoom::MakeRebotPlayerKey(const RoleID nRoleID) const
{
	RoomPlayerIndexList::Key key = {0};
	key.nKeyLength = (uint32_t)sizeof(RoleID);
	*(RoleID*)key.arrKey = nRoleID;

	return key;
}

void CRoom::UpdateDataInfo(uint32_t nOffset, uint32_t nDataSize)
{
	//配置如果大于0，则上报变化
	if(g_SomeConfig.GetRoomBegin() < 0)
	{
		return;
	}
	for(int32_t i = 0; i < m_nUpdateDataInfoCount; ++i)
	{
		if(m_arrUpdateDataInfoTable[i].nOffset == nOffset)
		{
			m_arrUpdateDataInfoTable[i].nDataSize = nDataSize;
			return;
		}
	}

	if(m_nUpdateDataInfoCount < UpdateDataInfoTableSize)
	{
		m_arrUpdateDataInfoTable[m_nUpdateDataInfoCount].nOffset = nOffset;
		m_arrUpdateDataInfoTable[m_nUpdateDataInfoCount].nDataSize = nDataSize;

		++m_nUpdateDataInfoCount;
	}
	GET_UPDATEDATACTL_INSTANCE().AddRoomNeedUpdate(m_nRoomID);
}

uint8_t* CRoom::GetDataByOffset(uint32_t nDataOffset)
{
	for(int32_t i = 0; i < m_nUpdateDataInfoCount; ++i)
	{
		if(nDataOffset == m_arrUpdateDataInfoTable[i].nOffset)
		{
			uint8_t* pStartPos = &m_nStartPos;
			return &pStartPos[nDataOffset];
		}
	}

	return NULL;
}

int32_t CRoom::GetUpdateDataInfoTable(uint8_t* arrUpdateData[], UpdateDataInfoTable arrUpdateTable[])
{
	//排序
	for(int32_t i = 1;i<m_nUpdateDataInfoCount;i++)
	{
		UpdateDataInfoTable dataTemp = m_arrUpdateDataInfoTable[i];
		int32_t j =i;
		while(j>0&&m_arrUpdateDataInfoTable[j-1].nOffset>dataTemp.nOffset)
		{
			m_arrUpdateDataInfoTable[j] = m_arrUpdateDataInfoTable[j-1];
			--j;
		}
		m_arrUpdateDataInfoTable[j] = dataTemp;
	}

	//合并可以合并的
	uint32_t nOffsetTemp = m_arrUpdateDataInfoTable[0].nOffset;
	uint32_t nDatasizeTemp = m_arrUpdateDataInfoTable[0].nDataSize;
	int32_t nIndex = 0;

	uint8_t* pStartPos = &m_nStartPos;

	for(int32_t i = 1;i< m_nUpdateDataInfoCount;i++)
	{
		if(m_arrUpdateDataInfoTable[i].nOffset>=nOffsetTemp&&m_arrUpdateDataInfoTable[i].nOffset<=nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex>=1)
			{
				nIndex--;
			}
			arrUpdateTable[nIndex].nOffset = nOffsetTemp;
		}
		else if(m_arrUpdateDataInfoTable[i].nOffset<=nOffsetTemp)
		{
			arrUpdateTable[nIndex].nOffset = m_arrUpdateDataInfoTable[i].nOffset;
		}
		else if(m_arrUpdateDataInfoTable[i].nOffset==nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex>=1)
			{
				nIndex--;
			}
			arrUpdateTable[nIndex].nOffset = nOffsetTemp;
			arrUpdateTable[nIndex].nDataSize = m_arrUpdateDataInfoTable[i].nDataSize+nDatasizeTemp;
			arrUpdateData[nIndex] = &pStartPos[arrUpdateTable[nIndex].nOffset];
			nIndex++;
			nOffsetTemp = arrUpdateTable[nIndex-1].nOffset;
			nDatasizeTemp = arrUpdateTable[nIndex-1].nDataSize;
			continue;
		}
		else if(m_arrUpdateDataInfoTable[i].nOffset>nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex==0||arrUpdateTable[nIndex-1].nOffset != nOffsetTemp)
			{
				arrUpdateTable[nIndex].nOffset = nOffsetTemp;
				arrUpdateTable[nIndex].nDataSize = nDatasizeTemp;
				arrUpdateData[nIndex] = &pStartPos[arrUpdateTable[nIndex].nOffset];
				nIndex++;
			}
			arrUpdateTable[nIndex]= m_arrUpdateDataInfoTable[i];
			arrUpdateData[nIndex] = &pStartPos[arrUpdateTable[nIndex].nOffset];
			nIndex++;
			nOffsetTemp = arrUpdateTable[nIndex-1].nOffset;
			nDatasizeTemp = arrUpdateTable[nIndex-1].nDataSize;
			continue;
		}
		if(m_arrUpdateDataInfoTable[i].nOffset+m_arrUpdateDataInfoTable[i].nDataSize>nOffsetTemp+nDatasizeTemp)
		{
			arrUpdateTable[nIndex].nDataSize= m_arrUpdateDataInfoTable[i].nOffset+m_arrUpdateDataInfoTable[i].nDataSize-nOffsetTemp;
		}
		else
		{
			arrUpdateTable[nIndex].nDataSize = nDatasizeTemp;
		}
		nOffsetTemp = arrUpdateTable[nIndex].nOffset;
		nDatasizeTemp = arrUpdateTable[nIndex].nDataSize;
		arrUpdateData[nIndex] = &pStartPos[arrUpdateTable[nIndex].nOffset];
		nIndex++;
	}

	if(m_nUpdateDataInfoCount==1)
	{
		arrUpdateData[nIndex] = &pStartPos[m_arrUpdateDataInfoTable[0].nOffset];
		arrUpdateTable[nIndex] = m_arrUpdateDataInfoTable[0];
		nIndex++;
	}
	//清空
	m_nUpdateDataInfoCount = 0;
	memset(m_arrUpdateDataInfoTable,0,sizeof(m_arrUpdateDataInfoTable));

	return nIndex;
}

bool CRoom::InsertMediaMap(RoleID nSpeakerID, ServerID nServerID, bool bIsPublicMic)
{
	if(bIsPublicMic)
	{
		return InsertMediaMap(m_mapOnPublicMicMediaserver, nSpeakerID, nServerID);
	}
	else
	{
		return InsertMediaMap(m_mapOnPrivateMicMediaserver, nSpeakerID, nServerID);
	}
}

void CRoom::EraseMediaMap(RoleID nSpeakerID, ServerID nServerID)
{
	EraseMediaMap(m_mapOnPublicMicMediaserver, nSpeakerID, nServerID);
	EraseMediaMap(m_mapOnPrivateMicMediaserver, nSpeakerID, nServerID);
}

void CRoom::EraseMediaMap(RoleID nSpeakerID, ServerSet& stServerSet)
{
	EraseMediaMap(m_mapOnPublicMicMediaserver, nSpeakerID, stServerSet);
	EraseMediaMap(m_mapOnPrivateMicMediaserver, nSpeakerID, stServerSet);
}

bool CRoom::InsertMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerID nServerID)
{
	ServerSet::iterator it = stMap[nSpeakerID].find(nServerID);
	if(it == stMap[nSpeakerID].end())
	{
		stMap[nSpeakerID].insert(nServerID);
		return true;
	}
	return false;
}

//主播下麦
void CRoom::EraseMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerSet& stServerSet)
{
	MediaserverMap::iterator it = stMap.find(nSpeakerID);
	if(it != stMap.end())
	{
		ServerSet::iterator set_it = it->second.begin();
		for(; set_it != it->second.end(); set_it++)
		{
			stServerSet.insert(*set_it);
		}
		stMap.erase(it);
	}
}

//
void CRoom::EraseMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerID nServerID)
{
	MediaserverMap::iterator it = stMap.find(nSpeakerID);
	if(it != stMap.end())
	{
		it->second.erase(nServerID);
		if(it->second.size() <= 0)
		{
			stMap.erase(it);
		}
	}
}

//玩家进房时，先发送公麦上麦通知
void CRoom::SendAddTransmitUserNotifyWhenPlayerEnterRoom(CPlayer* player)
{
	if(player == NULL)
	{
		return;
	}

	RoleID arrReciverID[1] = {player->GetRoleID()};
	for(int32_t i = 0; i < m_nOnPublicMicCount; i++)
	{
		RoleID nSpeakerID = m_arrOnPublicMic[i].m_nRoleID;
		if(nSpeakerID == enmInvalidRoleID || m_arrOnPublicMic[i].isOpened)
		{
			continue;
		}
		if(InsertMediaMap(m_mapOnPublicMicMediaserver, nSpeakerID, player->GetConnInfo().nServerID))
		{
			g_BllBase.SendAddTransmitUserNotifyToMediaServer(nSpeakerID, arrReciverID, 1, this, true);
		}
	}
}

FRAME_ROOMSERVER_NAMESPACE_END
