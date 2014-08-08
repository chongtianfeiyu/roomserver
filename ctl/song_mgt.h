/*
 * song_mgt.h
 *
 *  Created on: 2013-3-7
 *      Author: jimm
 */

#ifndef SONG_MGT_H_
#define SONG_MGT_H_

#include <map>
#include "public_typedef.h"
#include "common/common_object.h"
#include "common/common_pool.h"
#include "def/server_namespace.h"
#include "common/common_singleton.h"
#include "common/common_string.h"
#include "def/dal_def.h"

using namespace std;

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class SongInfo
{
public:
	CString<MaxSongNameLength>		strSongName;
	uint32_t		nSongAttr;

	SongInfo()
	{
		Reset();
	}

	void Reset()
	{
		strSongName.Initialize();
		nSongAttr = 0;
	}
};

class SongList : public CObject
{
public:
	SongList()
	{

	}

	virtual ~SongList()
	{

	}

	//初始化玩家管理器
	virtual int32_t Initialize()
	{
		m_nSongCount = 0;
		return S_OK;
	}
	//恢复玩家管理器
	virtual int32_t Resume()
	{
		m_nSongCount = 0;
		return S_OK;
	}
	//注销玩家管理器
	virtual int32_t Uninitialize()
	{
		m_nSongCount = 0;
		return S_OK;
	}

	uint16_t							m_nSongCount;							//当前歌曲数目
	SongInfo							m_arrSongList[MaxSongCountPerPlayer];	//歌曲列表
};

typedef CPool<SongList, MaxSongListMgtSize>		SongListPool;
typedef map<RoleID, SongList *>					PlayerSongListMgt;

class CPlayerSongListMgt : public CObject
{
public:
	CPlayerSongListMgt();

	virtual ~CPlayerSongListMgt();
	//初始化玩家管理器
	virtual int32_t Initialize();
	//恢复玩家管理器
	virtual int32_t Resume();
	//注销玩家管理器
	virtual int32_t Uninitialize();

	//创建歌单
	SongList *CreateSongList();
	//销毁歌单
	int32_t DestroySongList(RoleID nRoleID);
	//更新玩家歌单
	int32_t UpdatePlayerSongList(RoleID nRoleID, SongList *pList);
	//获取玩家歌单
	SongList *GetPlayerSongList(RoleID nRoleID);

protected:
	PlayerSongListMgt				m_stPlayerSongListMgt;
	SongListPool					m_stSongListPool;
};

#define	CREATE_PLAYERSONGLISTMGT_INSTANCE		CSingleton<CPlayerSongListMgt>::CreateInstance
#define	GET_PLAYERSONGLISTMGT_INSTANCE			CSingleton<CPlayerSongListMgt>::GetInstance
#define	DESTROY_PLAYERSONGLISTMGT_INSTANCE		CSingleton<CPlayerSongListMgt>::DestroyInstance

#define g_PlayerSongListMgt		GET_PLAYERSONGLISTMGT_INSTANCE()

class SongOrderInfo
{
public:
	uint32_t					nOrderID;
	CString<enmMaxSerialNumberLength>	strTransID;
	RoomID						nRoomID;			//房间号
	int64_t						nOrderStartTime;	//订单生成时间
	int64_t						nOrderEndTime;		//订单结束时间
	OrderStatus					nOrderStatus;		//订单状态
	CString<MaxSongNameLength>	strSongName;		//点的歌曲名
	RoleID						nSongerRoleID;		//歌手roleid
	CString<MaxRoleNameLength>	strSongerName;		//歌手昵称
	RoleID						nRequestRoleID;		//点歌者roleid
	CString<MaxRoleNameLength>	strRequestName;		//点歌玩家昵称
	CString<MaxWishWordsLength>	strWishWords;		//祝福语
	uint32_t					nOrderCost;			//点歌费用
	uint16_t					nSongCount;			//点了几首歌
	SongVoteResult				nSongVoteResult;	//投票结果

	SongOrderInfo()
	{
		Reset();
	}

	void Reset()
	{
		nOrderID = 0;
		strTransID.Initialize();
		nRoomID	= enmInvalidRoomID;
		nOrderStartTime = 0;
		nOrderEndTime = 0;
		nOrderStatus = enmOrderStatus_Processing;
		strSongName.Initialize();
		nSongerRoleID = enmInvalidRoleID;
		strSongerName.Initialize();
		nRequestRoleID = enmInvalidRoleID;
		strRequestName.Initialize();
		strWishWords.Initialize();
		nOrderCost = 0;
		nSongCount = 0;
		nSongVoteResult = enmSongVoteResult_Giveup;
	}
};

class SongOrderList
{
public:
	SongOrderList()
	{

	}

	virtual ~SongOrderList()
	{

	}

	//初始化玩家管理器
	virtual int32_t Initialize()
	{
		nOrderCount = 0;
		return S_OK;
	}
	//恢复玩家管理器
	virtual int32_t Resume()
	{
		nOrderCount = 0;
		return S_OK;
	}
	//注销玩家管理器
	virtual int32_t Uninitialize()
	{
		nOrderCount = 0;
		return S_OK;
	}

	uint16_t			nOrderCount;
	SongOrderInfo		arrSongOrderList[MaxOrderListLength];
};

typedef CPool<SongOrderList, MaxRoomCountPerServer>		SongOrderListPool;
typedef map<RoomID, SongOrderList *>					RoomSongOrderListMgt;

class CRoomSongOrderListMgt : public CObject
{
public:
	CRoomSongOrderListMgt();

	virtual ~CRoomSongOrderListMgt();
	//初始化玩家管理器
	virtual int32_t Initialize();
	//恢复玩家管理器
	virtual int32_t Resume();
	//注销玩家管理器
	virtual int32_t Uninitialize();

	//创建歌曲订单列表
	SongOrderList *CreateSongOrderList();
	//销毁歌单
	int32_t DestroySongOrderList(RoomID nRoomID);
	//更新房间歌曲订单列表
	int32_t UpdateRoomSongOrderList(RoomID nRoomID, SongOrderList *pList);
	//获取房间歌曲订单列表
	SongOrderList *GetRoomSongOrderList(RoomID nRoomID);
	//获取房间的某个订单信息
	SongOrderInfo *GetSongOrderInfo(RoomID nRoomID, uint32_t nOrderID);
	//生成订单id
	uint32_t MakeOrderID();

protected:
	uint32_t							m_nOrderCounter;
	RoomSongOrderListMgt				m_stRoomSongListMgt;
	SongOrderListPool					m_stSongOrderListPool;
};

#define	CREATE_ROOMSONGORDERLISTMGT_INSTANCE		CSingleton<CRoomSongOrderListMgt>::CreateInstance
#define	GET_ROOMSONGORDERLISTMGT_INSTANCE			CSingleton<CRoomSongOrderListMgt>::GetInstance
#define	DESTROY_ROOMSONGORDERLISTMGT_INSTANCE		CSingleton<CRoomSongOrderListMgt>::DestroyInstance

#define g_RoomSongOrderListMgt		GET_ROOMSONGORDERLISTMGT_INSTANCE()



FRAME_ROOMSERVER_NAMESPACE_END

#endif /* SONG_MGT_H_ */
