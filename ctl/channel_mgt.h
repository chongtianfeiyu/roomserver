/*
 * channel_mgt.h
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#ifndef CHANNEL_MGT_H_
#define CHANNEL_MGT_H_

#include "common/common_object.h"
#include "common/common_pool.h"
#include "common/common_hashlist.h"
#include "def/server_namespace.h"
#include "def/dal_def.h"
#include "dal/dal_channel.h"
#include "public_typedef.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


//定义频道对象池和索引类型
typedef CPool<CChannel, MaxChannelCountPerServer> ChannelPool;
typedef CHashList<ChannelIndex, MaxChannelCountPerServer, MaxChannelCountPerServer * 64> ChannelList;


class CChannelMgt : public CObject
{
protected:
	enum
	{
		enmAdditionalIndex_HashListIndex			= 0,
	};
public:
	CChannelMgt();
	virtual ~CChannelMgt();

public:
	//初始化频道管理器
	virtual int32_t Initialize();
	//恢复频道管理器
	virtual int32_t Resume();
	//注销频道管理器
	virtual int32_t Uninitialize();

public:
	//创建频道对象
	int32_t CreateChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex);
	//创建频道对象
	int32_t CreateChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex);

	//获取频道对象
	int32_t GetChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex);
	//获取频道对象
	int32_t GetChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex);

	//获取频道对象
	int32_t GetChannelByIndex(const ChannelIndex channelIndex, ChannelPool::CIndex*& pIndex);
	//获取频道对象
	int32_t GetChannelByIndex(const ChannelIndex channelIndex, CChannel*& pChannel);

	//获取所有频道
	void GetAllChannels(int32_t& nChannelCount, CChannel* arrChannel[]);

	//销毁频道对象
	int32_t DestroyChannel(const ChannelID channelID);
	//销毁频道对象
	int32_t DestroyChannelByIndex(const ChannelIndex channelIndex);
	//销毁频道对象
	int32_t DestroyChannel(ChannelPool::CIndex* pIndex);

	//清空频道管理器
	int32_t Clear();

	//获取第一个
	ChannelPool::CIndex* GetFirstChannel();

public:
	//获取对象池容量
	uint32_t GetCapacity() const;
	//获取对象池中对象数量
	uint32_t GetChannelCount() const;
	//对象池是否为空
	bool IsEmpty() const;
	//对象池是否已满
	bool IsFull() const;

protected:
	//根据角色ID结算Key
	ChannelList::Key MakeKey(const ChannelID channelID) const;

protected:
	ChannelPool				m_channelPool;				//频道对象池
	ChannelList				m_channelList;			//频道对象索引

};



FRAME_ROOMSERVER_NAMESPACE_END

#endif /* CHANNEL_MGT_H_ */
