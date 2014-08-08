/*
 * dal_player.cpp
 *
 *  Created on: 2011-12-15
 *      Author: jimm
 */

#include "dal_channel.h"
#include "main_frame.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

void CPlayer::GetAllEnterRoom(RoomID arrRoomID[], const int32_t arrSize, int32_t &nGetCurRoomCount)
{
	nGetCurRoomCount = m_nCurPlayerRoomCount > arrSize ? arrSize : m_nCurPlayerRoomCount;
	for(int32_t i = 0; i < nGetCurRoomCount; ++i)
	{
		arrRoomID[i] = m_arrPlayerRoomInfo[i].nRoomID;
	}
}

void CPlayer::GetPlayerRoomInfo(PlayerRoomInfo arrPlayerRoomInfo[], const int32_t arrSize, int32_t &nGetInfoCount)
{
	nGetInfoCount = m_nCurPlayerRoomCount > arrSize ? arrSize : m_nCurPlayerRoomCount;
	memcpy(arrPlayerRoomInfo, m_arrPlayerRoomInfo, nGetInfoCount * sizeof(PlayerRoomInfo));
}

void CPlayer::GetAllPlayerStateInAllRoom(PlayerState arrPlayerState[], const int32_t arrSize, int32_t &nGetPlayerStateCount)
{
	nGetPlayerStateCount = m_nCurPlayerRoomCount > arrSize ? arrSize : m_nCurPlayerRoomCount;
	for(int32_t i = 0; i < nGetPlayerStateCount; ++i)
	{
		arrPlayerState[i] = m_arrPlayerRoomInfo[i].nPlayerState;
	}
}

void CPlayer::EnterRoom(const RoomID nRoomID)
{
	for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
	{
		if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
		{
			m_arrPlayerRoomInfo[i].nPlayerState = enmPlayerState_InRoom;
			m_arrPlayerRoomInfo[i].nEnterRoomTime = CTimeValue::CurrentTime().Seconds();
			m_arrPlayerRoomInfo[i].nLastStaTime = CTimeValue::CurrentTime().Seconds();
			WRITE_DEBUG_LOG("total enter: player %d enter Room %d total enter [%d]!!!!!!",m_nRoleID,nRoomID,m_nCurPlayerRoomCount);
			return;
		}
	}
	m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nRoomID = nRoomID;
	m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nPlayerState = enmPlayerState_InRoom;
	m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nEnterRoomTime = CTimeValue::CurrentTime().Seconds();
	m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nLastStaTime = CTimeValue::CurrentTime().Seconds();
	m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nLastHeartBeatTime = CTimeValue::CurrentTime().Seconds();

	UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount]), sizeof(PlayerRoomInfo));
	++m_nCurPlayerRoomCount;

	if(m_nPlayerType == enmPlayerType_Rebot)
	{
		SetTotleEnterRoomCount(m_nTotleEnterRoomCount++);
	}
	UpdateDataInfo(GetMemberOffset(m_nCurPlayerRoomCount), sizeof(m_nCurPlayerRoomCount));

	WRITE_DEBUG_LOG("total enter: player enter room {nRoleID=%d, nRoomID=%d, nTotalEnter=%d}!",m_nRoleID,nRoomID,m_nCurPlayerRoomCount);
}

void CPlayer::ExitRoom(const RoomID nRoomID)
{
	for(int32_t i = 0; i < m_nCurPlayerRoomCount; ++i)
	{
		if(nRoomID == m_arrPlayerRoomInfo[i].nRoomID)
		{
			//将最后一位填充删除的位置
			m_nCurPlayerRoomCount--;
			memcpy(&m_arrPlayerRoomInfo[i], &m_arrPlayerRoomInfo[m_nCurPlayerRoomCount], sizeof(PlayerRoomInfo));

			m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].Reset();

			UpdateDataInfo(GetMemberOffset(m_nCurPlayerRoomCount), sizeof(m_nCurPlayerRoomCount));
			UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount]), sizeof(PlayerRoomInfo));
			WRITE_DEBUG_LOG("total enter: player %d exit Room %d total enter[%d]!!!!!!",m_nRoleID,nRoomID,m_nCurPlayerRoomCount);
			if(m_nPlayerType == enmPlayerType_Rebot)
			{
				SetTotleEnterRoomCount(m_nTotleEnterRoomCount--);
			}
			break;
		}
	}
}

RoleRank CPlayer::GetRoleRankInRoom(const RoomID nRoomID)
{
	for(int32_t i = 0; i < m_nCurPlayerRoomCount; ++i)
	{
		if(nRoomID == m_arrPlayerRoomInfo[i].nRoomID)
		{
			return m_arrPlayerRoomInfo[i].nRoleRank;
		}
	}

	return enmRoleRank_None;
}

void CPlayer::UpdateDataInfo(uint32_t nOffset, uint32_t nDataSize)
{
	//配置如果大于0，则上报变化
	if(g_SomeConfig.GetPlayerBegin() < 0)
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

	GET_UPDATEDATACTL_INSTANCE().AddPlayerNeedUpdate(m_nRoleID);
}

uint8_t* CPlayer::GetDataByOffset(uint32_t nDataOffset)
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

int32_t CPlayer::GetUpdateDataInfoTable(uint8_t* arrUpdateData[], UpdateDataInfoTable arrUpdateTable[])
{
	//排序
	for(int32_t i = 1;i < m_nUpdateDataInfoCount;i++)
	{
		UpdateDataInfoTable dataTemp = m_arrUpdateDataInfoTable[i];
		int32_t j = i;
		while(j > 0 && m_arrUpdateDataInfoTable[j-1].nOffset>dataTemp.nOffset)
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

	for(int32_t i = 1;i < m_nUpdateDataInfoCount;i++)
	{
		if(m_arrUpdateDataInfoTable[i].nOffset >= nOffsetTemp && m_arrUpdateDataInfoTable[i].nOffset <= nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex >= 1)
			{
				nIndex--;
			}
			arrUpdateTable[nIndex].nOffset = nOffsetTemp;
		}
		else if(m_arrUpdateDataInfoTable[i].nOffset <= nOffsetTemp)
		{
			arrUpdateTable[nIndex].nOffset = m_arrUpdateDataInfoTable[i].nOffset;
		}
		else if(m_arrUpdateDataInfoTable[i].nOffset == nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex >= 1)
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
		else if(m_arrUpdateDataInfoTable[i].nOffset > nOffsetTemp+nDatasizeTemp)
		{
			if(nIndex == 0 || arrUpdateTable[nIndex-1].nOffset != nOffsetTemp)
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
		if((m_arrUpdateDataInfoTable[i].nOffset+m_arrUpdateDataInfoTable[i].nDataSize) > nOffsetTemp+nDatasizeTemp)
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

	if(m_nUpdateDataInfoCount == 1)
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


//合并玩家的在房间的信息
void CPlayer::MergePlayerRoomInfo(PlayerRoomInfo arrPlayerRoomInfo[], int32_t nRoomInfoCount)
{
	for(int32_t i = 0; i < nRoomInfoCount; ++i)
	{
		if(m_nCurPlayerRoomCount >= MaxEnterRoomCount)
		{
			break;
		}

		bool bIsFind = false;
		for(int32_t j = 0; j < m_nCurPlayerRoomCount; ++j)
		{
			if(arrPlayerRoomInfo[i].nRoomID == m_arrPlayerRoomInfo[j].nRoomID)
			{
				bIsFind = true;
				break;
			}
		}

		if(bIsFind)
		{
			continue;
		}

		m_arrPlayerRoomInfo[m_nCurPlayerRoomCount] = arrPlayerRoomInfo[i];
		UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount]), sizeof(PlayerRoomInfo));

		++m_nCurPlayerRoomCount;
		UpdateDataInfo(GetMemberOffset(m_nCurPlayerRoomCount), sizeof(m_nCurPlayerRoomCount));
	}
}

FRAME_ROOMSERVER_NAMESPACE_END

