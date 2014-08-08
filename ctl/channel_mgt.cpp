/*
 * channel_mgt.cpp
 *
 *  Created on: 2011-12-16
 *      Author: jimm
 */

#include "def/server_errordef.h"
#include "channel_mgt.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN


CChannelMgt::CChannelMgt()
{

}

CChannelMgt::~CChannelMgt()
{

}

//初始化蹈房管理器
int32_t CChannelMgt::Initialize()
{
	m_channelPool.Initialize();
	m_channelList.Initialize();
	return S_OK;
}

//恢复蹈房管理器
int32_t CChannelMgt::Resume()
{
	m_channelPool.Resume();
	m_channelList.Resume();
	return S_OK;
}

//注销频道管理器
int32_t CChannelMgt::Uninitialize()
{
	m_channelPool.Uninitialize();
	m_channelList.Uninitialize();
	return S_OK;
}

//创建频道对象
int32_t CChannelMgt::CreateChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex)
{
	//创建对象
	pIndex = m_channelPool.CreateObject();
	if (NULL == pIndex)
	{
		return E_RS_CREATECHANNEL;
	}

	//建立索引
	ChannelList::Key key = MakeKey(channelID);
	ChannelList::CIndex* pListIndex = m_channelList.Insert(key, pIndex->Index());
	if (NULL == pListIndex)
	{
		m_channelPool.DestroyObject(pIndex);
		pIndex = NULL;
		return E_RS_CREATECHANNELINDEX;
	}

	//将索引保存到附加数据表中
	int32_t ret = pIndex->SetAdditionalData(enmAdditionalIndex_HashListIndex, (uint32_t)pListIndex->Index());
	if (0 > ret)
	{
		m_channelList.Erase(pListIndex);
		m_channelPool.DestroyObject(pIndex);
		pIndex = NULL;
		return ret;
	}

	return S_OK;
}

//创建频道对象
int32_t CChannelMgt::CreateChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex)
{
	ChannelPool::CIndex* pIndex = NULL;

	int32_t ret = CreateChannel(channelID, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pChannel = pIndex->ObjectPtr();
	channelIndex = pIndex->Index();

	return S_OK;
}

//获取频道对象
int32_t CChannelMgt::GetChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex)
{
	//根据索引查找
	ChannelList::Key key = MakeKey(channelID);
	ChannelList::CIndex* pListIndex = m_channelList.Find(key);
	if (NULL == pListIndex)
	{
		return E_RS_CHANNELNOTFOUND;
	}

	ChannelIndex& channelIndex = pListIndex->GetObject();
	pIndex = m_channelPool.GetIndex(channelIndex);
	if (NULL == pIndex)
	{
		return E_RS_CHANNELNOTFOUND;
	}

	return S_OK;
}

//获取频道对象
int32_t CChannelMgt::GetChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex)
{
	ChannelPool::CIndex* pIndex = NULL;

	int32_t ret = GetChannel(channelID, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pChannel = pIndex->ObjectPtr();
	channelIndex = pIndex->Index();

	return S_OK;
}

//获取频道对象
int32_t CChannelMgt::GetChannelByIndex(const ChannelIndex channelIndex, ChannelPool::CIndex*& pIndex)
{
	pIndex = m_channelPool.GetIndex(channelIndex);
	if (NULL == pIndex)
	{
		return E_RS_CHANNELNOTFOUND;
	}

	return S_OK;
}

//获取频道对象
int32_t CChannelMgt::GetChannelByIndex(const ChannelIndex channelIndex, CChannel*& pChannel)
{
	ChannelPool::CIndex* pIndex = NULL;

	int32_t ret = GetChannelByIndex(channelIndex, pIndex);
	if (0 > ret)
	{
		return ret;
	}

	pChannel = pIndex->ObjectPtr();

	return S_OK;
}

//获取所有频道
void CChannelMgt::GetAllChannels(int32_t& nChannelCount, CChannel* arrChannel[])
{
	nChannelCount = 0;

	ChannelList::CIndex* pIndex = m_channelList.First();
	while (NULL != pIndex)
	{
		GetChannelByIndex(pIndex->Object(), arrChannel[nChannelCount++]);
		pIndex = pIndex->Next();
	}
}

//销毁频道对象
int32_t CChannelMgt::DestroyChannel(const ChannelID channelID)
{
	//根据索引查找
	ChannelList::Key key = MakeKey(channelID);
	ChannelList::CIndex* pListIndex = m_channelList.Find(key);
	if (NULL == pListIndex)
	{
		return E_RS_CHANNELNOTEXIST;
	}

	ChannelIndex channelIndex = pListIndex->GetObject();

	//将频道从对象池及索引表中删除
	m_channelList.Erase(pListIndex);
	m_channelPool.DestroyObject(channelIndex);

	return S_OK;
}

//销毁频道对象
int32_t CChannelMgt::DestroyChannelByIndex(const ChannelIndex channelIndex)
{
	ChannelPool::CIndex* pIndex = m_channelPool.GetIndex(channelIndex);
	if (NULL == pIndex)
	{
		return E_RS_CHANNELNOTEXIST;
	}

	return DestroyChannel(pIndex);
}

//销毁频道对象
int32_t CChannelMgt::DestroyChannel(ChannelPool::CIndex* pIndex)
{
	if (NULL == pIndex)
	{
		return E_INVALIDARGUMENT;
	}

	uint32_t nAdditionalValue = 0;
	pIndex->GetAdditionalData(enmAdditionalIndex_HashListIndex, nAdditionalValue);
	ChannelList::CIndex* pListIndex = m_channelList.GetIndex((int32_t)nAdditionalValue);

	//将频道从对象池及索引表中删除
	m_channelList.Erase(pListIndex);
	m_channelPool.DestroyObject(pIndex);

	return S_OK;
}

//清空频道管理器
int32_t CChannelMgt::Clear()
{
	m_channelPool.Clear();
	m_channelList.Clear();
	return S_OK;
}

//获取第一个
ChannelPool::CIndex* CChannelMgt::GetFirstChannel()
{
	return m_channelPool.First();
}

//获取对象池容量
uint32_t CChannelMgt::GetCapacity() const
{
	return m_channelPool.Capacity();
}

//获取对象池中对象数量
uint32_t CChannelMgt::GetChannelCount() const
{
	return m_channelPool.ObjectCount();
}

//对象池是否为空
bool CChannelMgt::IsEmpty() const
{
	return m_channelPool.IsEmpty();
}

//对象池是否已满
bool CChannelMgt::IsFull() const
{
	return m_channelPool.IsFull();
}

//根据角色ID结算Key
ChannelList::Key CChannelMgt::MakeKey(const ChannelID channelID) const
{
	ChannelList::Key key = { 0 };
	key.nKeyLength = (uint32_t)sizeof(ChannelID);
	*(ChannelID*)key.arrKey = channelID;

	return key;
}


FRAME_ROOMSERVER_NAMESPACE_END


