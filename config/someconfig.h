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
	VipLevel 		nVipLevel;//vip�ȼ�
	uint16_t 		nMaxWatchCount; //���Թۿ�˽����������
	uint16_t		nMaxEnterRoomCount;//���Խ�����������
	uint16_t		nMaxInviteVipCount;//������Լ�û�Ϊ��ɫVip������ /ÿ��
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
	uint16_t 		nMaxPlayercount;  //��������������
	uint16_t 		nMaxHoatCount; //�������������
	uint16_t		nMaxSecHostCount;//���������������
	uint16_t		nMaxAdmCount;//����Ա���������
	uint16_t		nMaxTempAdmCount;//��ʱ����Ա���������
	uint16_t		nMaxWaitCount;   //����������
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

	//���ݷ���ʵ���û����������ЧƱ��
	uint16_t GetValidTicketsCount(int32_t nTotalPlayerCount)
	{
		//����������������е�������������������е������������Ӧ��Ʊ��
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
	uint16_t		m_nVipCount;//���ڵ�VIP����
	CVipInfo 		m_arrVipInfo[MAX_VIP_COUNT];  //����������ж������й�VIP����Ϣ
	uint16_t		m_nRoomTypeCount;//���ڷ������������
	CRoomTypeInfo   m_arrRoomTypeInfo[MAX_ROOM_TYPE_COUNT]; ////����������ж������йط������͵���Ϣ
	uint32_t 		m_nPlayerCountStaTimer;                      //ͳ���ڷ���������ʱ��
	uint32_t 		m_nOnlineTimeStaTimer;                      //ͳ�ƹ���Ա�ڷ���
	uint32_t		m_nRunTime;							   //ִ�е�ʱ��

	int32_t		m_nUpdatePlayerBegin;    	//��ҿ�ʼ�������ݵ�roomdispatcher������������������
	int32_t		m_nUpdateRoomBegin;    		//���俪ʼ�������ݵ�roomdispatcher������������������

	int32_t		m_nShutUpTime;				//����ʱ��

	int32_t		m_nRequestSongCost;			//������

	CTicketConfig	m_stTicketConfig;

	CVoteConfig		m_stVoteConfig;

	VipLevel	m_nCanPrivateTextVipLevel;	//����˽�ĵ�vip�ȼ�
	RoleRank	m_nCanPrivateTextRoleRank;	//����˽�ĵĹ���ȼ�
	IdentityType	m_nCanPrivateTextIdentityType;	//����˽�ĵ����
	int32_t		m_nClosePoorMan;			//��Ϊǰ��������IdentityType�и��ֶ���uint32�ģ�������Ҫ�ر������Ĵ���˿�û���û�а취�����ģ�ֻ���ټ�һ��ѡ���ֶ�
	VipLevel	m_nCanPublicTextVipLevel;	//���Թ��ĵ�vip�ȼ�
};


typedef CFrameConfigContainer<CConfig>	CConfigContainer;

#define INITIALIZE_CONFIG		CSingleton<CConfigContainer>::GetInstance().Initialize
#define GET_CONFIG				CSingleton<CConfigContainer>::GetInstance().GetConfigObject

#define g_SomeConfig GET_CONFIG()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* SOMECONFIG_H_ */
