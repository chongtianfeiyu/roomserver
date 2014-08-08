/*
 * someconfig.h
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#ifndef SOMECONFIG_H_
#define SOMECONFIG_H_

#include <memory.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tinyxml/tinyxml.h"
#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_typedef.h"
#include "../../lightframe/lightframe_impl.h"
#include "main_frame.h"
#include "public_typedef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define DEFAULT_ROOM_CONFIG		"./config/roomconfig.xml"
#define MAX_VIP_COUNT			10
#define MAX_ROOM_TYPE_COUNT			8
#define MAX_TICKET_CONFIG_COUNT		100
#define MAX_PROGRAM_SECTION_COUNT	16

class CVipInfo
{
public:
	VipLevel 		nVipLevel;//vip等级
	uint16_t 		nMaxWatchCount; //可以观看私麦的最大数量
	uint16_t		nMaxEnterRoomCount;//可以进入的最大数量
	uint16_t		nMaxInviteVipCount;//可以邀约用户为紫色Vip的数量 /每月
	CVipInfo()
	{
		nVipLevel = 0;
		nMaxWatchCount = 0;
		nMaxEnterRoomCount = 0;
		nMaxInviteVipCount = 0;
	}
};

class CRoomTypeInfo
{
public:
	uint16_t 		nMaxPlayercount;  //房间最大进入人数
	uint16_t 		nMaxHoatCount; //室主的最大数量
	uint16_t		nMaxSecHostCount;//副室主的最大数量
	uint16_t		nMaxAdmCount;//管理员的最大数量
	uint16_t		nMaxTempAdmCount;//临时管理员的最大数量
	uint16_t		nMaxWaitCount;   //麦序最大个数
	CRoomTypeInfo()
	{
		nMaxPlayercount = 0;
		nMaxHoatCount = 0;
		nMaxSecHostCount = 0;
		nMaxAdmCount = 0;
		nMaxTempAdmCount = 0;
		nMaxWaitCount = 0;
	}
};

class CTicketConfig
{
public:
	uint16_t		nConfigCount;
	uint16_t		arrPlayerCount[MAX_TICKET_CONFIG_COUNT];
	uint16_t		arrTicketCount[MAX_TICKET_CONFIG_COUNT];

	uint16_t		nRequesterTicketVaule;
	uint16_t		nRequesterChance;

	CTicketConfig()
	{
		nConfigCount = 0;
		memset(arrPlayerCount, 0, sizeof(arrPlayerCount));
		memset(arrTicketCount, 0, sizeof(arrTicketCount));

		nRequesterTicketVaule = 0;
		nRequesterChance = 0;
	}
};

class CVoteConfig
{
public:
	int32_t			nConfigCount;
	ProgramStatus	arrProgramStatus[MAX_PROGRAM_SECTION_COUNT];
	int32_t			arrProgramTime[MAX_PROGRAM_SECTION_COUNT];
	int32_t			nShowResultTime;

	CVoteConfig()
	{
		nConfigCount = 0;
		memset(arrProgramStatus, 0, sizeof(arrProgramStatus));
		memset(arrProgramTime, 0, sizeof(arrProgramTime));
		nShowResultTime = 0;
	}
};

class CConfig :public IConfigCenter
{
public:
	CConfig()
	{
		m_nVipCount=0;
		memset(m_arrVipInfo,0,sizeof(CVipInfo)*MAX_VIP_COUNT);
		m_nRoomTypeCount=0;
		memset(m_arrRoomTypeInfo,0,sizeof(CRoomTypeInfo)*MAX_ROOM_TYPE_COUNT);
		m_nPlayerCountStaTimer = 0;
		m_nOnlineTimeStaTimer = 0;
		m_nRunTime = 0;
		m_nUpdatePlayerBegin = 0;
		m_nUpdateRoomBegin = 0;
		m_nRequestSongCost = 0;
	}
	virtual ~CConfig()
	{

	}

public:
	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0) ;
	int32_t Resume()
	{
		m_nVipCount = 0;
		memset(m_arrVipInfo,0,sizeof(CVipInfo)*MAX_VIP_COUNT);
		m_nRoomTypeCount = 0;
		memset(m_arrRoomTypeInfo,0,sizeof(CRoomTypeInfo)*MAX_ROOM_TYPE_COUNT);
		m_nPlayerCountStaTimer = 0;
		m_nOnlineTimeStaTimer = 0;
		m_nRunTime = 0;
		m_nUpdatePlayerBegin = 0;
		m_nUpdateRoomBegin = 0;
		return S_OK;
	}
	int32_t Uninitialize()
	{
		m_nVipCount = 0;
		memset(m_arrVipInfo,0,sizeof(CVipInfo)*MAX_VIP_COUNT);
		m_nRoomTypeCount = 0;
		memset(m_arrRoomTypeInfo,0,sizeof(CRoomTypeInfo)*MAX_ROOM_TYPE_COUNT);
		m_nPlayerCountStaTimer = 0;
		m_nOnlineTimeStaTimer = 0;
		m_nRunTime = 0;
		m_nUpdatePlayerBegin = 0;
		m_nUpdateRoomBegin = 0;
		return S_OK;
	}

	int32_t LoadVipInfo(TiXmlElement* pRoot);

	CVipInfo GetVipInfo(VipLevel nVipLevel)
	{
		CVipInfo retVipInfo;
        for(int32_t i = 0; i<m_nVipCount;i++)
        {
        	if(nVipLevel==m_arrVipInfo[i].nVipLevel)
        	{
               return m_arrVipInfo[i];
        	}
        }
        return retVipInfo;
	}
	int32_t LoadRoomTypeInfo(TiXmlElement* pRoot);

	CRoomTypeInfo GetRoomTypeInfo(uint16_t nMaxPlayerCount)
	{
		CRoomTypeInfo retCRoomTypeInfo;
		for(int32_t i = 0; i<m_nRoomTypeCount;i++)
		{
			if(nMaxPlayerCount==m_arrRoomTypeInfo[i].nMaxPlayercount)
			{
				return m_arrRoomTypeInfo[i];
			}
		}
		return retCRoomTypeInfo;
	}

	int32_t LoadTimerInfo(TiXmlElement* pRoot);

	uint32_t GetPlayerCountTimer()
	{
		return m_nPlayerCountStaTimer;
	}

	uint32_t GetOnlineTimeStaTimer()
	{
		return m_nOnlineTimeStaTimer;
	}

	uint32_t GetStaRunTime()
	{
		return m_nRunTime;
	}

	int32_t LoadBeginValue(TiXmlElement* pRoot);

	int32_t LoadShutUpTime(TiXmlElement* pRoot);

	int32_t LoadRequestSongCost(TiXmlElement* pRoot);

	int32_t LoadTicketConfig(TiXmlElement* pRoot);

	int32_t LoadVoteConfig(TiXmlElement* pRoot);

	int32_t LoadTextConfig(TiXmlElement* pRoot);

	int32_t GetPlayerBegin()
	{
		return m_nUpdatePlayerBegin;
	}

	int32_t GetRoomBegin()
	{
		return m_nUpdateRoomBegin;
	}

	int32_t GetShutUpTime()
	{
		return m_nShutUpTime;
	}

	int32_t GetRequestSongCost()
	{
		return m_nRequestSongCost;
	}

	//根据房间实际用户来计算出有效票数
	uint16_t GetValidTicketsCount(int32_t nTotalPlayerCount)
	{
		//如果人数大于配置中的最大人数，则用配置中的最大人数所对应的票数
		if(nTotalPlayerCount > m_stTicketConfig.arrPlayerCount[m_stTicketConfig.nConfigCount - 1])
		{
			return m_stTicketConfig.arrTicketCount[m_stTicketConfig.nConfigCount - 1];
		}

		for(uint16_t i = 0; i < m_stTicketConfig.nConfigCount; ++i)
		{
			if(m_stTicketConfig.arrPlayerCount[i] >= nTotalPlayerCount)
			{
				return m_stTicketConfig.arrTicketCount[i];
			}
		}

		return 0;
	}

	int32_t GetProgramSectionTime(ProgramStatus nProgramStatus)
	{
		for(int32_t i = 0;i < m_stVoteConfig.nConfigCount; ++i)
		{
			if(nProgramStatus == m_stVoteConfig.arrProgramStatus[i])
			{
				return m_stVoteConfig.arrProgramTime[i];
			}
		}

		return 0;
	}

	int32_t GetShowResultTime()
	{
		return m_stVoteConfig.nShowResultTime;
	}

	uint16_t GetRequesterTicketValue()
	{
		return m_stTicketConfig.nRequesterTicketVaule;
	}

	uint16_t GetRequesterChance()
	{
		return m_stTicketConfig.nRequesterChance;
	}

	VipLevel GetCanPrivateTextVipLevel()
	{
		return m_nCanPrivateTextVipLevel;
	}

	RoleRank GetCanPrivateTextRoleRank()
	{
		return m_nCanPrivateTextRoleRank;
	}

	IdentityType GetCanPrivateTextIdentityType()
	{
		return m_nCanPrivateTextIdentityType;
	}

	bool IsClosePoorMan()
	{
		return m_nClosePoorMan > 0;
	}

	VipLevel GetCanPublicTextVipLevel()
	{
		return m_nCanPublicTextVipLevel;
	}
private:
	uint16_t		m_nVipCount;//现在的VIP数量
	CVipInfo 		m_arrVipInfo[MAX_VIP_COUNT];  //保存从配置中读出的有关VIP的信息
	uint16_t		m_nRoomTypeCount;//现在房间的类型数量
	CRoomTypeInfo   m_arrRoomTypeInfo[MAX_ROOM_TYPE_COUNT]; ////保存从配置中读出的有关房间类型的信息
	uint32_t 		m_nPlayerCountStaTimer;                      //统计在房间人数的时间
	uint32_t 		m_nOnlineTimeStaTimer;                      //统计管理员在房间
	uint32_t		m_nRunTime;							   //执行的时间

	int32_t		m_nUpdatePlayerBegin;    	//玩家开始更新数据到roomdispatcher的数据数量（个数）
	int32_t		m_nUpdateRoomBegin;    		//房间开始更新数据到roomdispatcher的数据数量（个数）

	int32_t		m_nShutUpTime;				//禁言时间

	int32_t		m_nRequestSongCost;			//点歌费用

	CTicketConfig	m_stTicketConfig;

	CVoteConfig		m_stVoteConfig;

	VipLevel	m_nCanPrivateTextVipLevel;	//可以私聊的vip等级
	RoleRank	m_nCanPrivateTextRoleRank;	//可以私聊的管理等级
	IdentityType	m_nCanPrivateTextIdentityType;	//可以私聊的身份
	int32_t		m_nClosePoorMan;			//因为前代码里面IdentityType中个字段是uint32的，所以想要关闭真正的纯屌丝用户是没有办法做到的，只能再加一个选项字段
	VipLevel	m_nCanPublicTextVipLevel;	//可以公聊的vip等级
};


typedef CFrameConfigContainer<CConfig>	CConfigContainer;

#define INITIALIZE_CONFIG		CSingleton<CConfigContainer>::GetInstance().Initialize
#define GET_CONFIG				CSingleton<CConfigContainer>::GetInstance().GetConfigObject

#define g_SomeConfig GET_CONFIG()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* SOMECONFIG_H_ */
