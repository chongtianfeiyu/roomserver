/*
 * room_mgt.h
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#ifndef ROOM_MGT_H_
#define ROOM_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "def/server_namespace.h"
#include "def/dal_def.h"
#include "dal/dal_room.h"
#include "public_typedef.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


//定义舞台对象池和索引类型
typedef CPool<CRoom, MaxChannelCountPerServer>	RoomPool;
typedef CHashList<RoomIndex, MaxRoomCountPerServer, MaxRoomCountPerServer * 256>	RoomList;

class CRoomMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_HashListIndex			= 0,
	};
public:
	CRoomMgt();
	virtual ~CRoomMgt();

public:
	//初始化舞台管理器
	virtual int32_t Initialize();
	//恢复舞台管理器
	virtual int32_t Resume();
	//注销舞台管理器
	virtual int32_t Uninitialize();

public:
	//创建舞台对象
	int32_t CreateRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex);
	//创建舞台对象
	int32_t CreateRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex);

	//获取舞台对象
	int32_t GetRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex);
	//获取舞台对象
	int32_t GetRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex);
	//获取舞台对象
	int32_t GetRoomByIndex(const RoomIndex roomIndex, RoomPool::CIndex*& pIndex);
	//获取舞台对象
	int32_t GetRoomByIndex(const RoomIndex roomIndex, CRoom*& pRoom);

	//销毁舞台对象
	int32_t DestroyRoomByIndex(const RoomIndex roomIndex);
	//销毁舞台对象
	int32_t DestroyRoom(RoomPool::CIndex* pIndex);
	//销毁舞台对象
	int32_t DestroyRoom(const RoomID nRoomID);
	//销毁舞台对象
	int32_t RealDestroyRoom(const RoomID nRoomID);

	int32_t GetAllRoom(CRoom *arrRoomObject[], const int32_t arrSize, int32_t &nRoomCount);

	//清空舞台管理器
	void Clear();

	//获取第一个房间
	RoomPool::CIndex* GetFirstRoom();

public:
	//获取对象池容量
	uint32_t GetCapacity() const;
	//获取对象池中对象数量
	uint32_t GetRoomCount() const;
	//对象池是否为空
	bool IsEmpty() const;
	//对象池是否已满
	bool IsFull() const;

protected:
	//根据roomID结算Key
	RoomList::Key MakeKey(const RoomID nRoomID) const;

protected:
	RoomPool				m_roomPool;			//舞台对象池
	RoomList				m_roomList;
};



FRAME_ROOMSERVER_NAMESPACE_END



#endif /* ROOM_MGT_H_ */
