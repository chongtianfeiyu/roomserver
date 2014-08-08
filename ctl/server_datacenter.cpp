/*
 * hallserver_datacenter.cpp
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#include "../../lightframe/frame_errordef.h"
#include "def/server_errordef.h"
#include "server_datacenter.h"
#include "def/dal_def.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN


CServerDataCenter::CServerDataCenter()
{
	m_pPlayerMgt = NULL;
	m_pChannelMgt = NULL;
	m_pRoomMgt = NULL;
}

CServerDataCenter::~CServerDataCenter()
{

}

size_t CServerDataCenter::GetSize()
{
	return sizeof(CPlayerMgt) + sizeof(CChannelMgt) + sizeof(CRoomMgt);

}
const char* CServerDataCenter::GetName()
{
	return SERVER_NAME_STRING"DataCenter";
}
//初始化数据中心
int32_t CServerDataCenter::Initialize()
{
	m_pPlayerMgt = new CPlayerMgt();
	if(NULL == m_pPlayerMgt)
	{
		return E_RS_CREATEPLAYERMGTERROR;
	}

	int32_t nRet = m_pPlayerMgt->Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	m_pRoomMgt = new CRoomMgt();
	if(NULL == m_pRoomMgt)
	{
		return E_RS_CREATEROOMMGTERROR;
	}

	nRet = m_pRoomMgt->Initialize();
	if(0 > nRet)
	{
		return nRet;
	}

	m_pChannelMgt = new CChannelMgt();
	if(NULL == m_pChannelMgt)
	{
		return E_RS_CREATECHANNELMGTERROR;
	}

	nRet = m_pChannelMgt->Initialize();
	if(0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

//恢复数据中心
int32_t CServerDataCenter::Resume()
{
	m_pPlayerMgt = new CPlayerMgt();
	if(NULL == m_pPlayerMgt)
	{
		return E_RS_CREATEPLAYERMGTERROR;
	}

	int32_t nRet = m_pPlayerMgt->Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	m_pRoomMgt = new CRoomMgt();
	if(NULL == m_pRoomMgt)
	{
		return E_RS_CREATEROOMMGTERROR;
	}

	nRet = m_pRoomMgt->Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	m_pChannelMgt = new CChannelMgt();
	if(NULL == m_pChannelMgt)
	{
		return E_RS_CREATECHANNELMGTERROR;
	}

	nRet = m_pChannelMgt->Resume();
	if(0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

//注销数据中心
int32_t CServerDataCenter::Uninitialize()
{
	if(m_pPlayerMgt!=NULL)
	{
		delete m_pPlayerMgt;
		m_pPlayerMgt = NULL;
	}

	if( m_pRoomMgt!=NULL)
	{
		delete m_pRoomMgt;
		m_pRoomMgt = NULL;
	}

	if(m_pChannelMgt!=NULL )
	{
		delete m_pChannelMgt;
		m_pChannelMgt = NULL;
	}

	return S_OK;
}

int32_t CServerDataCenter::GetAllPlayers(const RoomID nRoomID, CPlayer *arrPlayer[],
		const int32_t arrSize, int32_t &nPlayerCount)
{
	int32_t ret = 0;
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		return ret;
	}

	PlayerIndex arrPlayerIndex[MaxUserCountPerRoom] = { enmInvalidPlayerIndex };
	nPlayerCount = 0;
	pRoom->GetAllPlayers(arrPlayerIndex, MaxUserCountPerRoom, nPlayerCount);
	int32_t nPlayerCountTem = nPlayerCount;
	nPlayerCount=0;
	for(int32_t i = 0; i < nPlayerCountTem; ++i)
	{
		CPlayer *pPlayer = NULL;
		g_PlayerMgt.GetPlayerByIndex(arrPlayerIndex[i], pPlayer);
		if(pPlayer == NULL)
		{
			continue;
		}

		arrPlayer[nPlayerCount] = pPlayer;
		nPlayerCount++;
	}

	return S_OK;
}

int32_t CServerDataCenter::GetAllPlayers(CRoom *pRoom, CPlayer *arrPlayer[],
		const int32_t arrSize, int32_t &nPlayerCount)
{
	if(pRoom == NULL)
	{
		return S_OK;
	}

	PlayerIndex arrPlayerIndex[MaxUserCountPerRoom] = { enmInvalidPlayerIndex };
	nPlayerCount = 0;
	pRoom->GetAllPlayers(arrPlayerIndex, MaxUserCountPerRoom, nPlayerCount);

	nPlayerCount = nPlayerCount > arrSize ? arrSize : nPlayerCount;

	int32_t nPlayerCountTem = nPlayerCount;
	nPlayerCount = 0;
	for(int32_t i = 0; i < nPlayerCountTem; ++i)
	{
		CPlayer *pPlayer = NULL;
		g_PlayerMgt.GetPlayerByIndex(arrPlayerIndex[i], pPlayer);
		if(pPlayer == NULL)
		{
			continue;
		}
		arrPlayer[nPlayerCount] = pPlayer;
		nPlayerCount++;
	}

	return S_OK;
}

int32_t CServerDataCenter::GetAllPlayers(CRoom *pRoom, RoleID arrRoleID[],
		const int32_t arrSize, int32_t &nPlayerCount)
{
	if(pRoom == NULL)
	{
		return S_OK;
	}

	CPlayer *arrPlayer[MaxUserCountPerRoom];
	nPlayerCount = 0;

	GetAllPlayers(pRoom, arrPlayer, MaxUserCountPerRoom, nPlayerCount);

	nPlayerCount = nPlayerCount > arrSize ? arrSize : nPlayerCount;
	int32_t nPlayerCountTem = nPlayerCount;
	nPlayerCount=0;
	for(int32_t i = 0; i < nPlayerCountTem; ++i)
	{
		if(arrPlayer[i]==NULL)
		{
			continue;
		}
		arrRoleID[nPlayerCount] = arrPlayer[i]->GetRoleID();
		nPlayerCount++;
	}

	return S_OK;
}

void CServerDataCenter::RegistMediaServerInfo(MediaServerInfo stMediaServerInfo)
{
	for(int32_t i = 0; i < m_nMediaServerCount; ++i)
	{
		if(m_arrMediaServerInfo[i].m_nServerID == stMediaServerInfo.m_nServerID)
		{
			m_arrMediaServerInfo[i] = stMediaServerInfo;
			return;
		}
	}

	m_arrMediaServerInfo[m_nMediaServerCount] = stMediaServerInfo;
	++m_nMediaServerCount;
}

//取得Mediaserver的IP和端口(Mediaserver与网关的serverID必须配置一样)
MediaServerInfo CServerDataCenter::GetMediaServerInfo(ServerID nServerID)
{
	MediaServerInfo stInfo;
	for(int32_t i = 0; i < m_nMediaServerCount; ++i)
	{
		if(m_arrMediaServerInfo[i].m_nServerID == nServerID)
		{
			stInfo = m_arrMediaServerInfo[i];
			break;
		}
	}

	return stInfo;
}

ConnInfo *CServerDataCenter::GetConnInfo(RoleID nRoleID)
{
	CConnInfoMgt::iterator it = g_ConnInfoMgt.find(nRoleID);
	if(it != g_ConnInfoMgt.end())
	{
		return it->second;
	}

	return NULL;
}

FRAME_ROOMSERVER_NAMESPACE_END


