/*
 * someconfig.cpp
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_typedef.h"
#include "someconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define VIPS   "vips"
#define VIP   "vip"
#define LEVEL  "level"
#define WATCHCOUNT "watchcount"
#define ENTERROOMCOUNT "enterroomcount"
#define INVITEVIPCOUNT "invitevipcount"

#define ROOMTYPES   "roomtypes"
#define ROOMTYPE   "roomtype"
#define PLAYERCOUNT  "playercount"
#define HOSTCOUNT "hostcount"
#define SECHOSTCOUNT "sechostcount"
#define ADMCOUNT "admcount"
#define TEMPADMCOUNT "tempadmcount"
#define REBOTPERCENT "robotpercent"
#define MAXWAITCOUNT "waitcount"

#define TIMERS "timers"
#define PLAYERSTATIMER "playerstatimer"
#define ONLINETIMESTATIMER "onlinetimestatimer"
#define EVERY "every"
#define RUNTIME "runtime"
#define VALUE "value"

#define PLAYER_BEGIN   "playerbegin"
#define ROOM_BEGIN   "roombegin"

#define SHUT_UP_TIME   "shutuptime"

#define REQUESTSONG		"requestsong"
#define COST			"cost"

#define TICKET			"ticket"
#define REQUESTERVALUE	"requestervalue"
#define CHANCE			"chance"
#define PARAM			"param"
#define TICKETCOUNT		"ticketcount"

#define VOTE			"vote"
#define SHOWRESULTTIME	"can_enter_new_after_showresulttime"
#define SECTION			"section"
#define TIME			"time"

#define TEXT			"text"
#define PRIVATE			"private"
#define VIPLEVEL		"viplevel"
#define ROLERANK		"rolerank"
#define IDENTITYTYPE	"identitytype"
#define CLOSEPOORMAN	"closepoorman"
#define PUBLIC			"public"

int32_t CConfig::Initialize(const char* szFileName, const int32_t type)
{
	//加载配置文件
	TiXmlDocument doc(szFileName);
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		WRITE_ERROR_LOG("Load room config file failed!\n");
		return E_UNKNOWN;
	}

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{

		WRITE_ERROR_LOG("Root node cannot be found in config file!\n");
		return E_UNKNOWN;
	}

	int32_t ret = LoadVipInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadRoomTypeInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadTimerInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadBeginValue(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadShutUpTime(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadRequestSongCost(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadTicketConfig(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadVoteConfig(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadTextConfig(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	return ret;
}

int32_t CConfig::Reload(const char* szFileName, const int32_t type)
{
	return Initialize(szFileName,type);
}

int32_t CConfig::LoadVipInfo(TiXmlElement* pRoot)
{
	m_nVipCount = 0;
	memset(m_arrVipInfo,0,sizeof(CVipInfo)*MAX_VIP_COUNT);
	const char* pszValue = NULL;

	TiXmlElement *pVips = pRoot->FirstChildElement(VIPS);
	TiXmlElement *pVip = pVips->FirstChildElement(VIP);

	while(NULL != pVip)
	{
		VipLevel nVipLevel;
		pszValue = pVip->Attribute(LEVEL);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get VipLevel failed!\n");
			return E_UNKNOWN;
		}
		sscanf(pszValue,"%X",(uint32_t*)&nVipLevel);
		m_arrVipInfo[m_nVipCount].nVipLevel = nVipLevel;
		int32_t nMaxWatchCount;
		pszValue = pVip->Attribute(WATCHCOUNT, &nMaxWatchCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get MaxWatchCount failed!\n");
			return E_UNKNOWN;
		}
		m_arrVipInfo[m_nVipCount].nMaxWatchCount = nMaxWatchCount;
		int32_t nMaxEnterRoomCount;
		pszValue = pVip->Attribute(ENTERROOMCOUNT, &nMaxEnterRoomCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get MaxEnterRoomCount failed!\n");
			return E_UNKNOWN;
		}
		m_arrVipInfo[m_nVipCount].nMaxEnterRoomCount = nMaxEnterRoomCount;
		int32_t nMaxInviteVipCount;
		pszValue = pVip->Attribute(INVITEVIPCOUNT, &nMaxInviteVipCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get MaxInviteVipCount failed!\n");
			return E_UNKNOWN;
		}
		m_arrVipInfo[m_nVipCount].nMaxInviteVipCount = nMaxInviteVipCount;

		m_nVipCount++;
		pVip = pVip->NextSiblingElement();
	}
	return S_OK;
}

int32_t CConfig::LoadRoomTypeInfo(TiXmlElement* pRoot)
{
	m_nRoomTypeCount = 0;
	memset(m_arrRoomTypeInfo,0,sizeof(CRoomTypeInfo)*MAX_ROOM_TYPE_COUNT);
	const char* pszValue = NULL;

	TiXmlElement *pRoomTypes = pRoot->FirstChildElement(ROOMTYPES);
	TiXmlElement *pRoomType = pRoomTypes->FirstChildElement(ROOMTYPE);

	while(NULL != pRoomType)
	{
		int32_t nMaxPlayercount;
		pszValue = pRoomType->Attribute(PLAYERCOUNT, &nMaxPlayercount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get MaxPlayercount failed!\n");
			return E_UNKNOWN;
		}
		m_arrRoomTypeInfo[m_nRoomTypeCount].nMaxPlayercount = nMaxPlayercount;

		int32_t nMaxWaitCount;
		pszValue = pRoomType->Attribute(MAXWAITCOUNT, &nMaxWaitCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get RebotPercent failed!\n");
			return E_UNKNOWN;
		}
		m_arrRoomTypeInfo[m_nRoomTypeCount].nMaxWaitCount = nMaxWaitCount;

		m_nRoomTypeCount++;
		pRoomType = pRoomType->NextSiblingElement();
	}
	return S_OK;
}

int32_t CConfig::LoadTimerInfo(TiXmlElement* pRoot)
{
	m_nPlayerCountStaTimer = 0;
	m_nOnlineTimeStaTimer = 0;
	m_nRunTime = 0;
	const char* pszValue = NULL;

	TiXmlElement *pTimeers = pRoot->FirstChildElement(TIMERS);
	TiXmlElement *pPlayerSta = pTimeers->FirstChildElement(PLAYERSTATIMER);
	TiXmlElement *pOnlineTimeSta = pTimeers->FirstChildElement(ONLINETIMESTATIMER);
	int32_t nPlayerCountStaTimer;
	pszValue = pPlayerSta->Attribute(EVERY, &nPlayerCountStaTimer);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get PlayerCountStaTimer failed!\n");
		return E_UNKNOWN;
	}
	m_nPlayerCountStaTimer = nPlayerCountStaTimer;
	int32_t nOnlineTimeStaTimer;
	pszValue = pOnlineTimeSta->Attribute(EVERY, &nOnlineTimeStaTimer);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get OnlineTimeStaTimer failed!\n");
		return E_UNKNOWN;
	}
	m_nOnlineTimeStaTimer = nOnlineTimeStaTimer;
	int32_t nRunTime;
	pszValue = pOnlineTimeSta->Attribute(RUNTIME, &nRunTime);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get RunTime failed!\n");
		return E_UNKNOWN;
	}
	m_nRunTime = nRunTime;
	return S_OK;
}

int32_t CConfig::LoadBeginValue(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pPlayerBegin = pRoot->FirstChildElement(PLAYER_BEGIN);

	int32_t nPlayerBeginUpdate;
	pszValue = pPlayerBegin->Attribute(VALUE,&nPlayerBeginUpdate);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get Player Begin Update failed!\n");
		return E_UNKNOWN;
	}
	m_nUpdatePlayerBegin = nPlayerBeginUpdate;


	TiXmlElement *pRoomBegin = pRoot->FirstChildElement(ROOM_BEGIN);

	int32_t nRoomBeginUpdate;
	pszValue = pRoomBegin->Attribute(VALUE,&nRoomBeginUpdate);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get Room Begin Update failed!\n");
		return E_UNKNOWN;
	}
	m_nUpdateRoomBegin = nRoomBeginUpdate;

	return S_OK;
}

int32_t CConfig::LoadShutUpTime(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pShutUpTime = pRoot->FirstChildElement(SHUT_UP_TIME);

	int32_t nShutUpTime;
	pszValue = pShutUpTime->Attribute(VALUE,&nShutUpTime);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get shut up time failed!\n");
		return E_UNKNOWN;
	}
	m_nShutUpTime = nShutUpTime;

	return S_OK;
}

int32_t CConfig::LoadRequestSongCost(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pRequestSong = pRoot->FirstChildElement(REQUESTSONG);

	int32_t nRequestSongCost;
	pszValue = pRequestSong->Attribute(COST, &nRequestSongCost);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get request song cost failed!\n");
		return E_UNKNOWN;
	}
	m_nRequestSongCost = nRequestSongCost;

	return S_OK;
}

int32_t CConfig::LoadTicketConfig(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pTicket = pRoot->FirstChildElement(TICKET);

	int32_t nRequesterValue = 0;
	pszValue = pTicket->Attribute(REQUESTERVALUE, &nRequesterValue);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get requester value failed!\n");
		return E_UNKNOWN;
	}

	m_stTicketConfig.nRequesterTicketVaule = (uint16_t)nRequesterValue;

	int32_t nRequesterChance = 0;
	pszValue = pTicket->Attribute(CHANCE, &nRequesterChance);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get requester chance failed!\n");
		return E_UNKNOWN;
	}

	m_stTicketConfig.nRequesterChance = (uint16_t)nRequesterChance;

	TiXmlElement *pParam = pTicket->FirstChildElement(PARAM);

	m_stTicketConfig.nConfigCount = 0;

	while(pParam != NULL)
	{
		if(m_stTicketConfig.nConfigCount > MAX_TICKET_CONFIG_COUNT)
		{
			WRITE_ERROR_LOG("ticket config arrive max value{%d}!\n", MAX_TICKET_CONFIG_COUNT);
			break;
		}

		int32_t nPlayerCount = 0;
		pszValue = pParam->Attribute(PLAYERCOUNT, &nPlayerCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get ticket playercount failed!\n");
			return E_UNKNOWN;
		}

		int32_t nIndex = m_stTicketConfig.nConfigCount;
		m_stTicketConfig.arrPlayerCount[nIndex] = (uint16_t)nPlayerCount;

		int32_t nTicketCount = 0;
		pszValue = pParam->Attribute(TICKETCOUNT, &nTicketCount);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get ticket count failed!\n");
			return E_UNKNOWN;
		}

		m_stTicketConfig.arrTicketCount[nIndex] = (uint16_t)nTicketCount;

		++m_stTicketConfig.nConfigCount;

		pParam = pParam->NextSiblingElement();
	}

	return S_OK;
}

int32_t CConfig::LoadVoteConfig(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pVote = pRoot->FirstChildElement(VOTE);

	int32_t nShowResultTime = 0;
	pszValue = pVote->Attribute(SHOWRESULTTIME, &nShowResultTime);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get show result time value failed!\n");
		return E_UNKNOWN;
	}

	m_stVoteConfig.nShowResultTime = nShowResultTime;

	TiXmlElement *pParam = pVote->FirstChildElement(PARAM);

	m_stVoteConfig.nConfigCount = 0;

	while(pParam != NULL)
	{
		if(m_stVoteConfig.nConfigCount > MAX_VIP_COUNT)
		{
			WRITE_ERROR_LOG("vote config arrive max value{%d}!\n", MAX_TICKET_CONFIG_COUNT);
			break;
		}

		pszValue = pParam->Attribute(SECTION);
		if(NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get vote section failed!\n");
			return E_UNKNOWN;
		}

		int32_t nIndex = m_stVoteConfig.nConfigCount;
		if(strcmp(pszValue, "fightforticket") == 0)
		{
			m_stVoteConfig.arrProgramStatus[nIndex] = enmProgramStatus_FightingForTicket;
		}
		else if(strcmp(pszValue, "enjoy") == 0)
		{
			m_stVoteConfig.arrProgramStatus[nIndex] = enmProgramStatus_Enjoying;
		}
		else if(strcmp(pszValue, "vote") == 0)
		{
			m_stVoteConfig.arrProgramStatus[nIndex] = enmProgramStatus_Voteing;
		}
		else if(strcmp(pszValue, "showresult") == 0)
		{
			m_stVoteConfig.arrProgramStatus[nIndex] = enmProgramStatus_ShowResult;
		}
		else
		{
			WRITE_ERROR_LOG("Get unknown section!\n");
			return E_UNKNOWN;
		}

		int32_t nTime = 0;
		pszValue = pParam->Attribute(TIME, &nTime);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get time failed!\n");
			return E_UNKNOWN;
		}

		m_stVoteConfig.arrProgramTime[nIndex] = nTime;

		++m_stVoteConfig.nConfigCount;

		pParam = pParam->NextSiblingElement();
	}

	return S_OK;
}

int32_t CConfig::LoadTextConfig(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pText = pRoot->FirstChildElement(TEXT);
	if(pText == NULL)
	{
		WRITE_ERROR_LOG("Get text node failed!\n");
		return E_UNKNOWN;
	}

	TiXmlElement *pPrivate = pText->FirstChildElement(PRIVATE);
	if(pPrivate == NULL)
	{
		WRITE_ERROR_LOG("Get private node failed!\n");
		return E_UNKNOWN;
	}

	pszValue = pPrivate->Attribute(VIPLEVEL);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get private viplevel value failed!\n");
		return E_UNKNOWN;
	}

	int32_t nVipLevel = 0;
	sscanf(pszValue, "%x", (int32_t *)&nVipLevel);
	m_nCanPrivateTextVipLevel = (VipLevel)nVipLevel;

	pszValue = pPrivate->Attribute(ROLERANK);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get rolerank value failed!\n");
		return E_UNKNOWN;
	}
	int32_t nRoleRank = 0;
	sscanf(pszValue, "%x", (int32_t *)&nRoleRank);
	m_nCanPrivateTextRoleRank = (RoleRank)nRoleRank;

	pszValue = pPrivate->Attribute(IDENTITYTYPE);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get identitytype value failed!\n");
		return E_UNKNOWN;
	}
	int32_t nIdentityType = 0;
	sscanf(pszValue, "%x", (int32_t *)&nIdentityType);
	m_nCanPrivateTextIdentityType = (IdentityType)nIdentityType;

	pszValue = pPrivate->Attribute(CLOSEPOORMAN, &m_nClosePoorMan);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get closepoorman value failed!\n");
		return E_UNKNOWN;
	}

	TiXmlElement *pPublic = pText->FirstChildElement(PUBLIC);
	if(pPublic == NULL)
	{
		WRITE_ERROR_LOG("Get public node failed!\n");
		return E_UNKNOWN;
	}

	pszValue = pPublic->Attribute(VIPLEVEL);
	if(NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get public viplevel value failed!\n");
		return E_UNKNOWN;
	}
	sscanf(pszValue, "%x", (int32_t *)&nVipLevel);
	m_nCanPublicTextVipLevel = (VipLevel)nVipLevel;

	return S_OK;
}
FRAME_ROOMSERVER_NAMESPACE_END
