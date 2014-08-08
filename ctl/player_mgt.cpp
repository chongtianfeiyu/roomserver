/*
 * player_mgt.cpp
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#include "../../lightframe/frame_errordef.h"
#include "def/server_errordef.h"
#include "player_mgt.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


CPlayerMgt::CPlayerMgt()
{

}

CPlayerMgt::~CPlayerMgt()
{

}

//��ʼ����������
int32_t CPlayerMgt::Initialize()
{
	m_playerPool.Initialize();
	m_playerList.Initialize();
	return S_OK;
}

//�ָ���������
int32_t CPlayerMgt::Resume()
{
	m_playerPool.Resume();
	m_playerList.Resume();
	return S_OK;
}

//ע����ҹ�����
int32_t CPlayerMgt::Uninitialize()
{
	m_playerPool.Uninitialize();
	m_playerList.Uninitialize();
	return S_OK;
}

//������Ҷ���
int32_t CPlayerMgt::CreatePlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex)
{
	//��������
	pIndex = m_playerPool.CreateObject();
	if (NULL == pIndex)
	{
		return E_LIBF_CREATEPLAYER;
	}

	//��������
	PlayerList::Key key = MakeKey(roleID);
	PlayerList::CIndex* pListIndex = m_playerList.Insert(key, pIndex->Index());
	if (NULL == pListIndex)
	{
		m_playerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_LIBF_CREATEPLAYERINDEX;
	}

	//���������浽�������ݱ���
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_HashListIndex, (uint32_t)pListIndex->Index());
	if (0 > ret)
	{
		m_playerList.Erase(pListIndex);
		m_playerPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	return S_OK;
}

//������Ҷ���
int32_t CPlayerMgt::CreatePlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex)
{
	PlayerPool::CIndex* pIndex = NULL;

	int32_t ret = CreatePlayer(roleID, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pPlayer = pIndex->ObjectPtr();
	playerIndex = pIndex->Index();

	pPlayer->SetRoleID(roleID);

	return S_OK;
}

//��ȡ��Ҷ���
int32_t CPlayerMgt::GetPlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex)
{
	//������������
	PlayerList::Key key = MakeKey(roleID);
	PlayerList::CIndex* pListIndex = m_playerList.Find(key);
	if (NULL == pListIndex)
	{
		return E_LIBF_PLAYERNOTFOUND;
	}

	PlayerIndex& playerIndex = pListIndex->GetObject();
	pIndex = m_playerPool.GetIndex(playerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_PLAYERNOTFOUND;
	}

	return S_OK;
}

//��ȡ��Ҷ���
int32_t CPlayerMgt::GetPlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex)
{
	PlayerPool::CIndex* pIndex = NULL;

	int32_t ret = GetPlayer(roleID, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pPlayer = pIndex->ObjectPtr();
	playerIndex = pIndex->Index();

	return S_OK;
}

//��ȡ��Ҷ���
int32_t CPlayerMgt::GetPlayerByIndex(const PlayerIndex playerIndex, PlayerPool::CIndex*& pIndex)
{
	pIndex = m_playerPool.GetIndex(playerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_PLAYERNOTFOUND;
	}

	return S_OK;
}

//��ȡ��Ҷ���
int32_t CPlayerMgt::GetPlayerByIndex(const PlayerIndex playerIndex, CPlayer*& pPlayer)
{
	PlayerPool::CIndex* pIndex = NULL;

	int32_t ret = GetPlayerByIndex(playerIndex, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pPlayer = pIndex->ObjectPtr();

	return S_OK;
}

//������Ҷ���
int32_t CPlayerMgt::DestroyPlayer(const RoleID roleID)
{
	//������������
	PlayerList::Key key = MakeKey(roleID);
	PlayerList::CIndex* pListIndex = m_playerList.Find(key);
	if (NULL == pListIndex)
	{
		return E_LIBF_PLAYERNOTEXIST;
	}

	PlayerIndex playerIndex = pListIndex->GetObject();

	//����ҴӶ���ؼ���������ɾ��
	m_playerList.Erase(pListIndex);
	m_playerPool.DestroyObject(playerIndex);

	return S_OK;
}

//������Ҷ���
int32_t CPlayerMgt::DestroyPlayerByIndex(const PlayerIndex playerIndex)
{
	PlayerPool::CIndex* pIndex = m_playerPool.GetIndex(playerIndex);
	if (NULL == pIndex)
	{
		return E_LIBF_PLAYERNOTEXIST;
	}

	return DestroyPlayer(pIndex);
}

//������Ҷ���
int32_t CPlayerMgt::DestroyPlayer(PlayerPool::CIndex* pIndex)
{
	if (NULL == pIndex)
	{
		return E_INVALIDARGUMENT;
	}

	uint32_t nAdditionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_HashListIndex, nAdditionalValue);
	PlayerList::CIndex* pListIndex = m_playerList.GetIndex((int32_t)nAdditionalValue);

	//����ҴӶ���ؼ���������ɾ��
	m_playerList.Erase(pListIndex);
	m_playerPool.DestroyObject(pIndex);

	return S_OK;
}

//�����ҹ�����
int32_t CPlayerMgt::Clear()
{
	m_playerPool.Clear();
	m_playerList.Clear();
	return S_OK;
}

//��ȡ��һ�����
PlayerPool::CIndex* CPlayerMgt::GetFirstPlayer()
{
	return m_playerPool.First();
}

//��ȡ���������
uint32_t CPlayerMgt::GetCapacity() const
{
	return m_playerPool.Capacity();
}

//��ȡ������ж�������
uint32_t CPlayerMgt::GetPlayerCount() const
{
	return m_playerPool.ObjectCount();
}
//��ȡ�������
int32_t CPlayerMgt::GetAllPlayer(CPlayer *arrPlayrObject[], const int32_t arrSize, int32_t &nPlayerObjectCount)
{
	nPlayerObjectCount = 0;
	PlayerPool::CIndex *pIndex = m_playerPool.First();

	while(NULL != pIndex && nPlayerObjectCount < arrSize)
	{
		arrPlayrObject[nPlayerObjectCount++] = pIndex->ObjectPtr();
		pIndex = pIndex->Next();
	}

	return S_OK;
}

//��ȡ�������
int32_t CPlayerMgt::GetAllPlayer(RoleID arrRoleID[], const int32_t arrSize, int32_t &nPlayerObjectCount)
{
	nPlayerObjectCount = 0;
	PlayerPool::CIndex *pIndex = m_playerPool.First();

	while(NULL != pIndex && nPlayerObjectCount < arrSize)
	{
		arrRoleID[nPlayerObjectCount++] = pIndex->ObjectPtr()->GetRoleID();
		pIndex = pIndex->Next();
	}

	return S_OK;
}
//������Ƿ�Ϊ��
bool CPlayerMgt::IsEmpty() const
{
	return m_playerPool.IsEmpty();
}

//������Ƿ�����
bool CPlayerMgt::IsFull() const
{
	return m_playerPool.IsFull();
}

//���ݽ�ɫID����Key
PlayerList::Key CPlayerMgt::MakeKey(const RoleID roleID) const
{
	PlayerList::Key key = { 0 };
	key.nKeyLength = (uint32_t)sizeof(RoleID);
	*(RoleID*)key.arrKey = roleID;

	return key;
}


FRAME_ROOMSERVER_NAMESPACE_END


