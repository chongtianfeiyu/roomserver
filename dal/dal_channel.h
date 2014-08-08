/*
 * dal_channel.h
 *
 *  Created on: 2011-12-15
 *      Author: jimm
 */

#ifndef DAL_CHANNEL_H_
#define DAL_CHANNEL_H_


#include "common/common_complextype.h"
#include "common/common_object.h"
#include "def/server_namespace.h"
#include "public_typedef.h"
#include "dal_room.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

typedef CHashList<RoomIndex, MaxRoomCountPerChannel, ChannelBucketSize>	RoomIndexList;

class CChannel : public CObject
{
public:
	CChannel()
	{
		Reset();
	}

	~CChannel()
	{

	}

	virtual int32_t Initialize()
	{
		Reset();
		return m_sRoomIndexList.Initialize();
	}

	virtual int32_t Resume()
	{
		Reset();
		return m_sRoomIndexList.Resume();
	}

	virtual int32_t Uninitialize()
	{
		Reset();
		return m_sRoomIndexList.Uninitialize();
	}

	int32_t AddRoom(const RoomIndex nRoomIndex, const RoomType nRoomType);

	int32_t DeleteRoom(const RoomIndex nRoomIndex);

	bool IsRoomInChannel(const RoomIndex nRoomIndex);

	int32_t GetAllRoom(RoomIndex arrRoomIndex[], const int32_t arrSize, int32_t &nRoomCount);

	RegionType GetRegionType() const
	{
		return m_nRegionType;
	}

	void SetRegionType(const RegionType nRegionType)
	{
		m_nRegionType = nRegionType;
	}

	RegionID GetRegionID() const
	{
		return m_nRegionID;
	}

	void SetRegionID(const RegionID nRegionID)
	{
		m_nRegionID = nRegionID;
	}

	ChannelType GetChannelType() const
	{
		return m_nChannelType;
	}

	void SetChannelType(const ChannelType nChannelType)
	{
		m_nChannelType = nChannelType;
	}

	ChannelID GetChannelID() const
	{
		return m_nChannelID;
	}

	void SetChannelID(const ChannelID nChannelID)
	{
		m_nChannelID = nChannelID;
	}

	int32_t GetRoomCount() const
	{
		return m_nCurRoomCount;
	}

protected:
	RoomIndexList::Key MakeKey(const RoomIndex nRoomIndex);

	void Reset()
	{
		m_nRegionType = enmInvalidRegionType;
		m_nRegionID = enmInvalidRegionID;
		m_nChannelType = enmInvalidChannelType;
		m_nChannelID = enmInvalidChannelID;
		m_nCurRoomCount = 0;
	}

private:
	RegionType						m_nRegionType;			//大区类型
	RegionID						m_nRegionID;			//大区号
	ChannelType						m_nChannelType;			//频道类型
	ChannelID						m_nChannelID;			//频道号
	CString<MaxChannelNameLength>	m_strChannelName;		//频道名称
	int32_t							m_nCurRoomCount;		//当前房间数
	RoomIndexList					m_sRoomIndexList;		//房间对象池
};


FRAME_ROOMSERVER_NAMESPACE_END

CCOMPLEXTYPE_SPECIFICALLY(FRAME_ROOMSERVER_NAMESPACE::CChannel)

#endif /* DAL_CHANNEL_H_ */
