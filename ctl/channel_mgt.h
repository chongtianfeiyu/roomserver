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


//����Ƶ������غ���������
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
	//��ʼ��Ƶ��������
	virtual int32_t Initialize();
	//�ָ�Ƶ��������
	virtual int32_t Resume();
	//ע��Ƶ��������
	virtual int32_t Uninitialize();

public:
	//����Ƶ������
	int32_t CreateChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex);
	//����Ƶ������
	int32_t CreateChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex);

	//��ȡƵ������
	int32_t GetChannel(const ChannelID channelID, ChannelPool::CIndex*& pIndex);
	//��ȡƵ������
	int32_t GetChannel(const ChannelID channelID, CChannel*& pChannel, ChannelIndex& channelIndex);

	//��ȡƵ������
	int32_t GetChannelByIndex(const ChannelIndex channelIndex, ChannelPool::CIndex*& pIndex);
	//��ȡƵ������
	int32_t GetChannelByIndex(const ChannelIndex channelIndex, CChannel*& pChannel);

	//��ȡ����Ƶ��
	void GetAllChannels(int32_t& nChannelCount, CChannel* arrChannel[]);

	//����Ƶ������
	int32_t DestroyChannel(const ChannelID channelID);
	//����Ƶ������
	int32_t DestroyChannelByIndex(const ChannelIndex channelIndex);
	//����Ƶ������
	int32_t DestroyChannel(ChannelPool::CIndex* pIndex);

	//���Ƶ��������
	int32_t Clear();

	//��ȡ��һ��
	ChannelPool::CIndex* GetFirstChannel();

public:
	//��ȡ���������
	uint32_t GetCapacity() const;
	//��ȡ������ж�������
	uint32_t GetChannelCount() const;
	//������Ƿ�Ϊ��
	bool IsEmpty() const;
	//������Ƿ�����
	bool IsFull() const;

protected:
	//���ݽ�ɫID����Key
	ChannelList::Key MakeKey(const ChannelID channelID) const;

protected:
	ChannelPool				m_channelPool;				//Ƶ�������
	ChannelList				m_channelList;			//Ƶ����������

};



FRAME_ROOMSERVER_NAMESPACE_END

#endif /* CHANNEL_MGT_H_ */
