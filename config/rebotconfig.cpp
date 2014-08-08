/*
 *  rebotconfig.cpp
 *
 *  To do：
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_typedef.h"
#include "rebotconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define ROLEID   "roleid"
#define BEGIN   "begin"
#define END  "end"

#define _179IDS  "n179ids"
#define _179ID  "n179id"

#define NAMES   "names"
#define NAME   "name"
#define VALUE  "value"

#define COLORSEND "colorsend"
#define EVERYMIN "everymin"
#define EVERYMAX "everymax"

#define COLOR "color"

#define NEWPLAYER "newplayer"

#define BEGINDIS "begindistribution"

#define HAVEMICRAND "havemicrand"

int32_t CRebotConfig::Initialize( const char* szFileName, const int32_t nType)
{
	//加载配置文件
	TiXmlDocument doc(szFileName);
	if (!doc.LoadFile(TIXML_ENCODING_UTF8))
	{
		WRITE_ERROR_LOG("Load rebot config file failed!\n");
		return E_UNKNOWN;
	}

	//获取根节点
	TiXmlElement *pRoot = doc.RootElement();
	if (NULL == pRoot)
	{

		WRITE_ERROR_LOG("Root node cannot be found in config file!\n");
		return E_UNKNOWN;
	}

	int32_t ret = LoadRoleInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadRoleName(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = Load179IDInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadColorSendInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}

	ret = LoadColorInfo(pRoot);
	if (0 > ret)
	{
		return ret;
	}
	ret = LoadNewPlayerLoginTimes(pRoot);
	if (0 > ret)
	{
		return ret;
	}
	ret = LoadBeginDistribution(pRoot);
	if (0 > ret)
	{
		return ret;
	}
	ret = LoadHaveMic(pRoot);
	if (0 > ret)
	{
		return ret;
	}
	return ret;
}

int32_t CRebotConfig::Reload(const char* szFileName, const int32_t type)
{
	return Initialize(szFileName,type);
}

int32_t CRebotConfig::LoadRoleInfo(TiXmlElement* pRoot)
{
	m_nrRoleIDInfo.nRoleIDBegin = 0;
	m_nrRoleIDInfo.nRoleIDEnd = 0;
	const char* pszValue = NULL;

	TiXmlElement *pRoleID = pRoot->FirstChildElement(ROLEID);

	int32_t nRoleIDBegin;
	pszValue = pRoleID->Attribute(BEGIN,&nRoleIDBegin);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get RoleID begin failed!\n");
		return E_UNKNOWN;
	}
	m_nrRoleIDInfo.nRoleIDBegin = nRoleIDBegin;
	int32_t nRoleIDEnd;
	pszValue = pRoleID->Attribute(END, &nRoleIDEnd);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get RoleID end failed!\n");
		return E_UNKNOWN;
	}
	m_nrRoleIDInfo.nRoleIDEnd = nRoleIDEnd;
	return S_OK;

}

int32_t CRebotConfig::LoadRoleName(TiXmlElement* pRoot)
{
	m_nRoleNameCount = 0;
	memset(m_arrRoleName,0,MaxRoleNameLength*MAX_NAME_COUNT);
	const char* pszValue = NULL;

	TiXmlElement *pRoleNames = pRoot->FirstChildElement(NAMES);
	TiXmlElement *pRoleName = pRoleNames->FirstChildElement(NAME);

	while(NULL != pRoleName)
	{
		pszValue = pRoleName->Attribute(VALUE);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get Role Name failed!\n");
			return E_UNKNOWN;
		}
		m_arrRoleName[m_nRoleNameCount] = pszValue;
		m_nRoleNameCount++;
		pRoleName = pRoleName->NextSiblingElement();
	}
	return S_OK;
}

int32_t CRebotConfig::Load179IDInfo(TiXmlElement* pRoot)
{
	m_n179IDsCount = 0;
	memset(m_arr179IDInfo,0,sizeof(C179IDInfo)*MAX_179IDS_COUNT);
	const char* pszValue = NULL;

	TiXmlElement *p179IDs = pRoot->FirstChildElement(_179IDS);
	TiXmlElement *p179ID = p179IDs->FirstChildElement(_179ID);

	while(NULL != p179ID)
	{
		int32_t n179IDBegin;
		pszValue = p179ID->Attribute(BEGIN,&n179IDBegin);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get 179ID begin failed!\n");
			return E_UNKNOWN;
		}
		m_arr179IDInfo[m_n179IDsCount].n179IDBegin = n179IDBegin;
		int32_t n179IDEnd;
		pszValue = p179ID->Attribute(END, &n179IDEnd);
		if (NULL == pszValue)
		{
			WRITE_ERROR_LOG("Get 179ID end failed!\n");
			return E_UNKNOWN;
		}
		m_arr179IDInfo[m_n179IDsCount].n179IDEnd= n179IDEnd;
		m_n179IDsCount++;
		p179ID = p179ID->NextSiblingElement();
	}
	return S_OK;
}

int32_t CRebotConfig::LoadColorSendInfo(TiXmlElement* pRoot)
{
	m_nPerSendTimeMin = 0;
	m_nPerSendTimeMax = 0;
	const char* pszValue = NULL;
	TiXmlElement *pColorSend = pRoot->FirstChildElement(COLORSEND);
	int32_t nEveryMin;
	pszValue = pColorSend->Attribute(EVERYMIN,&nEveryMin);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get color send every min failed!\n");
		return E_UNKNOWN;
	}
	m_nPerSendTimeMin = nEveryMin;
	int32_t nEveryMax;
	pszValue = pColorSend->Attribute(EVERYMAX, &nEveryMax);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get color send every max failed!\n");
		return E_UNKNOWN;
	}
	m_nPerSendTimeMax = nEveryMax;

	return S_OK;
}

int32_t CRebotConfig::LoadColorInfo(TiXmlElement* pRoot)
{
	m_nColorInfo.nColorBegin = 0;
	m_nColorInfo.nColorEnd = 0;
	const char* pszValue = NULL;

	TiXmlElement *pColorInfo = pRoot->FirstChildElement(COLOR);

	int32_t nColorBegin;
	pszValue = pColorInfo->Attribute(BEGIN,&nColorBegin);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get Color Begin failed!\n");
		return E_UNKNOWN;
	}
	m_nColorInfo.nColorBegin = nColorBegin;
	int32_t nColorEnd;
	pszValue = pColorInfo->Attribute(END, &nColorEnd);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get Color End failed!\n");
		return E_UNKNOWN;
	}
	m_nColorInfo.nColorEnd = nColorEnd;
	return S_OK;

}

int32_t CRebotConfig::LoadNewPlayerLoginTimes(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pNewPlayerTime = pRoot->FirstChildElement(NEWPLAYER);

	int32_t nNewPlayerLoginTimes;
	pszValue = pNewPlayerTime->Attribute(VALUE,&nNewPlayerLoginTimes);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get NewPlayer login times failed!\n");
		return E_UNKNOWN;
	}
	m_nNewPlayerLoginTimes = nNewPlayerLoginTimes;
	return S_OK;

}
int32_t CRebotConfig::LoadBeginDistribution(TiXmlElement* pRoot)
{
	const char* pszValue = NULL;

	TiXmlElement *pBeginDistribution = pRoot->FirstChildElement(BEGINDIS);

	int32_t nBeginDistribution;
	pszValue = pBeginDistribution->Attribute(VALUE,&nBeginDistribution);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get BeginDistribution failed!\n");
		return E_UNKNOWN;
	}
	m_nBeginDistribution = nBeginDistribution;
	return S_OK;

}
int32_t CRebotConfig::LoadHaveMic(TiXmlElement* pRoot)
{
	m_nRandMic = 0;

	const char* pszValue = NULL;

	TiXmlElement *pMicRand = pRoot->FirstChildElement(HAVEMICRAND);

	int32_t nMicRand;
	pszValue = pMicRand->Attribute(VALUE,&nMicRand);
	if (NULL == pszValue)
	{
		WRITE_ERROR_LOG("Get mic rand value failed!\n");
		return E_UNKNOWN;
	}
	m_nRandMic = nMicRand;
	return S_OK;

}

FRAME_ROOMSERVER_NAMESPACE_END
