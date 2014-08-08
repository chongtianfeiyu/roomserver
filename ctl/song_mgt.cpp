/*
 * song_mgt.cpp
 *
 *  Created on: 2013-3-7
 *      Author: jimm
 */

#include "frame_errordef.h"
#include "def/server_errordef.h"
#include "song_mgt.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

CPlayerSongListMgt::CPlayerSongListMgt()
{
}

CPlayerSongListMgt::~CPlayerSongListMgt()
{

}

int32_t CPlayerSongListMgt::Initialize()
{
	m_stSongListPool.Initialize();
	return S_OK;
}

int32_t CPlayerSongListMgt::Resume()
{
	m_stSongListPool.Resume();
	return S_OK;
}

int32_t CPlayerSongListMgt::Uninitialize()
{
	m_stSongListPool.Uninitialize();
	return S_OK;
}

//创建歌单
SongList *CPlayerSongListMgt::CreateSongList()
{
//	SongListPool::CIndex* pIndex = NULL;
//
//	//创建对象
//	pIndex = m_stSongListPool.CreateObject();
//	if (NULL == pIndex)
//	{
//		return NULL;
//	}
//
//	return pIndex->ObjectPtr();
	return new SongList;
}

//销毁歌单
int32_t CPlayerSongListMgt::DestroySongList(RoleID nRoleID)
{
	return S_OK;
}

//更新玩家歌单
int32_t CPlayerSongListMgt::UpdatePlayerSongList(RoleID nRoleID, SongList *pList)
{
	m_stPlayerSongListMgt[nRoleID] = pList;
	return S_OK;
}

//获取玩家歌单
SongList *CPlayerSongListMgt::GetPlayerSongList(RoleID nRoleID)
{
	if(m_stPlayerSongListMgt.count(nRoleID) > 0)
	{
		return m_stPlayerSongListMgt[nRoleID];
	}

	return NULL;
}

CRoomSongOrderListMgt::CRoomSongOrderListMgt()
{
	m_nOrderCounter = 0;
}

CRoomSongOrderListMgt::~CRoomSongOrderListMgt()
{

}

int32_t CRoomSongOrderListMgt::Initialize()
{
	m_stSongOrderListPool.Initialize();
	return S_OK;
}

int32_t CRoomSongOrderListMgt::Resume()
{
	m_stSongOrderListPool.Resume();
	return S_OK;
}

int32_t CRoomSongOrderListMgt::Uninitialize()
{
	m_stSongOrderListPool.Uninitialize();
	return S_OK;
}

//创建订单
SongOrderList *CRoomSongOrderListMgt::CreateSongOrderList()
{
//	SongOrderListPool::CIndex* pIndex = NULL;
//
//	//创建对象
//	pIndex = m_stSongOrderListPool.CreateObject();
//	if (NULL == pIndex)
//	{
//		return NULL;
//	}
//
//	return pIndex->ObjectPtr();
	return new SongOrderList;
}

//销毁订单
int32_t CRoomSongOrderListMgt::DestroySongOrderList(RoomID nRoomID)
{
	return S_OK;
}

//更新订单
int32_t CRoomSongOrderListMgt::UpdateRoomSongOrderList(RoomID nRoomID, SongOrderList *pList)
{
	m_stRoomSongListMgt[nRoomID] = pList;
	return S_OK;
}

//获取订单
SongOrderList *CRoomSongOrderListMgt::GetRoomSongOrderList(RoomID nRoomID)
{
	if(m_stRoomSongListMgt.count(nRoomID) > 0)
	{
		return m_stRoomSongListMgt[nRoomID];
	}

	return NULL;
}

//获取房间的某个订单信息
SongOrderInfo *CRoomSongOrderListMgt::GetSongOrderInfo(RoomID nRoomID, uint32_t nOrderID)
{
	SongOrderList *pOrderList = GetRoomSongOrderList(nRoomID);
	if(pOrderList == NULL)
	{
		return NULL;
	}

	for(int32_t i = 0; i < pOrderList->nOrderCount; ++i)
	{
		if(pOrderList->arrSongOrderList[i].nOrderID == nOrderID)
		{
			return &pOrderList->arrSongOrderList[i];
		}
	}

	return NULL;
}

//生成订单id
uint32_t CRoomSongOrderListMgt::MakeOrderID()
{
	return ++m_nOrderCounter;
}

FRAME_ROOMSERVER_NAMESPACE_END

