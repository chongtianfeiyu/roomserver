/*
 * dal_channel.cpp
 *
 *  Created on: 2011-12-15
 *      Author: jimm
 */

#include "dal_channel.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN


int32_t CChannel::AddRoom(const RoomIndex nRoomIndex, const RoomType nRoomType)
{
	RoomIndexList::Key key = MakeKey(nRoomIndex);
	RoomIndexList::CIndex* pIndex = m_sRoomIndexList.Insert(key, nRoomIndex);
	if (NULL != pIndex)
	{
		++m_nCurRoomCount;
	}

	return S_OK;
}

int32_t CChannel::DeleteRoom(const RoomIndex nRoomIndex)
{
	RoomIndexList::Key key = MakeKey(nRoomIndex);
	int32_t ret = m_sRoomIndexList.Erase(key);
	if (0 <= ret)
	{
		--m_nCurRoomCount;
	}

	return S_OK;
}

bool CChannel::IsRoomInChannel(const RoomIndex nRoomIndex)
{
	RoomIndexList::Key key = MakeKey(nRoomIndex);
	RoomIndexList::CIndex *pIndex = m_sRoomIndexList.Find(key);

	return NULL == pIndex;
}

int32_t CChannel::GetAllRoom(RoomIndex arrRoomIndex[], const int32_t arrSize, int32_t &nRoomCount)
{
	nRoomCount = 0;
	RoomIndexList::CIndex *pIndex = m_sRoomIndexList.First();

	while(NULL != pIndex && nRoomCount < arrSize)
	{
		arrRoomIndex[nRoomCount++] = pIndex->Object();
		pIndex = pIndex->Next();
	}

	return S_OK;
}

RoomIndexList::Key CChannel::MakeKey(const RoomIndex nRoomIndex)
{
	RoomIndexList::Key key = { 0 };
	key.nKeyLength = (uint32_t)sizeof(nRoomIndex);
	*(RoomIndex*)key.arrKey = nRoomIndex;

	return key;
}

FRAME_ROOMSERVER_NAMESPACE_END

