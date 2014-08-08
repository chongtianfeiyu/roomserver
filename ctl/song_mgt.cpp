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

//�����赥
SongList *CPlayerSongListMgt::CreateSongList()
{
//	SongListPool::CIndex* pIndex = NULL;
//
//	//��������
//	pIndex = m_stSongListPool.CreateObject();
//	if (NULL == pIndex)
//	{
//		return NULL;
//	}
//
//	return pIndex->ObjectPtr();
	return new SongList;
}

//���ٸ赥
int32_t CPlayerSongListMgt::DestroySongList(RoleID nRoleID)
{
	return S_OK;
}

//������Ҹ赥
int32_t CPlayerSongListMgt::UpdatePlayerSongList(RoleID nRoleID, SongList *pList)
{
	m_stPlayerSongListMgt[nRoleID] = pList;
	return S_OK;
}

//��ȡ��Ҹ赥
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

//��������
SongOrderList *CRoomSongOrderListMgt::CreateSongOrderList()
{
//	SongOrderListPool::CIndex* pIndex = NULL;
//
//	//��������
//	pIndex = m_stSongOrderListPool.CreateObject();
//	if (NULL == pIndex)
//	{
//		return NULL;
//	}
//
//	return pIndex->ObjectPtr();
	return new SongOrderList;
}

//���ٶ���
int32_t CRoomSongOrderListMgt::DestroySongOrderList(RoomID nRoomID)
{
	return S_OK;
}

//���¶���
int32_t CRoomSongOrderListMgt::UpdateRoomSongOrderList(RoomID nRoomID, SongOrderList *pList)
{
	m_stRoomSongListMgt[nRoomID] = pList;
	return S_OK;
}

//��ȡ����
SongOrderList *CRoomSongOrderListMgt::GetRoomSongOrderList(RoomID nRoomID)
{
	if(m_stRoomSongListMgt.count(nRoomID) > 0)
	{
		return m_stRoomSongListMgt[nRoomID];
	}

	return NULL;
}

//��ȡ�����ĳ��������Ϣ
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

//���ɶ���id
uint32_t CRoomSongOrderListMgt::MakeOrderID()
{
	return ++m_nOrderCounter;
}

FRAME_ROOMSERVER_NAMESPACE_END

