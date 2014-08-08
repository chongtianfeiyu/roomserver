/*
 * room_mgt.cpp
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#include "def/server_errordef.h"
#include "room_mgt.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


CRoomMgt::CRoomMgt()
{

}

CRoomMgt::~CRoomMgt()
{

}

//初始化舞台管理器
int32_t CRoomMgt::Initialize()
{
	m_roomList.Initialize();
	m_roomPool.Initialize();

	return S_OK;
}

//恢复舞台管理器
int32_t CRoomMgt::Resume()
{
	m_roomPool.Resume();
	m_roomPool.Resume();

	return S_OK;
}

//注销舞台管理器
int32_t CRoomMgt::Uninitialize()
{
	m_roomPool.Uninitialize();
	m_roomPool.Uninitialize();

	return S_OK;
}

//创建舞台对象
int32_t CRoomMgt::CreateRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex)
{
	pIndex = m_roomPool.CreateObject();
	if (NULL == pIndex)
	{
		return E_RS_CREATEROOM;
	}

	//建立索引
	RoomList::Key key = MakeKey(nRoomID);
	RoomList::CIndex* pListIndex = m_roomList.Insert(key, pIndex->Index());
	if (NULL == pListIndex)
	{
		m_roomPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_RS_CREATEROOMINDEX;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_HashListIndex, (uint32_t)pListIndex->Index());
	if (0 > ret)
	{
		m_roomList.Erase(pListIndex);
		m_roomPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	return S_OK;
}

//创建舞台对象
int32_t CRoomMgt::CreateRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex)
{
	RoomPool::CIndex* pIndex = NULL;

	int32_t ret = CreateRoom(nRoomID, pIndex);
	if (ret < 0 || NULL == pIndex)
	{
		return E_RS_CREATEROOM;
	}

	pRoom = pIndex->ObjectPtr();
	roomIndex = pIndex->Index();

	pRoom->SetRoomID(nRoomID);
	return S_OK;
}

//获取舞台对象
int32_t CRoomMgt::GetRoom(const RoomID nRoomID, RoomPool::CIndex*& pIndex)
{
	//根据索引查找
	RoomList::Key key = MakeKey(nRoomID);
	RoomList::CIndex* pListIndex = m_roomList.Find(key);
	if (NULL == pListIndex)
	{
		return E_RS_ROOMNOTFOUND;
	}

	RoomIndex& roomIndex = pListIndex->GetObject();
	pIndex = m_roomPool.GetIndex(roomIndex);
	if (NULL == pIndex)
	{
		return E_RS_ROOMNOTFOUND;
	}

	return S_OK;
}

//获取舞台对象
int32_t CRoomMgt::GetRoom(const RoomID nRoomID, CRoom*& pRoom, RoomIndex& roomIndex)
{
	RoomPool::CIndex* pIndex = NULL;

	int32_t ret = GetRoom(nRoomID, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pRoom = pIndex->ObjectPtr();
	roomIndex = pIndex->Index();

	return S_OK;
}

//获取舞台对象
int32_t CRoomMgt::GetRoomByIndex(const RoomIndex roomIndex, RoomPool::CIndex*& pIndex)
{
	pIndex = m_roomPool.GetIndex(roomIndex);
	if (NULL == pIndex)
	{
		return E_RS_ROOMNOTFOUND;
	}

	return S_OK;
}

//获取舞台对象
int32_t CRoomMgt::GetRoomByIndex(const RoomIndex roomIndex, CRoom*& pRoom)
{
	RoomPool::CIndex* pIndex = m_roomPool.GetIndex(roomIndex);
	if (NULL == pIndex)
	{
		return E_RS_ROOMNOTFOUND;
	}

	pRoom = pIndex->ObjectPtr();

	return S_OK;
}

//销毁舞台对象
int32_t CRoomMgt::DestroyRoomByIndex(const RoomIndex roomIndex)
{
	RoomPool::CIndex* pIndex = m_roomPool.GetIndex(roomIndex);
	if (NULL == pIndex)
	{
		return E_RS_ROOMNOTEXIST;
	}

	return m_roomPool.DestroyObject(pIndex);
}

//销毁舞台对象
int32_t CRoomMgt::DestroyRoom(RoomPool::CIndex* pIndex)
{
	return m_roomPool.DestroyObject(pIndex);
}

//销毁舞台对象
int32_t CRoomMgt::DestroyRoom(const RoomID nRoomID)
{
	return S_OK;
//	//根据索引查找
//	RoomList::Key key = MakeKey(nRoomID);
//	RoomList::CIndex* pListIndex = m_roomList.Find(key);
//	if (NULL == pListIndex)
//	{
//		return E_RS_ROOMNOTEXIST;
//	}
//
//	RoomIndex roomIndex = pListIndex->GetObject();
//
//	//将玩家从对象池及索引表中删除
//	m_roomList.Erase(pListIndex);
//	m_roomPool.DestroyObject(roomIndex);
//
//	return S_OK;
}


//销毁舞台对象
int32_t CRoomMgt::RealDestroyRoom(const RoomID nRoomID)
{
	//根据索引查找
	RoomList::Key key = MakeKey(nRoomID);
	RoomList::CIndex* pListIndex = m_roomList.Find(key);
	if (NULL == pListIndex)
	{
		return E_RS_ROOMNOTEXIST;
	}

	RoomIndex roomIndex = pListIndex->GetObject();

	//将玩家从对象池及索引表中删除
	m_roomList.Erase(pListIndex);
	m_roomPool.DestroyObject(roomIndex);

	return S_OK;
}

int32_t CRoomMgt::GetAllRoom(CRoom *arrRoomObject[], const int32_t arrSize, int32_t &nRoomObjectCount)
{
	nRoomObjectCount = 0;
	RoomPool::CIndex *pIndex = m_roomPool.First();

	while(NULL != pIndex && nRoomObjectCount < arrSize)
	{
		arrRoomObject[nRoomObjectCount++] = pIndex->ObjectPtr();
		pIndex = pIndex->Next();
	}

	return S_OK;
}

//清空舞台管理器
void CRoomMgt::Clear()
{
	m_roomPool.Clear();
}

//获取第一个房间
RoomPool::CIndex* CRoomMgt::GetFirstRoom()
{
	return m_roomPool.First();
}

//获取对象池容量
uint32_t CRoomMgt::GetCapacity() const
{
	return m_roomPool.Capacity();
}

//获取对象池中对象数量
uint32_t CRoomMgt::GetRoomCount() const
{
	return m_roomPool.ObjectCount();
}

//对象池是否为空
bool CRoomMgt::IsEmpty() const
{
	return m_roomPool.IsEmpty();
}

//对象池是否已满
bool CRoomMgt::IsFull() const
{
	return m_roomPool.IsFull();
}

//根据roomID结算Key
RoomList::Key CRoomMgt::MakeKey(const RoomID nRoomID) const
{
	RoomList::Key key = { 0 };
	key.nKeyLength = (uint32_t)sizeof(RoomID);
	*(RoomID*)key.arrKey = nRoomID;

	return key;
}

FRAME_ROOMSERVER_NAMESPACE_END

