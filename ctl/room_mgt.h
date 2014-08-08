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


//������̨����غ���������
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
	//��ʼ����̨������
	virtual int32_t Initialize();
	//�ָ���̨������
	virtual int32_t Resume();
	//ע����̨������
	virtual int32_t Uninitialize();

public:
	//������̨����
	int32_t CreateRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex);
	//������̨����
	int32_t CreateRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex);

	//��ȡ��̨����
	int32_t GetRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex);
	//��ȡ��̨����
	int32_t GetRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex);
	//��ȡ��̨����
	int32_t GetRoomByIndex(const RoomIndex roomIndex, RoomPool::CIndex*& pIndex);
	//��ȡ��̨����
	int32_t GetRoomByIndex(const RoomIndex roomIndex, CRoom*& pRoom);

	//������̨����
	int32_t DestroyRoomByIndex(const RoomIndex roomIndex);
	//������̨����
	int32_t DestroyRoom(RoomPool::CIndex* pIndex);
	//������̨����
	int32_t DestroyRoom(const RoomID nRoomID);
	//������̨����
	int32_t RealDestroyRoom(const RoomID nRoomID);

	int32_t GetAllRoom(CRoom *arrRoomObject[], const int32_t arrSize, int32_t &nRoomCount);

	//�����̨������
	void Clear();

	//��ȡ��һ������
	RoomPool::CIndex* GetFirstRoom();

public:
	//��ȡ���������
	uint32_t GetCapacity() const;
	//��ȡ������ж�������
	uint32_t GetRoomCount() const;
	//������Ƿ�Ϊ��
	bool IsEmpty() const;
	//������Ƿ�����
	bool IsFull() const;

protected:
	//����roomID����Key
	RoomList::Key MakeKey(const RoomID nRoomID) const;

protected:
	RoomPool				m_roomPool;			//��̨�����
	RoomList				m_roomList;
};



FRAME_ROOMSERVER_NAMESPACE_END



#endif /* ROOM_MGT_H_ */
