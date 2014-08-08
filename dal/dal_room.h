/*
 * dal_room.h
 *
 *  Created on: 2011-11-30
 *      Author: jimm
 */

#ifndef DAL_ROOM_H_
#define DAL_ROOM_H_

#include "common/common_complextype.h"
#include "common/common_hashlist.h"
#include "common/common_string.h"
#include "dal/dal_player.h"
#include "def/server_namespace.h"
#include "def/server_errordef.h"
#include "public_typedef.h"
#include "config/someconfig.h"
#include "config/rebotconfig.h"
#include "dal/dal_updatedatainfotable.h"
#include "ctl/update_data_ctl.h"
#include "def/dal_def.h"
#include <bitset>
#include <set>
#include <map>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

using namespace std;

class CArtistInfo
{
public:
	RoomUserInfo   nRoomUserInfo;           //玩家信息
	uint8_t        nOnlineStat;             //在线状态
};

class CRoomArtist
{
public:
	uint16_t         nArtistCount;
	CArtistInfo     arrArtistInfo[MAX_ARTIST_NUM];
public:
	CRoomArtist()
	{
		nArtistCount = 0;
		memset(arrArtistInfo, 0, sizeof(arrArtistInfo));
	}
	~CRoomArtist()
	{

	}
	void AddArtistInfo(RoomUserInfo  nRoomUserInfo, int32_t nOnlineStat)
	{
		if(nArtistCount < MAX_ARTIST_NUM)
		{
			arrArtistInfo[nArtistCount].nRoomUserInfo = nRoomUserInfo;


			arrArtistInfo[nArtistCount].nOnlineStat = nOnlineStat;
			nArtistCount++;
		}
	}
	uint16_t GetOfflineArtistList(RoomUserInfo * arrArtistList)
	{
		uint16_t nOffLineCount = 0;
		for(int32_t i = 0 ; i < nArtistCount; ++i)
		{
			if(arrArtistInfo[i].nOnlineStat == enmOffline_stat)
			{
				arrArtistList[nOffLineCount] = arrArtistInfo[i].nRoomUserInfo;

				nOffLineCount++;
			}
		}
		return nOffLineCount;
	}
	void DelArtistInfo(RoleID nRoleID)
	{
		for(int32_t i = 0; i < nArtistCount; ++i)
		{
			if(arrArtistInfo[i].nRoomUserInfo.nRoleID == nRoleID && nArtistCount > 0)
			{
				arrArtistInfo[i].nRoomUserInfo = arrArtistInfo[nArtistCount-1].nRoomUserInfo;

				arrArtistInfo[i].nOnlineStat = arrArtistInfo[nArtistCount-1].nOnlineStat;
				nArtistCount--;
			}
		}
	}
	void SetOnlineStat(RoleID nRoleID, int32_t nOnlineStat)
	{
		for(int32_t i = 0; i < nArtistCount; ++i)
		{
			if(arrArtistInfo[i].nRoomUserInfo.nRoleID == nRoleID )
			{
				arrArtistInfo[i].nOnlineStat = nOnlineStat;
			}
		}
	}
};

class CPlayer;

typedef CHashList<PlayerIndex, MaxUserCountPerRoom, RoomPlayerBucketSize> RoomPlayerIndexList;
//typedef CHashList<PlayerIndex, MaxAdminCountPerRoom, RoomAdminBucketSize> RoomAdminIndexList;

//上麦通知的Mediaserver
typedef set<ServerID> ServerSet;
typedef map<RoleID, ServerSet> MediaserverMap;	//map<speakerID, MediaserverID的集合>

static int32_t nLeftTimeTemp = 0;

class OnPublicMicInfo
{
public:
	RoleID m_nRoleID;
	uint32_t m_nEndMicTime;
	bool isOpened;        //是否被关闭
	OnPublicMicInfo()
	{
		Reset();
	}

	void Reset()
	{
		m_nRoleID = enmInvalidRoleID;
		m_nEndMicTime = 0;
		isOpened = true;
	}
};
class KickList
{
public:
	RoleID m_nRoleID;
	uint32_t m_nEndKickTime;
	CString<MaxKickedReasonLength> m_strReason;

	KickList()
	{
		Reset();
	}

	void Reset()
	{
		m_nRoleID = enmInvalidRoleID;
		m_nEndKickTime = 0;
		m_strReason.Initialize();
	}
};

class MemberInfo
{
public:
	RoleID m_nRoleID;
	RoleRank m_nRoleRank;
	uint32_t m_nTotalTime;
	uint32_t m_nMicTime;


	MemberInfo()
	{
		Reset();
	}

	void Reset()
	{
		m_nRoleID = enmInvalidRoleID;
		m_nRoleRank = enmRoleRank_None;
		m_nTotalTime = 0;
		m_nMicTime = 0;
	}
};
class LockIpInfo
{
public:
	uint32_t m_nIP;
	uint32_t m_nEndLockTime;
	LockIpInfo()
	{
		Reset();
	}
	void Reset()
	{
		m_nIP = 0;
		m_nEndLockTime = 0;
	}
};

class ProhibitSpeakList
{
public:
	RoleID m_nRoleID;
	uint32_t m_nEndKickTime;

	ProhibitSpeakList()
	{
		Reset();
	}

	void Reset()
	{
		m_nRoleID = enmInvalidRoleID;
		m_nEndKickTime = 0;
	}
};


class CRoom : public CObject
{
public:

	CRoom()
	{
		Reset();
	}

	virtual ~CRoom()
	{

	}

	int32_t Initialize()
	{
		Reset();
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

	int32_t AddPlayer(const PlayerIndex nPlayerIndex, VipLevel nVipLevel, bool isHideEnter = false);

	int32_t DeletePlayer(const PlayerIndex nPlayerIndex, VipLevel nVipLevel, bool isHideEnter = false);

	bool IsPlayerInRoom(const PlayerIndex nPlayerIndex);

	int32_t GetAllPlayers(PlayerIndex arrPlayerIndex[], const int32_t arrSize, int32_t &nPlayerCount);

	void SetRegionID(const RegionID nRegionID)
	{
		m_nRegionID = nRegionID;
		UpdateDataInfo(GetMemberOffset(m_nRegionID), sizeof(m_nRegionID));
	}

	RegionID GetRegionID() const
	{
		return m_nRegionID;
	}

	void SetChannelID(const ChannelID nChannelID)
	{
		m_nChannelID = nChannelID;
		UpdateDataInfo(GetMemberOffset(m_nChannelID), sizeof(m_nChannelID));
	}

	ChannelID GetChannelID() const
	{
		return m_nChannelID;
	}

	RoleID GetHostID() const
	{
		return m_nHostID;
	}

	void SetHostID(const RoleID nRoleID)
	{
		m_nHostID = nRoleID;
		UpdateDataInfo(GetMemberOffset(m_nHostID), sizeof(m_nHostID));
	}

	int32_t GetCurUserCount() const
	{
		return m_sRoomPlayerIndexList.ObjectCount();
	}
	int32_t GetCurVisitorCount() const
	{
		return m_nCurVisitorCount;
	}

	void AddVisitorCount()
	{
		++m_nCurVisitorCount;
	}

	void DelVisitorCount()
	{
		if(m_nCurVisitorCount > 0)
		{
			--m_nCurVisitorCount;
		}
	}

	int32_t GetTotalPeopleCount() const
	{
		return m_sRoomPlayerIndexList.ObjectCount()/* + m_nCurVisitorCount*/;
	}

	int32_t GetMaxRealUserLimit() const
	{
		return m_nMaxRealUserLimit;
	}

	int32_t GetMaxShowUserLimit() const
	{
		return m_nMaxShowUserLimit;
	}
	void SetMaxRealUserLimit(const uint16_t nMaxShowUserLimit)
	{
		m_nMaxRealUserLimit = nMaxShowUserLimit;
		UpdateDataInfo(GetMemberOffset(m_nMaxRealUserLimit), sizeof(m_nMaxRealUserLimit));
	}

	void SetMaxShowUserLimit(const uint16_t nMaxRealUserLimit)
	{
		m_nMaxShowUserLimit = nMaxRealUserLimit;
		UpdateDataInfo(GetMemberOffset(m_nMaxShowUserLimit), sizeof(m_nMaxShowUserLimit));
	}
	RoomType GetRoomType() const
	{
		return m_nRoomType;
	}

	void SetRoomID(const RoomID nRoomID)
	{
		m_nRoomID = nRoomID;

		UpdateDataInfo(GetMemberOffset(m_nRoomID), sizeof(m_nRoomID));
	}

	RoomID GetRoomID() const
	{
		return m_nRoomID;
	}

	const char *GetRoomName()
	{
		return m_strRoomName.GetString();
	}

	void SetRoomName(const char *strRoomName)
	{
		m_strRoomName = strRoomName;

		UpdateDataInfo(GetMemberOffset(m_strRoomName), sizeof(m_strRoomName));
	}

	void SetRoomFund(const uint32_t nRoomFund)
	{
		m_nRoomFund = nRoomFund;

		UpdateDataInfo(GetMemberOffset(m_nRoomFund), sizeof(m_nRoomFund));
	}

	uint32_t GetRoomFund() const
	{
		return m_nRoomFund;
	}

	void SetRoomOption(const uint32_t nRoomOption)
	{
		m_nRoomOption = nRoomOption;

		UpdateDataInfo(GetMemberOffset(m_nRoomOption), sizeof(m_nRoomOption));
	}

	void SetRoomHasOprivateMic()
	{
		m_nRoomOption |= enmRoomOptionType_Has_PrivateMic;

		UpdateDataInfo(GetMemberOffset(m_nRoomOption), sizeof(m_nRoomOption));
	}
	void SetRoomHasNotOprivateMic()
	{
		m_nRoomOption &= ~enmRoomOptionType_Has_PrivateMic;

		UpdateDataInfo(GetMemberOffset(m_nRoomOption), sizeof(m_nRoomOption));
	}
	uint32_t GetRoomOption() const
	{
		return m_nRoomOption;
	}

	void SetMicroTime(const uint32_t nMicroTime)
	{
		m_nMicroTime = nMicroTime;

		UpdateDataInfo(GetMemberOffset(m_nMicroTime), sizeof(m_nMicroTime));
	}

	uint32_t GetMicroTime() const
	{
		return m_nMicroTime;
	}

	uint32_t GetMicTimeByRoleRank(RoleRank nRoleRank)
	{
		uint32_t onMicTime = m_nMicroTime;
		//临管以上一般不受时间限制
		if(nRoleRank>=enmRoleRank_TempAdmin)
		{
			onMicTime = 0;
			//自由排麦打开的时候临管的上麦时间要受限制
			if(IsFreeShowOpened()&&nRoleRank==enmRoleRank_TempAdmin)
			{
				onMicTime = m_nMicroTime;
			}
			//自由排麦打开的时候限制管理员麦时打开的时候管理员的卖时也是受限制的
			else if(nRoleRank==enmRoleRank_Admin&&!(IsOpenAdminOnMicTime())&&IsFreeShowOpened())
			{
				onMicTime = m_nMicroTime;
			}
		}
		return onMicTime;
	}

	void SetMediaAddressPort(uint32_t arrMediaAddress[], uint16_t arrMediaPort[])
	{
		m_arrMediaAddress[enmNetType_Tel] = arrMediaAddress[enmNetType_Tel];
		m_arrMediaAddress[enmNetType_Cnc] = arrMediaAddress[enmNetType_Cnc];
		m_arrMediaPort[enmNetType_Tel] = arrMediaPort[enmNetType_Tel];
		m_arrMediaPort[enmNetType_Cnc] = arrMediaPort[enmNetType_Cnc];

		UpdateDataInfo(GetMemberOffset(m_arrMediaAddress), sizeof(m_arrMediaAddress));
		UpdateDataInfo(GetMemberOffset(m_arrMediaPort), sizeof(m_arrMediaPort));
	}

	bool GetMediaAddressPort(uint32_t arrMediaAddress[], uint16_t arrMediaPort[])
	{
		if(m_arrMediaServerID[enmNetType_Tel] == enmInvalidServerID || m_arrMediaServerID[enmNetType_Cnc] == enmInvalidServerID)
		{
			return false;
		}
		if((m_arrMediaAddress[enmNetType_Tel] != 0 && m_arrMediaAddress[enmNetType_Cnc] != 0)
				|| (m_arrMediaPort[enmNetType_Tel] != 0 && m_arrMediaPort[enmNetType_Cnc] != 0))
		{
			arrMediaAddress[enmNetType_Tel] = m_arrMediaAddress[enmNetType_Tel];
			arrMediaAddress[enmNetType_Cnc] = m_arrMediaAddress[enmNetType_Cnc];
			arrMediaPort[enmNetType_Tel] = m_arrMediaPort[enmNetType_Tel];
			arrMediaPort[enmNetType_Cnc] = m_arrMediaPort[enmNetType_Cnc];
			return true;
		}

		return false;
	}

	void SetMediaServerID(ServerID arrMediaServerID[])
	{
		memcpy(m_arrMediaServerID, arrMediaServerID, sizeof(m_arrMediaServerID));

		UpdateDataInfo(GetMemberOffset(m_arrMediaServerID), sizeof(m_arrMediaServerID));
	}

	int32_t GetMediaServerIDByType(const NetType nNetType)
	{
         return m_arrMediaServerID[nNetType];
	}
	int32_t AddKickList(const RoleID nRoleID, const uint32_t nEndKickTime, const char *strReason = "default")
	{
		if(m_nCurKickCount >= MaxRoomKickListSize)
		{
			return E_RS_KICKLISTISFULL;
		}

		for(uint16_t i = 0; i < m_nCurKickCount; ++i)
		{
			if(m_arrKickList[i].m_nRoleID == nRoleID)
			{
				m_arrKickList[i].m_nEndKickTime = nEndKickTime;
				m_arrKickList[i].m_strReason = strReason;

				UpdateDataInfo(GetMemberOffset(m_arrKickList[i]), sizeof(m_arrKickList[i]));

				return S_OK;
			}
		}

		m_arrKickList[m_nCurKickCount].m_nRoleID = nRoleID;
		m_arrKickList[m_nCurKickCount].m_nEndKickTime = nEndKickTime;
		m_arrKickList[m_nCurKickCount].m_strReason = strReason;
		UpdateDataInfo(GetMemberOffset(m_arrKickList[m_nCurKickCount]), sizeof(m_arrKickList[m_nCurKickCount]));

		++m_nCurKickCount;
		UpdateDataInfo(GetMemberOffset(m_nCurKickCount), sizeof(m_nCurKickCount));

		return S_OK;
	}

	uint32_t GetEndKickTime(const RoleID nRoleID)
	{
		DelTimerOutKickList();
		for(uint16_t i = 0; i < m_nCurKickCount; ++i)
		{
			if(m_arrKickList[i].m_nRoleID == nRoleID)
			{
				return m_arrKickList[i].m_nEndKickTime;
			}
		}

		return 0;
	}

	int32_t DelTimerOutKickList()
	{
		int32_t nCurKickCount = m_nCurKickCount;
		for(int32_t i = 0; i < nCurKickCount; ++i)
		{
			if(i >= m_nCurKickCount)
			{
				break;
			}

			uint32_t nEndTime = m_arrKickList[i].m_nEndKickTime;
			if(nEndTime <= (uint32_t)CTimeValue::CurrentTime().Seconds() && nEndTime != 0)
			{
				m_arrKickList[i] = m_arrKickList[m_nCurKickCount - 1];
				m_arrKickList[m_nCurKickCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrKickList[m_nCurKickCount - 1].m_nEndKickTime = 0;
				m_arrKickList[m_nCurKickCount - 1].m_strReason = "";

				--m_nCurKickCount;

				UpdateDataInfo(GetMemberOffset(m_nCurKickCount), sizeof(m_nCurKickCount));
				UpdateDataInfo(GetMemberOffset(m_arrKickList[i]), sizeof(m_arrKickList[i]));

				i--;
			}
		}

		return S_OK;
	}

	void DeleteKickPlayer(const RoleID nRoleID)
	{
		for(uint16_t i = 0; i < m_nCurKickCount; ++i)
		{
			if(m_arrKickList[i].m_nRoleID == nRoleID)
			{
				m_arrKickList[i] = m_arrKickList[m_nCurKickCount - 1];
				m_arrKickList[m_nCurKickCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrKickList[m_nCurKickCount - 1].m_nEndKickTime = 0;
				m_arrKickList[m_nCurKickCount - 1].m_strReason = "";

				--m_nCurKickCount;

				UpdateDataInfo(GetMemberOffset(m_arrKickList[i]), sizeof(m_arrKickList[i]));
				UpdateDataInfo(GetMemberOffset(m_nCurKickCount), sizeof(m_nCurKickCount));
			}
		}
	}

	int32_t AddBlackList(const RoleID nRoleID)
	{
		if(m_nCurBlackCount >= MaxRoomBlackListSize)
		{
			return E_RS_BLACKLISTISFULL;
		}

		for(uint16_t i = 0; i < m_nCurBlackCount; ++i)
		{
			if(m_arrBlackList[i] == nRoleID)
			{
				return S_OK;
			}
		}

		m_arrBlackList[m_nCurBlackCount] = nRoleID;
		UpdateDataInfo(GetMemberOffset(m_arrBlackList[m_nCurBlackCount]), sizeof(m_arrBlackList[m_nCurBlackCount]));

		++m_nCurBlackCount;
		UpdateDataInfo(GetMemberOffset(m_nCurBlackCount), sizeof(m_nCurBlackCount));

		return S_OK;
	}

	void DelPlayerFromBlackList(const RoleID nRoleID)
	{
		for(uint16_t i = 0; i < m_nCurBlackCount; ++i)
		{
			if(m_arrBlackList[i] == nRoleID)
			{
				m_nCurBlackCount--;
				m_arrBlackList[i] = m_arrBlackList[m_nCurBlackCount];
				m_arrBlackList[m_nCurBlackCount] = enmInvalidRoleID;
				break;
			}
		}
	}
	bool IsInBlackList(const RoleID nRoleID)
	{
		for(uint16_t i = 0; i < m_nCurBlackCount; ++i)
		{
			if(m_arrBlackList[i] == nRoleID)
			{
				return true;
			}
		}

		return false;
	}

	void SetBlackList(const RoleID arrBlackRoleID[],const uint32_t nInBlackPlayerCount)
	{
		m_nCurBlackCount = MaxRoomBlackListSize > nInBlackPlayerCount ? nInBlackPlayerCount : MaxRoomBlackListSize;
		for(int32_t i =0;i<m_nCurBlackCount;i++)
		{
			m_arrBlackList[i] = arrBlackRoleID[i];
			UpdateDataInfo(GetMemberOffset(m_arrBlackList[i]), sizeof(m_arrBlackList[i]));
		}

		UpdateDataInfo(GetMemberOffset(m_nCurBlackCount), sizeof(m_nCurBlackCount));
	}
	int32_t AddProhibitList(const RoleID nRoleID,RoleID &nDelRoleID)
	{
		nDelRoleID = enmInvalidRoleID;
		if(m_nCurProhibitCount >= MaxRoomProhibitSpeakSize)
		{
			nDelRoleID = m_arrProhibitSpeakList[0].m_nRoleID;
			//删除第一个玩家，并加入到最后
			for(int32_t i = 0;i < m_nCurProhibitCount-1;i++)
			{
				m_arrProhibitSpeakList[i].m_nRoleID = m_arrProhibitSpeakList[i+1].m_nRoleID;
				m_arrProhibitSpeakList[i].m_nEndKickTime = m_arrProhibitSpeakList[i+1].m_nEndKickTime;
			}
			m_nCurProhibitCount--;
		}

		for(uint16_t i = 0; i < m_nCurProhibitCount; ++i)
		{
			if(m_arrProhibitSpeakList[i].m_nRoleID == nRoleID)
			{
				return S_OK;
			}
		}

		int32_t nEndKickTime = (uint32_t)CTimeValue::CurrentTime().Seconds() + g_SomeConfig.GetShutUpTime();

		m_arrProhibitSpeakList[m_nCurProhibitCount].m_nRoleID = nRoleID;
		m_arrProhibitSpeakList[m_nCurProhibitCount].m_nEndKickTime = nEndKickTime;
		UpdateDataInfo(GetMemberOffset(m_arrProhibitSpeakList[m_nCurProhibitCount]), sizeof(m_arrProhibitSpeakList[m_nCurProhibitCount]));

		++m_nCurProhibitCount;
		UpdateDataInfo(GetMemberOffset(m_nCurProhibitCount), sizeof(m_nCurProhibitCount));

		return S_OK;
	}

	int32_t DelProhibitList(const RoleID nRoleID)
	{
		for(uint16_t i = 0; i < m_nCurProhibitCount; ++i)
		{
			if(m_arrProhibitSpeakList[i].m_nRoleID == nRoleID)
			{
				for(int32_t j = i;j < m_nCurProhibitCount-1;j++)
				{
					m_arrProhibitSpeakList[j].m_nRoleID = m_arrProhibitSpeakList[j+1].m_nRoleID;
					m_arrProhibitSpeakList[j].m_nEndKickTime = m_arrProhibitSpeakList[j+1].m_nEndKickTime;
					UpdateDataInfo(GetMemberOffset(m_arrProhibitSpeakList[j]), sizeof(m_arrProhibitSpeakList[j]));
				}
				m_arrProhibitSpeakList[m_nCurProhibitCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrProhibitSpeakList[m_nCurProhibitCount - 1].m_nEndKickTime = 0;

				UpdateDataInfo(GetMemberOffset(m_arrProhibitSpeakList[m_nCurProhibitCount - 1]), sizeof(m_arrProhibitSpeakList[m_nCurProhibitCount - 1]));
				--m_nCurProhibitCount;
				UpdateDataInfo(GetMemberOffset(m_nCurProhibitCount), sizeof(m_nCurProhibitCount));
				return S_OK;
			}
		}

		return E_RS_NOTINSHUTUPLIST;
	}

	bool IsInProhibitList(const RoleID nRoleID,int32_t &nLeftTime = nLeftTimeTemp)
	{
//		DelTimerOutProhibitList();
		for(uint16_t i = 0; i < m_nCurProhibitCount; ++i)
		{
			if(m_arrProhibitSpeakList[i].m_nRoleID == nRoleID)
			{
				nLeftTime = m_arrProhibitSpeakList[i].m_nEndKickTime - (uint32_t)CTimeValue::CurrentTime().Seconds();
				if(nLeftTime > 0)
				{
					return true;
				}
				return false;
			}
		}

		return false;
	}

	int32_t DelTimerOutProhibitList(RoleID arrRoleID[],int32_t &nDelCount,int32_t nArrSize)
	{
		nDelCount = 0;
		int32_t nCurProhibitCount = m_nCurProhibitCount;
		for(int32_t i = 0; i < nCurProhibitCount; ++i)
		{
			if(i >= m_nCurProhibitCount)
			{
				break;
			}

			uint32_t nEndTime = m_arrProhibitSpeakList[i].m_nEndKickTime;
			if(nEndTime <= (uint32_t)CTimeValue::CurrentTime().Seconds() && nEndTime != 0)
			{
				if(nDelCount < nArrSize)
				{
					arrRoleID[nDelCount] = m_arrProhibitSpeakList[i].m_nRoleID;
					nDelCount++;
				}

				m_arrProhibitSpeakList[i] = m_arrProhibitSpeakList[m_nCurProhibitCount - 1];
				m_arrProhibitSpeakList[m_nCurProhibitCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrProhibitSpeakList[m_nCurProhibitCount - 1].m_nEndKickTime = 0;

				--m_nCurProhibitCount;

				UpdateDataInfo(GetMemberOffset(m_nCurProhibitCount), sizeof(m_nCurProhibitCount));
				UpdateDataInfo(GetMemberOffset(m_arrProhibitSpeakList[i]), sizeof(m_arrProhibitSpeakList[i]));
				//i--以便再次检查移到i上的新数据
				i--;
			}
		}

		return S_OK;
	}

	int32_t AddLockIPList(const uint32_t nIP,const uint32_t nEndLockTime)
	{
		for(uint16_t i = 0; i < m_nCurLockIPCount; ++i)
		{
			if(m_arrLockIP[i].m_nEndLockTime <= (uint32_t)CTimeValue::CurrentTime().Seconds() && m_arrLockIP[i].m_nEndLockTime != 0)
			{
				m_arrLockIP[i] = m_arrLockIP[m_nCurLockIPCount - 1];
				m_arrLockIP[m_nCurLockIPCount - 1].Reset();
				UpdateDataInfo(GetMemberOffset(m_arrLockIP[i]), sizeof(m_arrLockIP[i]));

				--m_nCurLockIPCount;
				i--;
				UpdateDataInfo(GetMemberOffset(m_nCurLockIPCount), sizeof(m_nCurLockIPCount));
			}
		}
		if(m_nCurLockIPCount >= MaxRoomLockIP)
		{
			return E_RS_LOCKIPLISTISFULL;
		}

		for(uint16_t i = 0; i < m_nCurLockIPCount; ++i)
		{
			if(m_arrLockIP[i].m_nIP == nIP)
			{
				m_arrLockIP[i].m_nEndLockTime = nEndLockTime;
				UpdateDataInfo(GetMemberOffset(m_arrLockIP[i]), sizeof(m_arrLockIP[i]));
				return S_OK;
			}
		}

		m_arrLockIP[m_nCurLockIPCount].m_nIP = nIP;
		m_arrLockIP[m_nCurLockIPCount].m_nEndLockTime = nEndLockTime;
		UpdateDataInfo(GetMemberOffset(m_arrLockIP[m_nCurLockIPCount]), sizeof(m_arrLockIP[m_nCurLockIPCount]));

		++m_nCurLockIPCount;
		UpdateDataInfo(GetMemberOffset(m_nCurLockIPCount), sizeof(m_nCurLockIPCount));

		return S_OK;
	}

	bool IsInLockIPList(const uint32_t nIP,uint32_t &nEndLockTime)
	{
		int32_t nCurLockIPCount = m_nCurLockIPCount;
		for(int32_t i = 0; i < nCurLockIPCount; ++i)
		{
			if(m_arrLockIP[i].m_nEndLockTime <= (uint32_t)CTimeValue::CurrentTime().Seconds() && m_arrLockIP[i].m_nEndLockTime != 0)
			{
				m_arrLockIP[i] = m_arrLockIP[m_nCurLockIPCount - 1];
				m_arrLockIP[m_nCurLockIPCount - 1].Reset();
				UpdateDataInfo(GetMemberOffset(m_arrLockIP[i]), sizeof(m_arrLockIP[i]));

				--m_nCurLockIPCount;
				i--;
				UpdateDataInfo(GetMemberOffset(m_nCurLockIPCount), sizeof(m_nCurLockIPCount));
			}
		}
		for(int32_t i = 0; i < m_nCurLockIPCount; ++i)
		{
			if(m_arrLockIP[i].m_nIP == nIP)
			{
				nEndLockTime = m_arrLockIP[i].m_nEndLockTime;
				return true;
			}
		}

		return false;
	}

	bool IsNeedPassWord()
	{
		return !(m_strPassword.Length() == 0);
	}

	void SetMemberList(const RoleID arrRoleID[], const RoleRank arrRoleRank[],
			const uint32_t arrTotalTime[],const uint32_t arrOnMicTime[], const uint16_t nMemberCount)
	{
		m_nCurMemberCount = MaxAdminCountPerRoom > nMemberCount ? nMemberCount : MaxAdminCountPerRoom;

		UpdateDataInfo(GetMemberOffset(m_nCurMemberCount), sizeof(m_nCurMemberCount));

		for(int32_t i = 0; i < m_nCurMemberCount; ++i)
		{
			m_arrMemberList[i].m_nRoleID = arrRoleID[i];
			m_arrMemberList[i].m_nRoleRank = arrRoleRank[i];
			m_arrMemberList[i].m_nTotalTime = arrTotalTime[i];
			m_arrMemberList[i].m_nMicTime = arrOnMicTime[i];

			UpdateDataInfo(GetMemberOffset(m_arrMemberList[i]), sizeof(m_arrMemberList[i]));

			switch(m_arrMemberList[i].m_nRoleRank)
			{
				case enmRoleRank_TempAdmin:
				{
					++m_nCurTempAdmCount;
					UpdateDataInfo(GetMemberOffset(m_nCurTempAdmCount), sizeof(m_nCurTempAdmCount));
				}
				break;
				case enmRoleRank_Admin:
				{
					++m_nCurAdmCount;
					UpdateDataInfo(GetMemberOffset(m_nCurAdmCount), sizeof(m_nCurAdmCount));
				}
				break;
				case enmRoleRank_MinorHost:
				{
					++m_nCurSecHostCount;
					UpdateDataInfo(GetMemberOffset(m_nCurSecHostCount), sizeof(m_nCurSecHostCount));
				}
				break;
			}
		}
	}
    int32_t AddMemberList(const RoleID nRoleID,const RoleRank nRoleRank,const uint32_t nTotalTime = 0,const uint32_t nMicTime = 0)
    {
    	if(m_nCurMemberCount>=MaxAdminCountPerRoom)
    	{
             return E_RS_ADMININROOMTOBIG;
    	}
    	int32_t ret = S_OK;
    	bool isInMemBer = false;
    	RoleRank nRoleRankTemp ;
    	for(int32_t i = 0 ;i<m_nCurMemberCount;i++)
    	{
    		if(m_arrMemberList[i].m_nRoleID==nRoleID)
    		{
    			isInMemBer = true;
    			nRoleRankTemp = m_arrMemberList[i].m_nRoleRank;
    			m_arrMemberList[i].m_nRoleRank = nRoleRank;
    			UpdateDataInfo(GetMemberOffset(m_arrMemberList[i].m_nRoleRank), sizeof(m_arrMemberList[i].m_nRoleRank));
    			return S_OK;
    		}
    	}
    	m_arrMemberList[m_nCurMemberCount].m_nRoleID = nRoleID;
    	m_arrMemberList[m_nCurMemberCount].m_nRoleRank = nRoleRank;
    	m_arrMemberList[m_nCurMemberCount].m_nTotalTime = nTotalTime;
    	m_arrMemberList[m_nCurMemberCount].m_nMicTime = nMicTime;

    	UpdateDataInfo(GetMemberOffset(m_arrMemberList[m_nCurMemberCount]), sizeof(m_arrMemberList[m_nCurMemberCount]));

    	switch(nRoleRank)
		{
			case enmRoleRank_TempAdmin:
			{
				++m_nCurTempAdmCount;
				UpdateDataInfo(GetMemberOffset(m_nCurTempAdmCount), sizeof(m_nCurTempAdmCount));
			}
			break;
			case enmRoleRank_Admin:
			{
				++m_nCurAdmCount;
				UpdateDataInfo(GetMemberOffset(m_nCurAdmCount), sizeof(m_nCurAdmCount));
			}
			break;
			case enmRoleRank_MinorHost:
			{
				++m_nCurSecHostCount;
				UpdateDataInfo(GetMemberOffset(m_nCurSecHostCount), sizeof(m_nCurSecHostCount));
			}
			break;
			default:
			break;
		}
    	if(isInMemBer)
    	{
    		switch(nRoleRankTemp)
			{
				case enmRoleRank_TempAdmin:
				{
					--m_nCurTempAdmCount;
					UpdateDataInfo(GetMemberOffset(m_nCurTempAdmCount), sizeof(m_nCurTempAdmCount));
				}
				break;
				case enmRoleRank_Admin:
				{
					--m_nCurAdmCount;
					UpdateDataInfo(GetMemberOffset(m_nCurAdmCount), sizeof(m_nCurAdmCount));
				}
				break;
				case enmRoleRank_MinorHost:
				{
					--m_nCurSecHostCount;
					UpdateDataInfo(GetMemberOffset(m_nCurSecHostCount), sizeof(m_nCurSecHostCount));
				}
				break;
				default:
				break;
			}

    	}
    	else
    	{
    		++m_nCurMemberCount;
    		UpdateDataInfo(GetMemberOffset(m_nCurMemberCount), sizeof(m_nCurMemberCount));
    	}

    	return ret;
    }

    int32_t DelMemberList(const RoleID nRoleID,const RoleRank nRoleRank)
	{
		for(int32_t i = 0;i<m_nCurMemberCount;i++)
		{
			if(m_arrMemberList[i].m_nRoleID==nRoleID&&m_arrMemberList[i].m_nRoleRank==nRoleRank)
			{
				m_arrMemberList[i].m_nRoleID = m_arrMemberList[m_nCurMemberCount - 1].m_nRoleID;
				m_arrMemberList[i].m_nRoleRank = m_arrMemberList[m_nCurMemberCount - 1].m_nRoleRank;
				m_arrMemberList[i].m_nTotalTime = m_arrMemberList[m_nCurMemberCount - 1].m_nTotalTime;
				m_arrMemberList[m_nCurMemberCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrMemberList[m_nCurMemberCount - 1].m_nRoleRank = enmRoleRank_None;
				m_arrMemberList[m_nCurMemberCount - 1].m_nTotalTime = 0;
				m_arrMemberList[m_nCurMemberCount - 1].m_nMicTime = 0;

				UpdateDataInfo(GetMemberOffset(m_arrMemberList[i]), sizeof(m_arrMemberList[i]));

				--m_nCurMemberCount;
				UpdateDataInfo(GetMemberOffset(m_nCurMemberCount), sizeof(m_nCurMemberCount));

				switch(nRoleRank)
				{
					case enmRoleRank_TempAdmin:
					{
						--m_nCurTempAdmCount;
						UpdateDataInfo(GetMemberOffset(m_nCurTempAdmCount), sizeof(m_nCurTempAdmCount));
					}
					break;
					case enmRoleRank_Admin:
					{
						--m_nCurAdmCount;
						UpdateDataInfo(GetMemberOffset(m_nCurAdmCount), sizeof(m_nCurAdmCount));
					}
					break;
					case enmRoleRank_MinorHost:
					{
						--m_nCurSecHostCount;
						UpdateDataInfo(GetMemberOffset(m_nCurSecHostCount), sizeof(m_nCurSecHostCount));
					}
					break;
					default:
					break;
				}

				return S_OK;
			}
		}
		return E_RS_ISNOTINMEMBERLIST;
	}

    int32_t DelMemberList(const RoleID nRoleID)
	{
		for(int32_t i = 0;i<m_nCurMemberCount;i++)
		{
			if(m_arrMemberList[i].m_nRoleID==nRoleID)
			{
				RoleRank nRoleRank = m_arrMemberList[i].m_nRoleRank;
				m_arrMemberList[i].m_nRoleID = m_arrMemberList[m_nCurMemberCount - 1].m_nRoleID;
				m_arrMemberList[i].m_nRoleRank = m_arrMemberList[m_nCurMemberCount - 1].m_nRoleRank;
				m_arrMemberList[i].m_nTotalTime = m_arrMemberList[m_nCurMemberCount - 1].m_nTotalTime;
				m_arrMemberList[m_nCurMemberCount - 1].m_nRoleID = enmInvalidRoleID;
				m_arrMemberList[m_nCurMemberCount - 1].m_nRoleRank = enmRoleRank_None;
				m_arrMemberList[m_nCurMemberCount - 1].m_nTotalTime = 0;
				m_arrMemberList[m_nCurMemberCount - 1].m_nMicTime = 0;

				UpdateDataInfo(GetMemberOffset(m_arrMemberList[i]), sizeof(m_arrMemberList[i]));

				--m_nCurMemberCount;
				UpdateDataInfo(GetMemberOffset(m_nCurMemberCount), sizeof(m_nCurMemberCount));

				switch(nRoleRank)
				{
					case enmRoleRank_TempAdmin:
					{
						--m_nCurTempAdmCount;
						UpdateDataInfo(GetMemberOffset(m_nCurTempAdmCount), sizeof(m_nCurTempAdmCount));
					}
					break;
					case enmRoleRank_Admin:
					{
						--m_nCurAdmCount;
						UpdateDataInfo(GetMemberOffset(m_nCurAdmCount), sizeof(m_nCurAdmCount));
					}
					break;
					case enmRoleRank_MinorHost:
					{
						--m_nCurSecHostCount;
						UpdateDataInfo(GetMemberOffset(m_nCurSecHostCount), sizeof(m_nCurSecHostCount));
					}
					break;
					default:
					break;
				}

				return S_OK;
			}
		}
		return E_RS_ISNOTINMEMBERLIST;
	}

    int32_t addAdminInRoomTime(const RoleID nRoleID,const uint32_t nAddinRoomTime)
    {
		for(int32_t i = 0;i<m_nCurMemberCount;i++)
		{
			if(m_arrMemberList[i].m_nRoleID==nRoleID)
			{
				m_arrMemberList[i].m_nTotalTime += nAddinRoomTime;
				UpdateDataInfo(GetMemberOffset(m_arrMemberList[i].m_nTotalTime), sizeof(m_arrMemberList[i].m_nTotalTime));
				return S_OK;
			}
		}
		return E_RS_ISNOTINMEMBERLIST;
    }
    int32_t addAdminOnMicTime(const RoleID nRoleID,const uint32_t nAddOnMicTime)
	{
		for(int32_t i = 0;i<m_nCurMemberCount;i++)
		{
			if(m_arrMemberList[i].m_nRoleID==nRoleID)
			{
				WRITE_NOTICE_LOG(" player %d in room %d add on mic time %d!\n",nRoleID,m_nRoomID,nAddOnMicTime);
				m_arrMemberList[i].m_nMicTime += nAddOnMicTime;
				UpdateDataInfo(GetMemberOffset(m_arrMemberList[i].m_nMicTime), sizeof(m_arrMemberList[i].m_nMicTime));
				return S_OK;
			}
		}
		return E_RS_ISNOTINMEMBERLIST;
	}
    bool IsCanAddTempAdm()
	{
		CRoomTypeInfo stRoomTypeInfo;
		stRoomTypeInfo = g_SomeConfig.GetRoomTypeInfo(m_nMaxShowUserLimit);
		return m_nCurTempAdmCount<stRoomTypeInfo.nMaxTempAdmCount;
	}

	bool IsCanAddAdm()
	{
		CRoomTypeInfo stRoomTypeInfo;
		stRoomTypeInfo = g_SomeConfig.GetRoomTypeInfo(m_nMaxShowUserLimit);
		return m_nCurAdmCount<stRoomTypeInfo.nMaxAdmCount;
	}
	bool IsCanAddSecHost()
	{
		CRoomTypeInfo stRoomTypeInfo;
		stRoomTypeInfo = g_SomeConfig.GetRoomTypeInfo(m_nMaxShowUserLimit);
		return m_nCurSecHostCount<stRoomTypeInfo.nMaxSecHostCount;
	}

	void GetMemberList(MemberInfo arrMemberList[], const uint16_t arrSize, uint16_t &nMemberCount)
	{
		nMemberCount = m_nCurMemberCount > arrSize ? arrSize : m_nCurMemberCount;
		memcpy(arrMemberList, m_arrMemberList, sizeof(MemberInfo) * nMemberCount);
	}
	void GetMemberList(int32_t &nMemberCount,RoleID arrRoleID[],RoleRank arrRoleRank[],const int32_t arrSize)
	{
		nMemberCount = m_nCurMemberCount > arrSize ? arrSize : m_nCurMemberCount;
		for(int32_t i =0;i<nMemberCount;i++)
		{
			arrRoleID[i] = m_arrMemberList[i].m_nRoleID;
			arrRoleRank[i] = m_arrMemberList[i].m_nRoleRank;
		}
	}


	void GetMemberInfo(const RoleID nRoleID, MemberInfo &stMemberInfo)
	{
		for(int32_t i = 0; i < m_nCurMemberCount; ++i)
		{
			if(m_arrMemberList[i].m_nRoleID == nRoleID)
			{
				memcpy(&stMemberInfo, &m_arrMemberList[i], sizeof(MemberInfo));
				break;
			}
		}
	}

	bool IsMemberInroom(const RoleID nRoleID)
	{
		for(int32_t i = 0; i < m_nCurMemberCount; ++i)
		{
			if(m_arrMemberList[i].m_nRoleID == nRoleID)
			{
				return true;
			}
		}
		return false;
	}

	RoleRank GetRoleRank(const RoleID nRoleID) const
	{
		for(int32_t i = 0; i < m_nCurMemberCount; ++i)
		{
			if(m_arrMemberList[i].m_nRoleID == nRoleID)
			{
				return m_arrMemberList[i].m_nRoleRank;
			}
		}

		return enmRoleRank_None;
	}

	int32_t SetPrivateMicOpened(const uint8_t nPrivateMicOpened)
	{
		if(nPrivateMicOpened==enmPrivateMicOpenedType_Open)
		{
			m_nRoomOption&=~enmRoomOptionType_PrivateMIC_Close;
		}
		else if(nPrivateMicOpened==enmPrivateMicOpenedType_Close)
		{
			m_nRoomOption|=enmRoomOptionType_PrivateMIC_Close;
		}
		else
		{
			return E_INVALIDARGUMENT;
		}
		UpdateDataInfo(GetMemberOffset(m_nRoomOption), sizeof(m_nRoomOption));
	}

	bool IsPrivateMicOpened()
	{
		return !(m_nRoomOption&enmRoomOptionType_PrivateMIC_Close)&&(m_nRoomOption&enmRoomOptionType_Has_PrivateMic);
	}

	int32_t SetFreeShowOpened(const uint8_t nFreeShowOpened)
	{
		if(nFreeShowOpened==enmFreeShowOpenedType_Open)
		{
			m_nRoomOption|=enmRoomOptionType_FreeShow_Open;
		}
		else if(nFreeShowOpened==enmFreeShowOpenedType_Close)
		{
			m_nRoomOption&=~enmRoomOptionType_FreeShow_Open;
		}
		else
		{
			return E_INVALIDARGUMENT;
		}

		UpdateDataInfo(GetMemberOffset(m_nRoomOption), sizeof(m_nRoomOption));

		return S_OK;
	}

	bool IsFreeShowOpened()
	{
		WRITE_DEBUG_LOG("option %d!!!!!!!!",m_nRoomOption);
		return (m_nRoomOption&enmRoomOptionType_FreeShow_Open);
	}
	bool HasPublicMicIndex()
	{
		for(int32_t i = 0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==enmInvalidRoleID&&m_arrOnPublicMic[i].isOpened)
			{
				return true;
			}
		}
			return false;
	}

	int32_t GetCanGotoMicCount()
	{
		int32_t nMicCount = 0;
		for(int32_t i = 0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==enmInvalidRoleID&&m_arrOnPublicMic[i].isOpened)
			{
				nMicCount++;
			}
		}
		return nMicCount;
	}
	void LockedMicByIndex(uint8_t nMicIndex)
	{
		m_arrOnPublicMic[nMicIndex-1].isOpened = false;
		UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic[nMicIndex-1]), sizeof(m_arrOnPublicMic[nMicIndex-1]));
	}

	void OpenedMicByIndex(uint8_t nMicIndex)
	{
		m_arrOnPublicMic[nMicIndex-1].isOpened = true;
		UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic[nMicIndex-1]), sizeof(m_arrOnPublicMic[nMicIndex-1]));
	}

	RoleID GetPlayerOnMicByIndex(uint8_t nMicIndex)
	{
		return m_arrOnPublicMic[nMicIndex-1].m_nRoleID;
	}

	MicStatus GetMicStatusByMicIndex(uint8_t nMicIndex)
	{
		if(m_arrOnPublicMic[nMicIndex-1].isOpened)
		{
			return enmMicStatus_Open;
		}
		return enmMicStatus_Close;
	}
	int32_t RemoveOnPublicMic(const RoleID nRoleID,uint8_t &nMicIndex)
	{
		for(int32_t i =0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==nRoleID)
			{
				bool isOpenedTemp = m_arrOnPublicMic[i].isOpened;
				m_arrOnPublicMic[i].Reset();
				m_arrOnPublicMic[i].isOpened = isOpenedTemp;
				nMicIndex = i+1;
				--m_nOnPublicMicCount;

				UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic[i]), sizeof(m_arrOnPublicMic[i]));
				UpdateDataInfo(GetMemberOffset(m_nOnPublicMicCount), sizeof(m_nOnPublicMicCount));
				return S_OK;
			}
		}
		return E_RS_NOTINPUBLIC;
	}

	int32_t AddOnPublicMic(const RoleID nRoleID,uint8_t &nMicIndex)
	{
		if(m_nOnPublicMicCount >= MaxPublicMicCount)
		{
			return E_RS_PUBLICISFULL;
		}
		for(int32_t i = 0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==nRoleID)
			{
				return E_RS_INPUBLICMIC;
			}
		}
		for(int32_t i =0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==enmInvalidRoleID&&m_arrOnPublicMic[i].isOpened)
			{
				m_arrOnPublicMic[i].m_nRoleID = nRoleID;
				nMicIndex = i+1;
				++m_nOnPublicMicCount;

				UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic[i].m_nRoleID), sizeof(m_arrOnPublicMic[i].m_nRoleID));
				UpdateDataInfo(GetMemberOffset(m_nOnPublicMicCount), sizeof(m_nOnPublicMicCount));
				return S_OK;
			}
		}
		return E_RS_PUBLICINDEXISEMPTY;
	}

	int32_t AddOnPublicMicInIndex(const RoleID nRoleID,uint8_t nMicIndex)
	{
		if(nMicIndex<1||nMicIndex>MaxPublicMicCount)
		{
			return E_RS_PUBLICINDEXISERROR;
		}
		if(m_nOnPublicMicCount >= MaxPublicMicCount)
		{
			return E_RS_PUBLICISFULL;
		}
		if(!m_arrOnPublicMic[nMicIndex-1].isOpened)
		{
			return E_RS_PUBLICISLOCKED;
		}
		if(m_arrOnPublicMic[nMicIndex-1].m_nRoleID!=enmInvalidRoleID)
		{
			return E_RS_PUBLICINDEXISEMPTY;
		}

		m_arrOnPublicMic[nMicIndex-1].m_nRoleID=nRoleID;
		++m_nOnPublicMicCount;

		UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic[nMicIndex-1].m_nRoleID), sizeof(m_arrOnPublicMic[nMicIndex-1].m_nRoleID));
		UpdateDataInfo(GetMemberOffset(m_nOnPublicMicCount), sizeof(m_nOnPublicMicCount));
		return S_OK;
	}
	void GetOnPublicMic(RoleID arrPublicMic[], const uint16_t arrSize, uint16_t &nPublicMicCount)
	{
		nPublicMicCount = m_nOnPublicMicCount > arrSize ? arrSize : m_nOnPublicMicCount;
		int32_t n = 0;
		for(int32_t i=0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID!=enmInvalidRoleID)
			{
				arrPublicMic[n] = m_arrOnPublicMic[i].m_nRoleID;
				n++;
			}
		}
	}

	uint8_t GetPublicMicIndex(const RoleID nRoleID)
	{
		for(int32_t i = 0;i<MaxPublicMicCount;i++)
		{
			if(m_arrOnPublicMic[i].m_nRoleID==nRoleID)
			{
				return i+1;
			}
		}
		return 0;
	}

	bool IsInWaittingMic(const RoleID nRoleID)
	{
		for(int32_t i = 0; i < m_nWaittingMicCount; ++i)
		{
			if(m_arrWaittingMic[i] == nRoleID)
			{
				return true;
			}
		}
		return false;
	}
	int32_t AddToWaittingMic(const RoleID nRoleID)
	{
		CRoomTypeInfo stRoomTypeInfo = g_SomeConfig.GetRoomTypeInfo(m_nMaxShowUserLimit);
		if(m_nWaittingMicCount >= MaxWaittingMicCount||m_nWaittingMicCount>=stRoomTypeInfo.nMaxWaitCount)
		{
			return E_RS_WAITTINGMICISFULL;
		}

		for(int32_t i = 0; i < m_nWaittingMicCount; ++i)
		{
			if(m_arrWaittingMic[i] == nRoleID)
			{
				return S_OK;
			}
		}

		m_arrWaittingMic[m_nWaittingMicCount] = nRoleID;
		UpdateDataInfo(GetMemberOffset(m_arrWaittingMic[m_nWaittingMicCount]), sizeof(m_arrWaittingMic[m_nWaittingMicCount]));

		++m_nWaittingMicCount;
		UpdateDataInfo(GetMemberOffset(m_nWaittingMicCount), sizeof(m_nWaittingMicCount));

		return S_OK;
	}

	int32_t RemoveFromWaittingMic(const RoleID nRoleID)
	{
		for(int32_t i = 0; i < m_nWaittingMicCount; ++i)
		{
			if(m_arrWaittingMic[i] == nRoleID)
			{
				for(int32_t j = i; j < m_nWaittingMicCount-1; ++j)
				{
					m_arrWaittingMic[j] = m_arrWaittingMic[j+1];
					UpdateDataInfo(GetMemberOffset(m_arrWaittingMic[j]), sizeof(m_arrWaittingMic[j]));
				}
				--m_nWaittingMicCount;
				m_arrWaittingMic[m_nWaittingMicCount] = enmInvalidRoleID;

				UpdateDataInfo(GetMemberOffset(m_arrWaittingMic[m_nWaittingMicCount]), sizeof(m_arrWaittingMic[m_nWaittingMicCount]));
				UpdateDataInfo(GetMemberOffset(m_nWaittingMicCount), sizeof(m_nWaittingMicCount));
				return S_OK;
			}
		}
		return E_RS_NOTINWAITTINGMIC;
	}
    RoleID GetFirstWaittingRoleID()
    {
    	if(m_nWaittingMicCount==0)
    	{
    		return enmInvalidRoleID;
    	}
    	RoleID retRoleID = m_arrWaittingMic[0];
    	for(int32_t j = 0; j < m_nWaittingMicCount-1; ++j)
		{
			m_arrWaittingMic[j] = m_arrWaittingMic[j+1];
			UpdateDataInfo(GetMemberOffset(m_arrWaittingMic[j]), sizeof(m_arrWaittingMic[j]));
		}
    	if(m_nWaittingMicCount>0)
    	{
    		--m_nWaittingMicCount;
			m_arrWaittingMic[m_nWaittingMicCount] = enmInvalidRoleID;
			UpdateDataInfo(GetMemberOffset(m_arrWaittingMic[m_nWaittingMicCount]), sizeof(m_arrWaittingMic[m_nWaittingMicCount]));
			UpdateDataInfo(GetMemberOffset(m_nWaittingMicCount), sizeof(m_nWaittingMicCount));
    	}
    	return retRoleID;
    }
	void GetWaittingMic(RoleID arrWaittingMic[], const uint16_t arrSize, uint16_t &nWaittingMicCount)
	{
		nWaittingMicCount = m_nWaittingMicCount > arrSize ? arrSize : m_nWaittingMicCount;
		for(uint16_t i = 0;i<nWaittingMicCount;i++)
		{
			arrWaittingMic[i] = m_arrWaittingMic[i];
		}
	}

	int32_t AddOnPrivateMic(const RoleID nRoleID)
	{
		if(m_nPrivateMicCount >= MaxPrivateMicCount)
		{
			return E_RS_PRIVATEMICISFULL;
		}
		for(int32_t i = 0;i<m_nPrivateMicCount;i++)
		{
			if(m_arrPrivateMic[i]==nRoleID)
			{
				return E_RS_INPRIVATEMIC;
			}
		}
		m_arrPrivateMic[m_nPrivateMicCount] = nRoleID;
		UpdateDataInfo(GetMemberOffset(m_arrPrivateMic[m_nPrivateMicCount]), sizeof(m_arrPrivateMic[m_nPrivateMicCount]));

		++m_nPrivateMicCount;
		UpdateDataInfo(GetMemberOffset(m_nPrivateMicCount), sizeof(m_nPrivateMicCount));
		return S_OK;
	}

	void GetOnPrivateMic(RoleID arrPrivateMic[], const uint16_t arrSize, uint16_t &nPrivateMicCount)
	{
		nPrivateMicCount = m_nPrivateMicCount > arrSize ? arrSize : m_nPrivateMicCount;
		for(int32_t i=0;i<nPrivateMicCount;i++)
		{
			arrPrivateMic[i] = m_arrPrivateMic[i];
		}
	}
	int32_t RemoveFromPrivateMic(const RoleID nRoleID)
	{
		for(int32_t i = 0; i < m_nPrivateMicCount; ++i)
		{
			if(m_arrPrivateMic[i] == nRoleID)
			{
				m_arrPrivateMic[i] = m_arrPrivateMic[m_nPrivateMicCount - 1];
				m_arrPrivateMic[m_nPrivateMicCount - 1] = enmInvalidRoleID;
				UpdateDataInfo(GetMemberOffset(m_arrPrivateMic[i]), sizeof(m_arrPrivateMic[i]));

				--m_nPrivateMicCount;
				UpdateDataInfo(GetMemberOffset(m_nPrivateMicCount), sizeof(m_nPrivateMicCount));

				return S_OK;
			}
		}
		return E_RS_NOTINPRIVATEMIC;
	}
	const char *GetRoomNotice()
	{
		return m_strRoomNotice.GetString();
	}

	void SetRoomNotice(const char *strRoomNotice)
	{
		m_strRoomNotice = strRoomNotice;

		UpdateDataInfo(GetMemberOffset(m_strRoomNotice), sizeof(m_strRoomNotice));
	}
	const char *GetRoomSignature()
	{
		return m_strRoomSignature.GetString();
	}

	void SetRoomSignature(const char *strRoomSignature)
	{
		m_strRoomSignature = strRoomSignature;

		UpdateDataInfo(GetMemberOffset(m_strRoomSignature), sizeof(m_strRoomSignature));
	}
	const char *GetRoomWelcome()
	{
		return m_strRoomWelcome.GetString();
	}

	void SetRoomWelcome(const char *strRoomWelcome)
	{
		m_strRoomWelcome = strRoomWelcome;

		UpdateDataInfo(GetMemberOffset(m_strRoomWelcome), sizeof(m_strRoomWelcome));
	}

	const char *GetRoomPassWord()
	{
		return m_strPassword.GetString();
	}

	void SetRoomPassWord(const char *strRoomPassWord)
	{
		m_strPassword = strRoomPassWord;

		UpdateDataInfo(GetMemberOffset(m_strPassword), sizeof(m_strPassword));
	}
	RegionType GetRegionType()
	{
		return m_nRegionType;
	}

	void SetRegionType(const RegionType nRegionType)
	{
		m_nRegionType = nRegionType;

		UpdateDataInfo(GetMemberOffset(m_nRegionType), sizeof(m_nRegionType));
	}
	ChannelType GetChannelType()
	{
		return m_nChannelType;
	}

	void SetChannelType(const ChannelType nChannelType)
	{
		m_nChannelType = nChannelType;

		UpdateDataInfo(GetMemberOffset(m_nChannelType), sizeof(m_nChannelType));
	}
	RoomType GetRoomType()
	{
		return m_nRoomType;
	}

	void SetRoomType(const RoomType nRoomType)
	{
		m_nRoomType = nRoomType;

		UpdateDataInfo(GetMemberOffset(m_nRoomType), sizeof(m_nRoomType));
	}
	const char *GetRegionName()
	{
		return m_strRegionName.GetString();
	}

	void SetRegionName(const char *strRegionName)
	{
		m_strRegionName = strRegionName;

		UpdateDataInfo(GetMemberOffset(m_strRegionName), sizeof(m_strRegionName));
	}
	const char *GetChannelName()
	{
		return m_strChannelName.GetString();
	}

	void SetChannelName(const char *strChannelName)
	{
		m_strChannelName = strChannelName;

		UpdateDataInfo(GetMemberOffset(m_strChannelName), sizeof(m_strChannelName));
	}
	const char *GetHostName()
	{
		return m_strHostName.GetString();
	}

	void SetHostName(const char *strHostName)
	{
		m_strHostName = strHostName;

		UpdateDataInfo(GetMemberOffset(m_strHostName), sizeof(m_strHostName));
	}
	bool PlayerCanEnter(bool isNeedRealUserList)
	{
		if(isNeedRealUserList)
		{
			return (int32_t)m_nMaxRealUserLimit>(int32_t)(m_sRoomPlayerIndexList.ObjectCount()-m_nHideEnterOfficer);
		}
		return (int32_t)m_nMaxShowUserLimit>(int32_t)(m_sRoomPlayerIndexList.ObjectCount()-m_nHideEnterOfficer);
	}
	uint32_t GetHideEnterOfficerCount()
	{
		return m_nHideEnterOfficer;
	}
    bool IsKickRobotForRoomFull()
    {
    	return (m_sRoomPlayerIndexList.ObjectCount()+m_sRoomRebotIndexList.ObjectCount()-m_nHideEnterOfficer)>m_nMaxRealUserLimit;
    }
    uint32_t GetRobotCount()
    {
    	return m_sRoomRebotIndexList.ObjectCount();
    }
    //为正为加，为负为减
    int32_t GetNeedRobotCount()
    {
    	if((int32_t)(m_sRoomPlayerIndexList.ObjectCount() - m_nHideEnterOfficer)<(int32_t)g_RebotConfig.GetBeginDistribution())
    	{
    		return 0;
    	}
    	int32_t nCanEnterCount = (int32_t)(m_nMaxRealUserLimit-(m_sRoomPlayerIndexList.ObjectCount()+m_sRoomRebotIndexList.ObjectCount()-m_nHideEnterOfficer));
    	if(IsKickRobotForRoomFull())
    	{
           return nCanEnterCount;
    	}
    	int32_t nNeedCount = (int32_t)((m_sRoomPlayerIndexList.ObjectCount()-g_RebotConfig.GetBeginDistribution()-m_nHideEnterOfficer)*m_nRobotPercent/100)-m_sRoomRebotIndexList.ObjectCount();
    	if(nCanEnterCount<nNeedCount)
    	{
    		return nCanEnterCount;
    	}
    	return nNeedCount;
    }
    RoleID GetKickRobotRoleID()
    {
    	RoomPlayerIndexList::CIndex *pIndex = m_sRoomRebotIndexList.First();
    	if(NULL != pIndex)
    	{
    		return pIndex->Object();
    	}
    	return enmInvalidRoleID;
    }
	Gender GetHostGender()
	{
		return m_nHostGender;
	}

	void SetHostGender(const Gender nGender)
	{
		m_nHostGender = nGender;

		UpdateDataInfo(GetMemberOffset(m_nHostGender), sizeof(m_nHostGender));
	}
	uint32_t GetHost179ID()
	{
		return m_nHost179ID;
	}

	void SetHost179ID(const uint32_t nHost179ID)
	{
		m_nHost179ID = nHost179ID;

		UpdateDataInfo(GetMemberOffset(m_nHost179ID), sizeof(m_nHost179ID));
	}
	bool IsOpenAdminOnMicTime()
	{
		return !(m_nRoomOption&enmRoomOptionType_FreeShow_ForManager_Close);
	}

	//机器人相关的代码
	int32_t AddRebotPlayer(const RoleID nRoleID);
	int32_t DeleteRebotPlayer(const RoleID nRoleID);
	bool IsRebotPlayerInRoom(const RoleID nRoleID);
	int32_t GetAllRebotPlayers(RoleID arrRoleID[], const int32_t arrSize, int32_t &nPlayerCount);

	bool IsCanPublicChat(VipLevel nVipLevel,TextResult& nTextResult)
	{
		VipLevel nTempVipLevel = enmVipLevel_NONE;
		if(m_nRoomOption & enmRoomOptionType_Public_Chat_Vip)
		{
			nTempVipLevel = enmVipLevel_REGISTER;
			nTextResult = enmTextResult_VipCanPublicChat;
		}
		else
		{
			nTempVipLevel = enmVipLevel_NONE;
			nTextResult = enmTextResult_RegCanPublicChat;
		}

		if(nVipLevel > nTempVipLevel)
		{
			return true;
		}
		return false;
	}
	bool IsSendColorOpen()
	{
		return !(m_nRoomOption&enmRoomOptionType_Color_Text_Close);
	}
	bool IsRoomCloseed()
	{
		return m_nRoomOption&enmRoomOptionType_Room_Close;
	}
	bool IsCloseAdminEditMicTime()
	{
		return m_nRoomOption&enmRoomOptionType_FreeShow_Edit_Time_Close;
	}

	uint32_t GetStartPos()
	{
		return (uint32_t)offsetof(CRoom, m_nStartPos);
	}

	uint32_t GetEndPos()
	{
		return (uint32_t)offsetof(CRoom, m_nEndPos);
	}

	bool IsHaveUpdateData()
	{
		return m_nUpdateDataInfoCount > 0 ? true : false;
	}

	uint8_t* GetDataByOffset(uint32_t nDataOffset);

	int32_t GetUpdateDataInfoTable(uint8_t* arrUpdateData[], UpdateDataInfoTable arrUpdateTable[]);

	void SetRobotPercent(uint32_t nRobotPercent)
	{
		m_nRobotPercent = nRobotPercent;
		UpdateDataInfo(GetMemberOffset(m_nRobotPercent), sizeof(m_nRobotPercent));
	}

	uint32_t GetRobotPercent()
	{
		return m_nRobotPercent;
	}

	void SetRobotSendPercent(uint32_t nRobotSendPercent)
	{
		m_nSendPercent = nRobotSendPercent;
		UpdateDataInfo(GetMemberOffset(m_nSendPercent), sizeof(m_nSendPercent));
	}

	uint32_t GetRobotSendPercent()
	{
		return m_nSendPercent;
	}

	void ReSetAllMic()
	{
		for(int32_t i = 0;i<MaxPublicMicCount;i++ )
		{
			m_arrOnPublicMic[i].Reset();
		}
		UpdateDataInfo(GetMemberOffset(m_arrOnPublicMic),sizeof(m_arrOnPublicMic));
	}

//	void UpdateUserCountOnMedia(ServerID nServerID, int32_t nUpdateCount)
//	{
//		if(nServerID == enmInvalidServerID)
//		{
//			return;
//		}
//
//		for(int32_t i = 0; i < m_nMediaServerCount; ++i)
//		{
//			if(m_arrMediaServer[i] == nServerID)
//			{
//				m_arrOnMediaServerUserCount[i] += nUpdateCount;
//				if(m_arrOnMediaServerUserCount[i] <= 0)
//				{
//					m_arrMediaServer[i] = m_arrMediaServer[m_nMediaServerCount - 1];
//					--m_nMediaServerCount;
//				}
//				return;
//			}
//		}
//
//		if((m_nMediaServerCount < MaxMediaCount) && (nUpdateCount > 0))
//		{
//			m_arrMediaServer[m_nMediaServerCount] = nServerID;
//			m_arrOnMediaServerUserCount[m_nMediaServerCount] += nUpdateCount;
//			++m_nMediaServerCount;
//		}
//	}

	void SendAddTransmitUserNotifyWhenPlayerEnterRoom(CPlayer* player);

	bool InsertMediaMap(RoleID nSpeakerID, ServerID nServerID, bool bIsPublicMic);
	void EraseMediaMap(RoleID nSpeakerID, ServerSet& stServerSet);
	void EraseMediaMap(RoleID nSpeakerID, ServerID nServerID);

	void SetRebotNextColorTime(uint32_t nTime)
	{
		m_nNextRebotSendColorTime = nTime;
	}

	uint32_t GetRebotNextColorTime()
	{
		return m_nNextRebotSendColorTime;
	}
private:
	void UpdateDataInfo(uint32_t nOffset, uint32_t nDataSize);

	bool InsertMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerID nServerID);
	void EraseMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerID nServerID);
	void EraseMediaMap(MediaserverMap& stMap, RoleID nSpeakerID, ServerSet& stServerSet);

private:

	RoomPlayerIndexList::Key MakeKey(const PlayerIndex nPlayerIndex) const;
	RoomPlayerIndexList::Key MakeRebotPlayerKey(const RoleID nRoleID) const;

protected:
	void Reset()
	{
		m_nRegionID = enmInvalidRegionID;
		m_nRegionType = enmInvalidRegionType;
		m_nChannelID = enmInvalidChannelID;
		m_nChannelType = enmInvalidChannelType;
		m_nRoomID = enmInvalidRoomID;
		m_nRoomType = enmInvalidRoomType;
		m_strRoomName.Initialize();
		m_strRegionName.Initialize();
		m_strChannelName.Initialize();
		m_strRoomNotice.Initialize();
		m_strRoomSignature.Initialize();
		m_strRoomWelcome.Initialize();

		m_nCurVisitorCount = 0;
		m_nMaxRealUserLimit = MaxDefaultRealUserLimit;
		m_nMaxShowUserLimit = MaxDefaultShowUserLimit;
		m_nHostID = enmInvalidRoleID;
		m_strHostName.Initialize();
		m_strPassword.Initialize();

		m_nRoomFund = 0;
		m_nRoomOption = 0;
		m_nMicroTime = 0;
		m_strRoomSignature.Initialize();

		memset(m_arrMediaAddress, 0, sizeof(m_arrMediaAddress));
		memset(m_arrMediaPort, 0, sizeof(m_arrMediaPort));
		memset(m_arrMediaServerID, 0, sizeof(m_arrMediaServerID));

		m_nCurKickCount = 0;
		m_nCurBlackCount = 0;
		memset(m_arrBlackList, enmInvalidRoleID, sizeof(m_arrBlackList));

		m_nCurProhibitCount = 0;
		memset(m_arrProhibitSpeakList, 0, sizeof(m_arrProhibitSpeakList));

		m_nCurLockIPCount = 0;
		m_nCurSecHostCount = 0;
		m_nCurAdmCount = 0;
		m_nCurTempAdmCount = 0;
		m_nCurMemberCount = 0;
		m_sRoomPlayerIndexList.Initialize();
		m_sRoomRebotIndexList.Initialize();

		m_nOnPublicMicCount = 0;
		m_nWaittingMicCount = 0;
		memset(m_arrWaittingMic, enmInvalidRoleID, sizeof(m_arrWaittingMic));

		m_nPrivateMicCount = 0;
		memset(m_arrPrivateMic, enmInvalidRoleID, sizeof(m_arrPrivateMic));

		m_nHostGender = enmGender_Unknown;
		m_nEndPos = 0;

		m_nRobotPercent = 0;		//机器人个数/真人个数×100
		m_nSendPercent = 0;			//发送彩条的概率

		m_nHideEnterOfficer = 0;
		m_nUpdateDataInfoCount = 0;
		m_nNextRebotSendColorTime = 0;
//		m_nMediaServerCount = 0;
//		memset(m_arrOnMediaServerUserCount, 0, sizeof(m_arrOnMediaServerUserCount));
	}

public:
	//注意，不要在m_nStartPos前添加成员变量
	uint8_t											m_nStartPos;

protected:
	RoomID 											m_nRoomID;
	RoomType										m_nRoomType; //房间的类型
	RegionID                    					m_nRegionID;
	RegionType 										m_nRegionType;
	ChannelID 										m_nChannelID;
	ChannelType 									m_nChannelType;
	CString<MaxRoomPasswordLength> 					m_strPassword; //房间密码
	RoomOptionType 									m_nRoomOption; //房间的设置选项
	uint32_t 										m_nMicroTime; //上麦时间
	CString<enmMaxRoomNameLength> 					m_strRoomName; //房间的名称
	CString<MaxRegionNameLength>					m_strRegionName;
	CString<MaxChannelNameLength>					m_strChannelName;
	CString<MaxRoomNoticeLength> 					m_strRoomNotice; //房间的公告
	CString<MaxRoomSignatureLength> 				m_strRoomSignature; //房间的主题
	CString<MaxRoomWelcomeLength> 					m_strRoomWelcome; //房间的欢迎词
	uint32_t										m_nRoomFund; //房间公积金
	uint16_t 										m_nCurVisitorCount; //当前房间游客数量
	uint16_t 										m_nMaxRealUserLimit; //例如450（大的值）
	uint16_t 										m_nMaxShowUserLimit; //例如300（小的个值）
	RoleID 											m_nHostID; //室主的roleid
	CString<enmMaxRoleNameLength> 					m_strHostName; //室主的昵称
	Gender 											m_nHostGender; //室主性别
	uint32_t 										m_nHost179ID;   //室主179ID
	uint32_t										m_arrMediaAddress[enmNetType_Count];
	uint16_t										m_arrMediaPort[enmNetType_Count];
	ServerID										m_arrMediaServerID[enmNetType_Count];          //media的serverID
	int32_t 										m_nCurKickCount;
	KickList 										m_arrKickList[MaxRoomKickListSize]; //被踢名单
	int32_t 										m_nCurBlackCount;
	RoleID 											m_arrBlackList[MaxRoomBlackListSize];//黑名单
	int32_t 										m_nCurProhibitCount;
	ProhibitSpeakList 								m_arrProhibitSpeakList[MaxRoomProhibitSpeakSize];//禁言名单
	int32_t 										m_nCurLockIPCount;
	LockIpInfo 										m_arrLockIP[MaxRoomLockIP];//被封IP
	int32_t 										m_nCurMemberCount;
	int32_t											m_nCurSecHostCount;
	int32_t											m_nCurAdmCount;
	int32_t											m_nCurTempAdmCount;
	MemberInfo 										m_arrMemberList[MaxAdminCountPerRoom];
	int32_t 										m_nOnPublicMicCount;
	OnPublicMicInfo 								m_arrOnPublicMic[MaxPublicMicCount]; //正在公卖上的role
	int32_t 										m_nWaittingMicCount;
	RoleID 											m_arrWaittingMic[MaxWaittingMicCount]; //正在排麦的role
	int32_t 										m_nPrivateMicCount;
	RoleID 											m_arrPrivateMic[MaxPrivateMicCount]; //正在私麦上的玩家
	uint32_t										m_nRobotPercent;		//机器人个数/真人个数×100
	uint32_t										m_nSendPercent;			//发送彩条的概率
	uint32_t										m_nNextRebotSendColorTime;	//下次机器人发彩条的时间
	uint32_t 										m_nHideEnterOfficer;	//隐身进入的官方人员
//	int32_t											m_nMediaServerCount;
//	ServerID										m_arrMediaServer[MaxMediaCount];
//	int32_t											m_arrOnMediaServerUserCount[MaxMediaCount];


public:
	//注意，这个值只是标记作用，所有基本数据类型都要在此成员变量之前定义
	uint8_t											m_nEndPos;

protected:
	RoomPlayerIndexList 							m_sRoomPlayerIndexList; //玩家对象池
	RoomPlayerIndexList 							m_sRoomRebotIndexList; //机器人对象池

private:
	int32_t											m_nUpdateDataInfoCount;
	UpdateDataInfoTable								m_arrUpdateDataInfoTable[UpdateDataInfoTableSize];

	MediaserverMap									m_mapOnPublicMicMediaserver; //发过上公麦通知的Mediaserver
	MediaserverMap									m_mapOnPrivateMicMediaserver; //发过上私麦通知的Mediaserver
public:
	CRoomArtist                                     m_RoomArtistList;
};

FRAME_ROOMSERVER_NAMESPACE_END

CCOMPLEXTYPE_SPECIFICALLY(FRAME_ROOMSERVER_NAMESPACE::CRoom)

#endif /* DAL_ROOM_H_ */
