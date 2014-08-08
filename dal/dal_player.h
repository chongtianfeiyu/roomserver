/*
 * dal_player.h
 *
 *  Created on: 2011-12-15
 *      Author: jimm
 */

#ifndef DAL_PLAYER_H_
#define DAL_PLAYER_H_

#include "common/common_complextype.h"
#include "common/common_object.h"
#include "common/common_datetime.h"
#include "common/common_string.h"
#include "def/server_namespace.h"
#include "def/server_errordef.h"
#include "public_typedef.h"
#include "dal_room.h"
#include "config/someconfig.h"
#include "def/dal_def.h"
#include "dal/dal_updatedatainfotable.h"
#include "ctl/update_data_ctl.h"
#include <bitset>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

using namespace std;

class PlayerRoomInfo
{
public:
	//	ServerID		nServerID;			//所在的roomserverid
	RoomID 			nRoomID; //进入的房间
	PlayerState 	nPlayerState; //在房间的状态
	RoleRank 		nRoleRank; //在房间的管理权限
	uint32_t 		nEnterRoomTime; //进入房间的时间
	uint32_t 		nGoToMicTime; //上麦时间
	uint64_t 		nTotalTime; //在本房间的总共时间
	RoleID 			nP2PRoleID; //与其一对一的玩家
	uint8_t 		nPublicMicIndex ;
	uint8_t 		nAudioStatus;
	uint8_t 		nVideoStatus;
	int32_t 		nWatchPrivateMicCount;   //现在在观看的私麦的数量
	RoleID  		arrWatchPrivateMic[MaxWatchPrivateMicCount]; //观看的私麦的人
	int64_t 		nLastStaTime;			//最后一次统计时间
	int64_t 		nLastStaOnMicTime;			//最后一次统计时间
	int64_t			nLastHeartBeatTime;			//最后心跳时间
	uint32_t		m_nKey;						//登录时服务器分配的key

	PlayerRoomInfo()
	{
		Reset();
	}

	void Reset()
	{
		nRoomID = enmInvalidRoomID;
		nPlayerState = enmInvalidPlayerState;
		nRoleRank = enmRoleRank_None;
		nEnterRoomTime = 0;
		nTotalTime = 0;
		nGoToMicTime = 0;
		nP2PRoleID = enmInvalidRoleID;
		nPublicMicIndex = 0;
		nAudioStatus = enmAVType_Close;
		nVideoStatus = enmAVType_Open;
		nWatchPrivateMicCount = 0;
		memset(arrWatchPrivateMic,0,sizeof(arrWatchPrivateMic));
		nLastStaTime = 0;
		nLastStaOnMicTime = 0;
		nLastHeartBeatTime = 0;
		m_nKey = 0;
	}
};

class RoomRankInfo
{
public:
	RoomID 			nRoomID; //进入的房间
	RoleRank 		nRoleRank; //在房间的管理权限

	RoomRankInfo()
	{
		Reset();
	}

	void Reset()
	{
		nRoomID = enmInvalidRoomID;
		nRoleRank = enmRoleRank_None;
	}
};

#define PlayerTimeOutTime 	5*SECOND_PER_MINUTE
#define MaxP2PWaitTime   10*US_PER_MINUTE       //10分钟

class CPlayer : public CObject
{
public:
	CPlayer()
	{
		Reset();
	}

	virtual ~CPlayer()
	{

	}

	int32_t Initialize()
	{
		Reset();
		m_nLastHeatBeatTime = CDateTime::CurrentDateTime().Seconds();
		return S_OK;
	}

	int32_t Resume()
	{
		Reset();
		return S_OK;
	}

	int32_t Uninitialize()
	{
		Reset();
		return S_OK;
	}

	void GetAllEnterRoom(RoomID arrRoomID[], const int32_t arrSize, int32_t &nGetCurRoomCount);

	void GetPlayerRoomInfo(PlayerRoomInfo arrPlayerRoomInfo[], const int32_t arrSize, int32_t &nGetInfoCount);

	void GetAllPlayerStateInAllRoom(PlayerState arrPlayerState[], const int32_t arrSize, int32_t &nGetPlayerStateCount);

	void EnterRoom(const RoomID nRoomID);

	void ExitRoom(const RoomID nRoomID);

	RoleRank GetRoleRankInRoom(const RoomID nRoomID);

	bool CanEnterRoom(bool isSpecial = false) const
	{
		CVipInfo stVipInfo = g_SomeConfig.GetVipInfo(m_nVipLevel);
		uint16_t nMaxCanEnterRoomCount = stVipInfo.nMaxEnterRoomCount;
		if(isSpecial)
		{
			return !(m_nTotleEnterRoomCount >= MaxEnterRoomCount);
		}
		return (m_nTotleEnterRoomCount < MaxEnterRoomCount) && (m_nTotleEnterRoomCount-m_nNewPlayerRoomCount < nMaxCanEnterRoomCount);
	}
	uint16_t GetMaxCanEnterRoomCount()
	{
		CVipInfo stVipInfo = g_SomeConfig.GetVipInfo(m_nVipLevel);
		return stVipInfo.nMaxEnterRoomCount;
	}
	void SetTotleEnterRoomCount( int32_t nTotleEnterRoomCount)
	{
		m_nTotleEnterRoomCount = nTotleEnterRoomCount;

		UpdateDataInfo(GetMemberOffset(m_nTotleEnterRoomCount), sizeof(m_nTotleEnterRoomCount));
	}

	int32_t GetTotleEnterRoomCount()
	{
		return m_nTotleEnterRoomCount;
	}

	void SetNewPlayerRoomCount( int32_t nNewPlayerRoomCount)
	{
		m_nNewPlayerRoomCount = nNewPlayerRoomCount;

		UpdateDataInfo(GetMemberOffset(m_nNewPlayerRoomCount), sizeof(m_nNewPlayerRoomCount));
	}
	int32_t GetCurEnterRoomCount() const
	{
		return m_nCurPlayerRoomCount;
	}

	void SetRoleID(const RoleID nRoleID)
	{
		m_nRoleID = nRoleID;
		UpdateDataInfo(GetMemberOffset(m_nRoleID), sizeof(m_nRoleID));
	}

	RoleID GetRoleID() const
	{
		return m_nRoleID;
	}

	void SetAccountID(const AccountID nAccountID)
	{
		m_nAccountID = nAccountID;
		UpdateDataInfo(GetMemberOffset(m_nAccountID), sizeof(m_nAccountID));
	}

	AccountID GetAccountID() const
	{
		return m_nAccountID;
	}

	void SetVipLevel(const VipLevel nVipLevel)
	{
		m_nVipLevel = nVipLevel;
		UpdateDataInfo(GetMemberOffset(m_nVipLevel), sizeof(m_nVipLevel));
	}

	VipLevel GetVipLevel() const
	{
		return m_nVipLevel;
	}

	void SetUserLevel(const UserLevel nUserLevel)
	{

	}

	UserLevel GetUserLevel() const
	{
		return 1;
	}
	void SetRoleName(const char *strRoleName)
	{
		m_strRoleName = strRoleName;
		UpdateDataInfo(GetMemberOffset(m_strRoleName), sizeof(m_strRoleName));
	}

	const char* GetRoleName() const
	{
		return m_strRoleName.GetString();
	}

	void SetPlayerGender(const Gender nGender)
	{
		m_nPlayerGender = nGender;
		UpdateDataInfo(GetMemberOffset(m_nPlayerGender), sizeof(m_nPlayerGender));
	}

	Gender GetPlayerGender() const
	{
		return m_nPlayerGender;
	}

	void SetLoginTime(const uint64_t nLoginTime)
	{
		m_nLoginTime = nLoginTime;
		UpdateDataInfo(GetMemberOffset(m_nLoginTime), sizeof(m_nLoginTime));
	}

	uint64_t GetLoginTime() const
	{
		return m_nLoginTime;
	}

	void SetLogoutTime(const uint64_t nLogoutTime)
	{
		m_nLogoutTime = nLogoutTime;
		UpdateDataInfo(GetMemberOffset(m_nLogoutTime), sizeof(m_nLogoutTime));
	}

	uint64_t GetLogoutTime() const
	{
		return m_nLogoutTime;
	}

	void GetEnterRoomInfo(RoomID arrRoomID[],int32_t &nEnterCount,const int32_t nArrSize)
	{
		nEnterCount = m_nCurPlayerRoomCount > nArrSize ? nArrSize : m_nCurPlayerRoomCount;
		for(int32_t i = 0;i < nEnterCount;i++)
		{
			arrRoomID[i] = m_arrPlayerRoomInfo[i].nRoomID;
		}
	}
	void SetAdminRoomID(const RoomID arrRoomID[], const RoleRank arrRoleRank[],const uint16_t nCount)
	{
		m_nCurAdminRoomCount = MaxBeAdminPerPlayer > nCount ? nCount : MaxBeAdminPerPlayer;
		for(int32_t i = 0; i < m_nCurAdminRoomCount; ++i)
		{
			m_arrRoomRankInfo[i].nRoomID = arrRoomID[i];
			m_arrRoomRankInfo[i].nRoleRank = arrRoleRank[i];
			UpdateDataInfo(GetMemberOffset(m_arrRoomRankInfo[i]), sizeof(m_arrRoomRankInfo[i]));
		}
		UpdateDataInfo(GetMemberOffset(m_nCurAdminRoomCount), sizeof(m_nCurAdminRoomCount));
	}

	void GetAdminRoomRankInfo(RoomRankInfo arrRoomRankInfo[], const uint16_t arrSize, uint16_t &nCount) const
	{
		nCount = m_nCurAdminRoomCount > arrSize ? arrSize : m_nCurAdminRoomCount;
		for(int32_t i = 0; i < nCount; ++i)
		{
			arrRoomRankInfo[i] = m_arrRoomRankInfo[i];
		}
	}

	bool IsAdminRoom(const RoomID nRoomID,const RoleRank nRoleRank) const
	{
		for(int32_t i = 0; i < m_nCurAdminRoomCount; ++i)
		{
			if(m_arrRoomRankInfo[i].nRoomID == nRoomID&&m_arrRoomRankInfo[i].nRoleRank == nRoleRank)
			{
				return true;
			}
		}
		return false;
	}

	bool IsCanAddAdminInRoom(const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurAdminRoomCount;i++)
		{
			if(m_arrRoomRankInfo[i].nRoomID!=nRoomID)
			{
				return false;
			}
		}
		return true;
	}
	int32_t  AddAdminRoom(const RoomID nRoomID, const RoleRank nRoleRank)
	{
		for(int32_t i = 0; i < m_nCurAdminRoomCount; ++i)
		{
			if(m_arrRoomRankInfo[i].nRoomID == nRoomID&&m_arrRoomRankInfo[i].nRoleRank==nRoleRank)
			{
				return S_OK;
			}
			else if(m_arrRoomRankInfo[i].nRoomID == nRoomID&&m_arrRoomRankInfo[i].nRoleRank!=nRoleRank)
			{
				m_arrRoomRankInfo[i].nRoleRank=nRoleRank;
				UpdateDataInfo(GetMemberOffset(m_arrRoomRankInfo[i].nRoleRank), sizeof(m_arrRoomRankInfo[i].nRoleRank));
				return S_OK;
			}
		}
		if(m_nCurAdminRoomCount>=MaxBeAdminPerPlayer)
		{
			return E_RS_ADMININROOMTOBIG;
		}
		m_arrRoomRankInfo[m_nCurAdminRoomCount].nRoleRank=nRoleRank;
		m_arrRoomRankInfo[m_nCurAdminRoomCount].nRoomID=nRoomID;
		UpdateDataInfo(GetMemberOffset(m_arrRoomRankInfo[m_nCurAdminRoomCount]), sizeof(m_arrRoomRankInfo[m_nCurAdminRoomCount]));

		m_nCurAdminRoomCount++;
		UpdateDataInfo(GetMemberOffset(m_nCurAdminRoomCount), sizeof(m_nCurAdminRoomCount));
		return S_OK;
	}
	int32_t DelAdminRoom(const RoomID nRoomID)
	{
		for(int32_t i = 0; i < m_nCurAdminRoomCount; ++i)
		{
			if(m_arrRoomRankInfo[i].nRoomID == nRoomID)
			{
				m_arrRoomRankInfo[i]=m_arrRoomRankInfo[m_nCurAdminRoomCount - 1];
				m_arrRoomRankInfo[m_nCurAdminRoomCount - 1].nRoleRank=enmRoleRank_None;
				m_arrRoomRankInfo[m_nCurAdminRoomCount - 1].nRoomID=enmInvalidRoomID;
				UpdateDataInfo(GetMemberOffset(m_arrRoomRankInfo[i]), sizeof(m_arrRoomRankInfo[i]));

				--m_nCurAdminRoomCount;
				UpdateDataInfo(GetMemberOffset(m_nCurAdminRoomCount), sizeof(m_nCurAdminRoomCount));
				return S_OK;
			}
		}
		return S_OK;
	}
	void RecvHeartBeatInRoom(RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nLastHeartBeatTime = CTimeValue::CurrentTime().Seconds();
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nLastHeartBeatTime), sizeof(m_arrPlayerRoomInfo[i].nLastHeartBeatTime));
				break;
			}
		}
	}
	//获取超时的房间
	int32_t GetTimeOutRoomID(RoomID arrRoomID[],uint32_t nSize,int32_t &nTimeoutRoomIDCount )
	{
		nTimeoutRoomIDCount = 0;
		for(int32_t i = 0;i<m_nCurPlayerRoomCount&&nTimeoutRoomIDCount<(int32_t)nSize;i++)
		{
			if(CTimeValue::CurrentTime().Seconds()-PlayerTimeOutTime>m_arrPlayerRoomInfo[i].nLastHeartBeatTime)
			{
				arrRoomID[nTimeoutRoomIDCount++] = m_arrPlayerRoomInfo[i].nRoomID;
			}
		}
		return S_OK;
	}
	bool IsStateInRoom(const PlayerState nPlayerState,const RoomID nRoomID) const
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID&&m_arrPlayerRoomInfo[i].nPlayerState==nPlayerState)
			{
				return true;
			}
		}
		return false;
	}

	int32_t SetP2PRoleIDInRoom(const RoomID nRoomID,const RoleID nP2PRoleID )
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nP2PRoleID = nP2PRoleID;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nP2PRoleID), sizeof(m_arrPlayerRoomInfo[i].nP2PRoleID));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;

	}

	int32_t GetP2PRoleIDInRoom(const RoomID nRoomID, RoleID &nP2PRoleID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				nP2PRoleID=m_arrPlayerRoomInfo[i].nP2PRoleID;
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}
	int32_t SetStateInRoom(const PlayerState nPlayerState,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				PlayerState nNewPlayerState = nPlayerState;
				if(nPlayerState == enmPlayerState_PublicMicQueue)
				{
					if(m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_P2PMic)
					{
						nNewPlayerState = enmPlayerState_P2PMicAndMicQueue;
					}
				}
				if(nPlayerState == enmPlayerState_P2PMic)
				{
					if(m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_PublicMicQueue)
					{
						nNewPlayerState = enmPlayerState_P2PMicAndMicQueue;
					}
				}
				m_arrPlayerRoomInfo[i].nPlayerState=nNewPlayerState;
				if(nPlayerState == enmPlayerState_PrivateMic||nPlayerState == enmPlayerState_PublicMic)
				{
					uint64_t nCurTime = CTimeValue::CurrentTime().Seconds()/60;
					m_arrPlayerRoomInfo[i].nGoToMicTime = nCurTime;
					m_arrPlayerRoomInfo[i].nLastStaOnMicTime = CTimeValue::CurrentTime().Seconds();
				}
				else
				{
					m_arrPlayerRoomInfo[i].nAudioStatus = enmAVType_Close;
					m_arrPlayerRoomInfo[i].nVideoStatus = enmAVType_Open;
					m_arrPlayerRoomInfo[i].nGoToMicTime = 0;
				}
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i]), sizeof(m_arrPlayerRoomInfo[i]));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}
	uint32_t GetGoToMicTime(const RoomID nRoomID) const
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return m_arrPlayerRoomInfo[i].nGoToMicTime;
			}
		}
		return 0;
	}
	int32_t SetPublicMicIndexInRoom(const uint8_t nPublicMicIndex,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nPublicMicIndex = nPublicMicIndex;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nPublicMicIndex), sizeof(m_arrPlayerRoomInfo[i].nPublicMicIndex));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}

	PlayerState GetStateInRoom(const RoomID nRoomID)const
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return m_arrPlayerRoomInfo[i].nPlayerState;
			}
		}
		return enmInvalidPlayerState;
	}

	StatusType GetPlayerStatusInRoom(const RoomID nRoomID)const
	{
		StatusType nStatusType = enmInvalidStatusType;
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				if( m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_PublicMic)
				{
					switch(m_arrPlayerRoomInfo[i].nPublicMicIndex)
					{
						case 1:
						{
							nStatusType|=enmStatusType_PUBLIC_1;
						}
						break;
						case 2:
						{
							nStatusType|=enmStatusType_PUBLIC_2;
						}
						break;
						case 3:
						{
							nStatusType|=enmStatusType_PUBLIC_3;
						}
						break;
					}
				}
				if( m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_PrivateMic)
				{
					nStatusType|=enmStatusType_PRIVATE;
				}
				if( m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_P2PMic||m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_P2PMicAndMicQueue)
				{
					nStatusType|=enmStatusType_P2P;
				}
				//todo 后面要加判断的
				if(m_arrPlayerRoomInfo[i].nAudioStatus == enmAVType_Open)
				{
					nStatusType|=enmStatusType_AUDIO;
				}
				if(m_arrPlayerRoomInfo[i].nVideoStatus == enmAVType_Open)
				{
					nStatusType|=enmStatusType_VIDEO;
				}

			}
		}
		return nStatusType;
	}

	bool IsInRoom(const RoomID nRoomID) const
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return true;
			}
		}
		return false;
	}

	bool IsOnMicInRoom(const RoomID nRoomID) const
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID&&(m_arrPlayerRoomInfo[i].nPlayerState==enmPlayerState_P2PMic||m_arrPlayerRoomInfo[i].nPlayerState==enmPlayerState_PublicMic||m_arrPlayerRoomInfo[i].nPlayerState==enmPlayerState_PrivateMic||m_arrPlayerRoomInfo[i].nPlayerState==enmPlayerState_P2PMicAndMicQueue))
			{
				return true;
			}
		}
		return false;
	}
    /*
     * 只在test_main中用了
     */
	int32_t SetPlayerInRoom(const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return S_OK;
			}
		}
		m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nRoomID = nRoomID;
		m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nPlayerState = enmPlayerState_InRoom;
		uint64_t nCurTime = CTimeValue::CurrentTime().Seconds();
		m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nEnterRoomTime = nCurTime;
		UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nRoomID), sizeof(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nRoomID));
		UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nPlayerState), sizeof(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nPlayerState));
		UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nEnterRoomTime), sizeof(m_arrPlayerRoomInfo[m_nCurPlayerRoomCount].nEnterRoomTime));

		++m_nCurPlayerRoomCount;
		UpdateDataInfo(GetMemberOffset(m_nCurPlayerRoomCount), sizeof(m_nCurPlayerRoomCount));
		return S_OK;
	}
    uint32_t GetEnterRoomTime(const RoomID nRoomID) const
    {
    	for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return m_arrPlayerRoomInfo[i].nEnterRoomTime;
			}
		}
    	return CTimeValue::CurrentTime().Seconds();
    }
	void SetConnInfo(const ConnUin &stInfo)
	{
		m_stConnInfo = stInfo;
		UpdateDataInfo(GetMemberOffset(m_stConnInfo), sizeof(ConnUin));
	}

	const ConnUin &GetConnInfo()
	{
		return m_stConnInfo;
	}
	NetType GetPlayerNetType()
	{
		return m_stConnInfo.nNetType;
	}

	int32_t SetRoleRankInRoom(const RoleRank nRoleRank,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nRoleRank=nRoleRank;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nRoleRank), sizeof(m_arrPlayerRoomInfo[i].nRoleRank));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}

	int32_t SetRoleAudioInRoom(const AVType nAVType,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nAudioStatus=nAVType;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nAudioStatus), sizeof(m_arrPlayerRoomInfo[i].nAudioStatus));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}
	int32_t SetRoleVideoInRoom(const AVType nAVType,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nVideoStatus=nAVType;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nVideoStatus), sizeof(m_arrPlayerRoomInfo[i].nVideoStatus));
				return S_OK;
			}
		}
		return E_RS_PLAYRENOTINROOM;
	}
	bool  PrivateMicIsOpened(RoleID nRoleID,RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				int32_t nWatchPrivateMicCount = m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
				for(int32_t j =0;j<nWatchPrivateMicCount;j++)
				{
					if(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j]==nRoleID)
					{
						return true;
					}
				}
			}
		}
		return false;

	}
	int32_t AddWatchPrivateMic(RoleID nRoleID,RoomID nRoomID,bool isOfficePlayer=false)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				int32_t nWatchPrivateMicCount = m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
				for(int32_t j =0;j<nWatchPrivateMicCount;j++)
				{
					if(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j]==nRoleID)
					{
						return S_OK;
					}
				}
				CVipInfo stVipInfo = g_SomeConfig.GetVipInfo(m_nVipLevel);
				int32_t nCanWatchMaxCount = stVipInfo.nMaxWatchCount;
				if(isOfficePlayer)
				{
					nCanWatchMaxCount = MaxWatchPrivateMicCount;
				}
                if(nWatchPrivateMicCount>=MaxWatchPrivateMicCount||nWatchPrivateMicCount>=nCanWatchMaxCount)
				{
                	WRITE_WARNING_LOG("player open private is to more{nRoleID=%d, nVipLevel=%d, nWatchCount=%d, nMaxWatchCount=%d}",m_nRoleID,m_nVipLevel,nWatchPrivateMicCount,nCanWatchMaxCount);
                	return E_RS_WATCHPRIVATEMICISTOBIG;
				}
                m_arrPlayerRoomInfo[i].arrWatchPrivateMic[nWatchPrivateMicCount] = nRoleID;
                UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[nWatchPrivateMicCount]), sizeof(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[nWatchPrivateMicCount]));

                ++m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
                UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nWatchPrivateMicCount), sizeof(m_arrPlayerRoomInfo[i].nWatchPrivateMicCount));

                return S_OK;
			}
		}
		WRITE_ERROR_LOG("open private:player is not in room{nRoleID=%d, nRoomID=%d}",m_nRoleID,nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	int32_t GetWatchPrivateMic(RoleID arrWatchRole[],const int32_t nArrSize,int32_t &nWatchCount,RoomID nRoomID)
	{
		nWatchCount = 0;
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				int32_t nWatchPrivateMicCount = m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
				int32_t j =0;
				for(j =0;j<nWatchPrivateMicCount&&nWatchCount<nArrSize;j++)
				{

					if(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j] != enmInvalidRoleID)
					{
						arrWatchRole[nWatchCount++] = m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j];
					}
				}

			}
		}
		return S_OK;
	}
	int32_t DelWatchPrivateMic(RoleID nRoleID,RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				int32_t nWatchPrivateMicCount = m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
				int32_t j =0;
				for(j =0;j<nWatchPrivateMicCount;j++)
				{
					if(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j] == nRoleID)
					{
						m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j] = m_arrPlayerRoomInfo[i].arrWatchPrivateMic[nWatchPrivateMicCount-1];
						m_arrPlayerRoomInfo[i].arrWatchPrivateMic[nWatchPrivateMicCount-1] = enmInvalidRoleID;
						UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j]), sizeof(m_arrPlayerRoomInfo[i].arrWatchPrivateMic[j]));

						--m_arrPlayerRoomInfo[i].nWatchPrivateMicCount;
						UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nWatchPrivateMicCount), sizeof(m_arrPlayerRoomInfo[i].nWatchPrivateMicCount));

						return S_OK;
					}
				}
				if(j==nWatchPrivateMicCount)
				{
                    return E_RS_PLAYERNOTWATCHTHISMIC;
				}

			}
		}
		return E_RS_PLAYRENOTINROOM;
	}

	void SetPlayerType(const PlayerType nPlayerType)
	{
		m_nPlayerType = nPlayerType;
		UpdateDataInfo(GetMemberOffset(m_nPlayerType), sizeof(m_nPlayerType));
	}
	PlayerType GetPlayerType()
	{
		return m_nPlayerType;
	}
	bool IsReboot()
	{
		return (m_nPlayerType==enmPlayerType_Rebot);
	}
	void SetUserClientInfo(const UserClientInfo nUserClientInfo)
	{
		m_nUserClientInfo = nUserClientInfo;
		UpdateDataInfo(GetMemberOffset(m_nUserClientInfo), sizeof(m_nUserClientInfo));
	}

	UserClientInfo GetUserClientInfo()
	{
		return m_nUserClientInfo;
	}

	bool IsHasCamera()
	{
		return m_nUserClientInfo&enmUserClientInfo_HasCamera;
	}

	bool IsHideEnter()
	{
		return m_nUserClientInfo&enmUserClientInfo_HideEnter;
	}
	void SetIdentityType(const IdentityType nIdentityType)
	{
		m_nIdentityType = nIdentityType;
		UpdateDataInfo(GetMemberOffset(m_nIdentityType), sizeof(m_nIdentityType));
	}

	IdentityType GetIdentityType()
	{
		return m_nIdentityType;
	}
	void SetOnlineTime(const uint32_t nOnlineTime)
	{
		m_nOnlineTime = nOnlineTime;
		UpdateDataInfo(GetMemberOffset(m_nOnlineTime), sizeof(m_nOnlineTime));
	}

	uint32_t GetOnlineTime()
	{
		return m_nOnlineTime;
	}
	void SetOnMicTime(const uint32_t nOnMicTime)
	{
		m_nOnMicTime = nOnMicTime;
		UpdateDataInfo(GetMemberOffset(m_nOnMicTime), sizeof(m_nOnMicTime));
	}

	uint32_t GetOnMicTime()
	{
		return m_nOnMicTime;
	}
	void SetItem(const ItemID arrItemID[], const uint16_t nCount)
	{
		m_nItemCount = MaxOnUserItemCount > nCount ? nCount : MaxOnUserItemCount;
		for(int32_t i =0;i<m_nItemCount;i++)
		{
			m_arrItemID[i] = arrItemID[i];
			UpdateDataInfo(GetMemberOffset(m_arrItemID[i]), sizeof(m_arrItemID[i]));
		}
		UpdateDataInfo(GetMemberOffset(m_nItemCount), sizeof(m_nItemCount));
	}
	void GetItem(ItemID arrItemID[],const uint16_t nSize,uint16_t &nCount)
	{
		nCount = nSize > m_nItemCount ? m_nItemCount : nSize;
		for(int32_t i =0;i<nCount;i++)
		{
			arrItemID[i] = m_arrItemID[i];
		}
	}

	bool IsOfficialPlayer()
	{
		return ((m_nIdentityType&enmIdentityType_OPERATOR)==enmIdentityType_OPERATOR)||((m_nIdentityType&enmIdentityType_PATROLMAN)==enmIdentityType_PATROLMAN);
	}

	void SetLastStaTime(const RoomID nRoomID,const uint32_t nLastStaTime)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nLastStaTime = nLastStaTime;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nLastStaTime), sizeof(m_arrPlayerRoomInfo[i].nLastStaTime));
			}
		}
	}

	uint32_t GetLastStaTime(const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return m_arrPlayerRoomInfo[i].nLastStaTime ;
			}
		}

		return CTimeValue::CurrentTime().Seconds();
	}

	void SetLastStaOnMicTime(const RoomID nRoomID,const uint32_t nLastStaOnMicTime)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].nLastStaOnMicTime = nLastStaOnMicTime;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].nLastStaOnMicTime), sizeof(m_arrPlayerRoomInfo[i].nLastStaOnMicTime));
			}
		}
	}

	uint32_t GetLastStaOnMicTime(const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID == nRoomID && (m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_PublicMic || m_arrPlayerRoomInfo[i].nPlayerState == enmPlayerState_PrivateMic))
			{
				return m_arrPlayerRoomInfo[i].nLastStaOnMicTime;
			}
		}

		return CTimeValue::CurrentTime().Seconds();
	}

	PlayerSelfSetting GetPlayerSelfSetting()
	{
		return m_nPlayerSelfSetting;
	}

	void SetPlayerSelfSetting(PlayerSelfSetting nPlayerSelfSetting)
	{
		m_nPlayerSelfSetting = nPlayerSelfSetting;
		UpdateDataInfo(GetMemberOffset(m_nPlayerSelfSetting), sizeof(m_nPlayerSelfSetting));
	}
	uint32_t GetStartPos()
	{
		return (uint32_t)offsetof(CPlayer, m_nStartPos);
	}

	uint32_t GetEndPos()
	{
		return (uint32_t)offsetof(CPlayer, m_nEndPos);
	}
	bool IsHaveUpdateData()
	{
		return m_nUpdateDataInfoCount > 0 ? true : false;
	}

	uint8_t* GetDataByOffset(uint32_t nDataOffset);

	int32_t GetUpdateDataInfoTable(uint8_t* arrUpdateData[], UpdateDataInfoTable arrUpdateTable[]);

	//合并玩家的在房间的信息
	void MergePlayerRoomInfo(PlayerRoomInfo arrPlayerRoomInfo[], int32_t nRoomInfoCount);

	void SetPlayerGlobalState(PlayerState nPlayerGlobalState)
	{
		m_nPlayerGlobalState = nPlayerGlobalState;
		UpdateDataInfo(GetMemberOffset(m_nPlayerGlobalState), sizeof(m_nPlayerGlobalState));
	}
	PlayerState GetPlayerGlobalState()
	{
		return m_nPlayerGlobalState;
	}

	bool IsOnMic()
	{
		return m_nPlayerGlobalState==enmPlayerState_PublicMic||m_nPlayerGlobalState==enmPlayerState_PrivateMic||m_nPlayerGlobalState==enmPlayerState_P2PMic;
	}

	void SetLoginTimes(uint32_t nLoginTimes)
	{
		m_nLoginTimes = nLoginTimes;
		UpdateDataInfo(GetMemberOffset(m_nLoginTimes), sizeof(m_nLoginTimes));
	}

	uint32_t  GetLoginTimes()
	{
		return m_nLoginTimes;
	}

	void SetKey(uint32_t nKey,const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				m_arrPlayerRoomInfo[i].m_nKey = nKey;
				UpdateDataInfo(GetMemberOffset(m_arrPlayerRoomInfo[i].m_nKey), sizeof(m_arrPlayerRoomInfo[i].m_nKey));
				break;
			}
		}
	}

	uint32_t GetKey(const RoomID nRoomID)
	{
		for(int32_t i = 0;i<m_nCurPlayerRoomCount;i++)
		{
			if(m_arrPlayerRoomInfo[i].nRoomID==nRoomID)
			{
				return m_arrPlayerRoomInfo[i].m_nKey;
			}
		}
		return 0;
	}

	void SetMagnateLevel(MagnateLevel nMagnateLevel)
	{
		m_nMagnateLevel = nMagnateLevel;
	}

	MagnateLevel GetMagnateLevel()
	{
		return m_nMagnateLevel;
	}

	ServerID GetGateServerID()
	{
		return m_nGateServerID;
	}

	void SetGateServerID(ServerID nServerID)
	{
		m_nGateServerID = nServerID;
	}

	//0表示没有收藏1表示收藏
	uint8_t IsCollected(RoomID nRoomID)
	{
		for(uint16_t i = 0; i < m_nCollectCount; ++i)
		{
			if(nRoomID == m_arrCollectRoomList[i])
			{
				return true;
			}
		}

		return false;
	}

	void SetCollectRoomList(uint16_t nCollectCount, RoomID arrRoomID[])
	{
		uint16_t nCount = nCollectCount > MaxCollectCount ? MaxCollectCount : nCollectCount;
		for(uint16_t i = 0; i < nCount; ++i)
		{
			m_arrCollectRoomList[i] = arrRoomID[i];
		}

		m_nCollectCount = nCount;
	}
private:
	void UpdateDataInfo(uint32_t nOffset, uint32_t nDataSize);
private:
	void Reset()
	{
		m_nRoleID = enmInvalidRoleID;
		m_nPlayerType = enmPlayerType_Normal;
		m_strRoleName.Initialize();
		m_nAccountID = enmInvalidAccountID;
		m_nPlayerGender = enmGender_Unknown;
		m_nVipLevel =enmVipLevel_NONE;
		m_nMoney = 0;
		m_nOnlineTime = 0;
		m_nOnMicTime = 0;
		m_nCurPlayerRoomCount = 0;
		m_nCurAdminRoomCount = 0;
		m_nLastHeatBeatTime = 0;
		m_nLoginTime = 0;
		m_nLogoutTime = 0;

		m_stConnInfo.Reset();
		m_nUserClientInfo = enmUserClientInfo_PC;
		m_nIdentityType = enmIdentityType_None;
		m_nItemCount = 0;
		for(int32_t i = 0; i < MaxOnUserItemCount; ++i)
		{
			m_arrItemID[i] = enmInvalidItemID;
		}
		m_nTotleEnterRoomCount = 0;

		m_nPlayerSelfSetting = 0;

		m_nPlayerGlobalState = enmInvalidPlayerState;
		m_nNewPlayerRoomCount = 0;
		m_nLoginTimes = 0;

		m_nUpdateDataInfoCount = 0;

		m_nMagnateLevel = enmMagnateLevel_0;

		m_nGateServerID = enmInvalidServerID;
		m_nCollectCount = 0;
	}
public:
	//注意，不要在m_nStartPos前添加成员变量
	uint8_t											m_nStartPos;
private:
	//注意，不要在m_nRoleID前添加成员变量
	RoleID 								m_nRoleID;
	PlayerType							m_nPlayerType;//玩家的类型（机器人否）
	CString<MaxRoleNameLength> 			m_strRoleName; //玩家昵称
	uint32_t 							m_nAccountID;   //179ID
	Gender 								m_nPlayerGender;
	VipLevel 							m_nVipLevel; //vip等级
	uint32_t 							m_nMoney;
	uint32_t 							m_nOnlineTime; //总共在线时长  (分钟)
	uint32_t 							m_nOnMicTime; //总共在麦上时长 (分钟)
	int32_t								m_nTotleEnterRoomCount;//hallserver统计的进入的总房间数
	uint32_t							m_nNewPlayerRoomCount;	//hallserver玩家进入的新手房数量
	int32_t 							m_nCurPlayerRoomCount; //当前进入的房间数
	PlayerRoomInfo 						m_arrPlayerRoomInfo[MaxEnterRoomCount]; //当前进入房间的信息
	int32_t 							m_nCurAdminRoomCount;
	RoomRankInfo 						m_arrRoomRankInfo[MaxBeAdminPerPlayer]; //是哪些房间的管理员
	uint64_t 							m_nLastHeatBeatTime; //最后一次收到心跳包的时间
	uint64_t 							m_nLoginTime; //登陆时间
	uint64_t 							m_nLogoutTime; //登出时间
	ConnUin 							m_stConnInfo; //玩家的连接信息
	UserClientInfo						m_nUserClientInfo; //玩家的客户端情况
	IdentityType                        m_nIdentityType;//身份
	uint16_t							m_nItemCount;   //正在被使用的物品个数
	ItemID                              m_arrItemID[MaxOnUserItemCount];
	PlayerSelfSetting					m_nPlayerSelfSetting; //玩家的个人设置
	PlayerState 						m_nPlayerGlobalState;			//玩家的状态(全局的麦的状态)
	uint32_t							m_nLoginTimes;					//成功登陆次数
	MagnateLevel						m_nMagnateLevel;				//玩家的财富等级

	ServerID							m_nGateServerID;				//玩家连的网关ID(也是直连的MediaserverID)
	uint16_t							m_nCollectCount;
	RoomID								m_arrCollectRoomList[MaxCollectCount];	//玩家收藏的房间列表

public:
	//注意，这个值只是标记作用，所有基本数据类型都要在此成员变量之前定义
	uint8_t											m_nEndPos;

private:
	int32_t											m_nUpdateDataInfoCount;
	UpdateDataInfoTable								m_arrUpdateDataInfoTable[UpdateDataInfoTableSize];
};

FRAME_ROOMSERVER_NAMESPACE_END

CCOMPLEXTYPE_SPECIFICALLY(FRAME_ROOMSERVER_NAMESPACE::CPlayer)

#endif /* DAL_PLAYER_H_ */
