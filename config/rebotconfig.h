/*
 *  rebotconfig.h
 *
 *  To do：
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#ifndef REBOTCONFIG_H_
#define REBOTCONFIG_H_

#include <memory.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tinyxml/tinyxml.h"
#include "common/common_singleton.h"
#include "common/common_string.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_typedef.h"
#include "../../lightframe/lightframe_impl.h"
#include "main_frame.h"
#include "public_typedef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define DEFAULT_REBOT_CONFIG		"./config/robotconfig.xml"
#define MAX_179IDS_COUNT			10
#define MAX_NAME_COUNT			10000

class CRoleIDInfo
{
public:
	uint32_t  nRoleIDBegin;
	uint32_t  nRoleIDEnd;
	CRoleIDInfo()
	{
		nRoleIDBegin = 0;
		nRoleIDEnd = 0;
	}
};

class C179IDInfo
{
public:
	uint32_t  n179IDBegin;
	uint32_t  n179IDEnd;
	C179IDInfo()
	{
		n179IDBegin = 0;
		n179IDEnd = 0;
	}
};

class CColorInfo
{
public:
	uint32_t  nColorBegin;
	uint32_t  nColorEnd;
	CColorInfo()
	{
		nColorBegin = 0;
		nColorEnd = 0;
	}
};

class CRebotConfig :public IConfigCenter
{
public:
	CRebotConfig()
	{
		m_nrRoleIDInfo.nRoleIDBegin = 0;
		m_nrRoleIDInfo.nRoleIDEnd = 0;
		m_nRoleNameCount = 0;
		memset(m_arrRoleName,0,MaxRoleNameLength*MAX_NAME_COUNT);
		m_n179IDsCount = 0;
		memset(m_arr179IDInfo,0,sizeof(C179IDInfo)*MAX_179IDS_COUNT);
		m_nPerSendTime = 0;
		m_nPercent = 0;
		m_nColorInfo.nColorBegin = 0;
		m_nColorInfo.nColorEnd = 0;
		m_nNewPlayerLoginTimes = 0;
		m_nPerSendTimeMax = 0;
		m_nPerSendTimeMin = 0;
	}
	virtual ~CRebotConfig()
	{

	}

public:
	virtual int32_t Initialize(const char* szFileName = NULL, const int32_t type=0);
	virtual int32_t Reload(const char* szFileName = NULL, const int32_t type=0) ;
	int32_t Resume()
	{
		m_nrRoleIDInfo.nRoleIDBegin = 0;
		m_nrRoleIDInfo.nRoleIDEnd = 0;
		m_nRoleNameCount = 0;
		memset(m_arrRoleName,0,MaxRoleNameLength*MAX_NAME_COUNT);
		m_n179IDsCount = 0;
		memset(m_arr179IDInfo,0,sizeof(C179IDInfo)*MAX_179IDS_COUNT);
		m_nPerSendTime = 0;
		m_nPercent = 0;
		m_nColorInfo.nColorBegin = 0;
		m_nColorInfo.nColorEnd = 0;
		m_nNewPlayerLoginTimes = 0;
		m_nPerSendTimeMax = 0;
		m_nPerSendTimeMin = 0;
		return S_OK;
	}
	int32_t Uninitialize()
	{
		m_nrRoleIDInfo.nRoleIDBegin = 0;
		m_nrRoleIDInfo.nRoleIDEnd = 0;
		m_nRoleNameCount = 0;
		memset(m_arrRoleName,0,MaxRoleNameLength*MAX_NAME_COUNT);
		m_n179IDsCount = 0;
		memset(m_arr179IDInfo,0,sizeof(C179IDInfo)*MAX_179IDS_COUNT);
		m_nPerSendTime = 0;
		m_nPercent = 0;
		m_nColorInfo.nColorBegin = 0;
		m_nColorInfo.nColorEnd = 0;
		m_nNewPlayerLoginTimes = 0;
		m_nPerSendTimeMax = 0;
		m_nPerSendTimeMin = 0;
		return S_OK;
	}
	int32_t LoadRoleInfo(TiXmlElement* pRoot);

	CRoleIDInfo   GetRoleIDInfo()
	{
		return m_nrRoleIDInfo;
	}

	int32_t LoadRoleName(TiXmlElement* pRoot);

	uint32_t GetNameCount()
	{
		return m_nRoleNameCount;
	}

	const char* GetRoleName(const uint32_t nIndex)
	{
		if(nIndex>=(m_nRoleNameCount-1))
		{
			m_arrRoleName[m_nRoleNameCount-1].GetString();
		}
		return m_arrRoleName[nIndex].GetString();
	}

	int32_t	Load179IDInfo(TiXmlElement* pRoot);

	uint32_t Get179IDsCount()
	{
		return m_n179IDsCount;
	}

	C179IDInfo  Get179IDInfo(const uint32_t nIndex)
	{
		if(nIndex>=(m_n179IDsCount-1))
		{
			return m_arr179IDInfo[m_n179IDsCount-1];
		}
		return m_arr179IDInfo[nIndex];
	}
	int32_t	LoadColorSendInfo(TiXmlElement* pRoot);
	int32_t	LoadColorInfo(TiXmlElement* pRoot);
	int32_t	LoadNewPlayerLoginTimes(TiXmlElement* pRoot);
	int32_t	LoadBeginDistribution(TiXmlElement* pRoot);
	int32_t LoadHaveMic(TiXmlElement* pRoot);

	uint32_t GetSendTime()
	{
		return m_nPerSendTime;
	}
	uint32_t GetPercent()
	{
		return m_nPercent;
	}
	CColorInfo GetColorInfo()
	{
		return m_nColorInfo;
	}
	uint32_t GetNewPlayerLoginTimes()
	{
		return m_nNewPlayerLoginTimes;
	}
	uint32_t GetBeginDistribution()
	{
		return m_nBeginDistribution;
	}
	uint32_t GetSendTimeMin()
	{
		return m_nPerSendTimeMin;
	}
	uint32_t GetSendTimeMax()
	{
		return m_nPerSendTimeMax;
	}

	int32_t GetNextSentTime()
	{
		int32_t every = 0;
		if(m_nPerSendTimeMax <= m_nPerSendTimeMin)
		{
			every = 1;
		}
		else
		{
			every = m_nPerSendTimeMax - m_nPerSendTimeMin;
		}
		int32_t nNextSentTime = Random2(10000)%every;
		nNextSentTime = nNextSentTime + m_nPerSendTimeMin;

		return nNextSentTime;
	}
	int32_t GetMicRand()
	{
		return m_nRandMic;
	}

private:
	CRoleIDInfo						    m_nrRoleIDInfo;  //可分配的roleid信息;
	uint32_t                            m_nRoleNameCount;			   //名字的个数
	CString<MaxRoleNameLength> 			m_arrRoleName[MAX_NAME_COUNT]; //玩家昵称
	uint32_t							m_n179IDsCount;                //可分配的179的号码段个数
	C179IDInfo							m_arr179IDInfo[MAX_179IDS_COUNT]; //179的号码段信息
	uint32_t							m_nPerSendTime;        //每次发送的间隔
	uint32_t							m_nPercent;            //发送的概率
	CColorInfo							m_nColorInfo;   //可以发送的彩条的信息
	uint32_t							m_nNewPlayerLoginTimes; //界定新手的时间（小于这个登录次数发送彩条）
	uint32_t							m_nBeginDistribution; //开始分配机器人的真实玩家个数

	uint32_t							m_nPerSendTimeMin;       //每次发送的间隔的最小时间
	uint32_t							m_nPerSendTimeMax;       //每次发送的间隔的最大时间
	uint32_t                            m_nRandMic;               //机器人有摄像头概率
};


typedef CFrameConfigContainer<CRebotConfig>	CRebotConfigContainer;

#define INITIALIZE_REBOT_CONFIG			CSingleton<CRebotConfigContainer>::GetInstance().Initialize
#define GET_REBOT_CONFIG				CSingleton<CRebotConfigContainer>::GetInstance().GetConfigObject

#define g_RebotConfig GET_REBOT_CONFIG()

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* REBOTCONFIG_H_ */
