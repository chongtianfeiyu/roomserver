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


//定义玩家对象池和索引类型
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
	//初始化玩家管理器
	virtual int32_t Initialize();
	//恢复玩家管理器
	virtual int32_t Resume();
	//注销玩家管理器
	virtual int32_t Uninitialize();

public:
	//创建玩家对象
	int32_t CreatePlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex);
	//创建玩家对象
	int32_t CreatePlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex);

	//获取玩家对象
	int32_t GetPlayer(const RoleID roleID, PlayerPool::CIndex*& pIndex);
	//获取玩家对象
	int32_t GetPlayer(const RoleID roleID, CPlayer*& pPlayer, PlayerIndex& playerIndex);

	//获取玩家对象
	int32_t GetPlayerByIndex(const PlayerIndex playerIndex, PlayerPool::CIndex*& pIndex);
	//获取玩家对象
	int32_t GetPlayerByIndex(const PlayerIndex playerIndex, CPlayer*& pPlayer);

	//销毁玩家对象
	int32_t DestroyPlayer(const RoleID roleID);
	//销毁玩家对象
	int32_t DestroyPlayerByIndex(const PlayerIndex playerIndex);
	//销毁玩家对象
	int32_t DestroyPlayer(PlayerPool::CIndex* pIndex);

	//清空玩家管理器
	int32_t Clear();

	//获取第一个玩家
	PlayerPool::CIndex* GetFirstPlayer();

	//获得playerIndex
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
	//获取对象池容量
	uint32_t GetCapacity() const;
	//获取对象池中对象数量
	uint32_t GetPlayerCount() const;
	//对象池是否为空
	bool IsEmpty() const;
	//对象池是否已满
	bool IsFull() const;

protected:
	//根据角色ID结算Key
	PlayerList::Key MakeKey(const RoleID roleID) const;

protected:
	PlayerPool				m_playerPool;			//玩家对象池
	PlayerList				m_playerList;			//玩家对象索引

};



FRAME_ROOMSERVER_NAMESPACE_END


#endif /* PLAYER_MGT_H_ */
