/*
 * player_mgt.h
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#ifndef PLAYER_MGT_H_
#define PLAYER_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "def/server_namespace.h"
#include "def/dal_def.h"
#include "dal/dal_player.h"
#include "public_typedef.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


//������Ҷ���غ���������
typedef CPool<CPlayer, MaxPlayerCountPerRoomServer>	PlayerPool;
typedef CHashList<PlayerIndex, MaxPlayerCountPerRoomServer, MaxPlayerCountPerRoomServer * 256>	PlayerList;


class CPlayerMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_HashListIndex			= 0,
	};
public:
	CPlayerMgt();
	virtual ~CPlayerMgt();

public:
	//��ʼ����ҹ�����
	virtual int32_t Initialize();
	//�ָ���ҹ�����
	virtual int32_t Resume();
	//ע����ҹ�����
	virtual int32_t Uninitialize();

public:
	//������Ҷ���
	int32_t CreatePlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex);
	//������Ҷ���
	int32_t CreatePlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex);

	//��ȡ��Ҷ���
	int32_t GetPlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex);
	//��ȡ��Ҷ���
	int32_t GetPlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex);

	//��ȡ��Ҷ���
	int32_t GetPlayerByIndex(const PlayerIndex playerIndex, PlayerPool::CIndex*& pIndex);
	//��ȡ��Ҷ���
	int32_t GetPlayerByIndex(const PlayerIndex playerIndex, CPlayer*& pPlayer);

	//������Ҷ���
	int32_t DestroyPlayer(const RoleID roleID);
	//������Ҷ���
	int32_t DestroyPlayerByIndex(const PlayerIndex playerIndex);
	//������Ҷ���
	int32_t DestroyPlayer(PlayerPool::CIndex* pIndex);

	//�����ҹ�����
	int32_t Clear();

	//��ȡ��һ�����
	PlayerPool::CIndex* GetFirstPlayer();

	//���playerIndex
	int32_t GetPlayerIndex(const RoleID roleID,PlayerIndex& playerIndex)
	{
		PlayerPool::CIndex* pIndex = NULL;
		int32_t ret = GetPlayer(roleID, pIndex);
		if (0 > ret)
		{
			return ret;
		}
		playerIndex = pIndex->Index();
		return S_OK;
	}

	int32_t GetAllPlayer(CPlayer *arrPlayrObject[], const int32_t arrSize, int32_t &nPlayerObjectCount);
	int32_t GetAllPlayer(RoleID arrRoleID[], const int32_t arrSize, int32_t &nPlayerObjectCount);
public:
	//��ȡ���������
	uint32_t GetCapacity() const;
	//��ȡ������ж�������
	uint32_t GetPlayerCount() const;
	//������Ƿ�Ϊ��
	bool IsEmpty() const;
	//������Ƿ�����
	bool IsFull() const;

protected:
	//���ݽ�ɫID����Key
	PlayerList::Key MakeKey(const RoleID roleID) const;

protected:
	PlayerPool				m_playerPool;			//��Ҷ����
	PlayerList				m_playerList;			//��Ҷ�������

};



FRAME_ROOMSERVER_NAMESPACE_END


#endif /* PLAYER_MGT_H_ */
