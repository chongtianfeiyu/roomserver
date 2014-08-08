/*
 * song_mgt.h
 *
 *  Created on: 2013-3-7
 *      Author: jimm
 */

#ifndef SONG_MGT_H_
#define SONG_MGT_H_

#include <map>
#include "public_typedef.h"
#include "common/common_object.h"
#include "common/common_pool.h"
#include "def/server_namespace.h"
#include "common/common_singleton.h"
#include "common/common_string.h"
#include "def/dal_def.h"

using namespace std;

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class SongInfo
{
public:
	CString<MaxSongNameLength>		strSongName;
	uint32_t		nSongAttr;

	SongInfo()
	{
		Reset();
	}

	void Reset()
	{
		strSongName.Initialize();
		nSongAttr = 0;
	}
};

class SongList : public CObject
{
public:
	SongList()
	{

	}

	virtual ~SongList()
	{

	}

	//��ʼ����ҹ�����
	virtual int32_t Initialize()
	{
		m_nSongCount = 0;
		return S_OK;
	}
	//�ָ���ҹ�����
	virtual int32_t Resume()
	{
		m_nSongCount = 0;
		return S_OK;
	}
	//ע����ҹ�����
	virtual int32_t Uninitialize()
	{
		m_nSongCount = 0;
		return S_OK;
	}

	uint16_t							m_nSongCount;							//��ǰ������Ŀ
	SongInfo							m_arrSongList[MaxSongCountPerPlayer];	//�����б�
};

typedef CPool<SongList, MaxSongListMgtSize>		SongListPool;
typedef map<RoleID, SongList *>					PlayerSongListMgt;

class CPlayerSongListMgt : public CObject
{
public:
	CPlayerSongListMgt();

	virtual ~CPlayerSongListMgt();
	//��ʼ����ҹ�����
	virtual int32_t Initialize();
	//�ָ���ҹ�����
	virtual int32_t Resume();
	//ע����ҹ�����
	virtual int32_t Uninitialize();

	//�����赥
	SongList *CreateSongList();
	//���ٸ赥
	int32_t DestroySongList(RoleID nRoleID);
	//������Ҹ赥
	int32_t UpdatePlayerSongList(RoleID nRoleID, SongList *pList);
	//��ȡ��Ҹ赥
	SongList *GetPlayerSongList(RoleID nRoleID);

protected:
	PlayerSongListMgt				m_stPlayerSongListMgt;
	SongListPool					m_stSongListPool;
};

#define	CREATE_PLAYERSONGLISTMGT_INSTANCE		CSingleton<CPlayerSongListMgt>::CreateInstance
#define	GET_PLAYERSONGLISTMGT_INSTANCE			CSingleton<CPlayerSongListMgt>::GetInstance
#define	DESTROY_PLAYERSONGLISTMGT_INSTANCE		CSingleton<CPlayerSongListMgt>::DestroyInstance

#define g_PlayerSongListMgt		GET_PLAYERSONGLISTMGT_INSTANCE()

class SongOrderInfo
{
public:
	uint32_t					nOrderID;
	CString<enmMaxSerialNumberLength>	strTransID;
	RoomID						nRoomID;			//�����
	int64_t						nOrderStartTime;	//��������ʱ��
	int64_t						nOrderEndTime;		//��������ʱ��
	OrderStatus					nOrderStatus;		//����״̬
	CString<MaxSongNameLength>	strSongName;		//��ĸ�����
	RoleID						nSongerRoleID;		//����roleid
	CString<MaxRoleNameLength>	strSongerName;		//�����ǳ�
	RoleID						nRequestRoleID;		//�����roleid
	CString<MaxRoleNameLength>	strRequestName;		//�������ǳ�
	CString<MaxWishWordsLength>	strWishWords;		//ף����
	uint32_t					nOrderCost;			//������
	uint16_t					nSongCount;			//���˼��׸�
	SongVoteResult				nSongVoteResult;	//ͶƱ���

	SongOrderInfo()
	{
		Reset();
	}

	void Reset()
	{
		nOrderID = 0;
		strTransID.Initialize();
		nRoomID	= enmInvalidRoomID;
		nOrderStartTime = 0;
		nOrderEndTime = 0;
		nOrderStatus = enmOrderStatus_Processing;
		strSongName.Initialize();
		nSongerRoleID = enmInvalidRoleID;
		strSongerName.Initialize();
		nRequestRoleID = enmInvalidRoleID;
		strRequestName.Initialize();
		strWishWords.Initialize();
		nOrderCost = 0;
		nSongCount = 0;
		nSongVoteResult = enmSongVoteResult_Giveup;
	}
};

class SongOrderList
{
public:
	SongOrderList()
	{

	}

	virtual ~SongOrderList()
	{

	}

	//��ʼ����ҹ�����
	virtual int32_t Initialize()
	{
		nOrderCount = 0;
		return S_OK;
	}
	//�ָ���ҹ�����
	virtual int32_t Resume()
	{
		nOrderCount = 0;
		return S_OK;
	}
	//ע����ҹ�����
	virtual int32_t Uninitialize()
	{
		nOrderCount = 0;
		return S_OK;
	}

	uint16_t			nOrderCount;
	SongOrderInfo		arrSongOrderList[MaxOrderListLength];
};

typedef CPool<SongOrderList, MaxRoomCountPerServer>		SongOrderListPool;
typedef map<RoomID, SongOrderList *>					RoomSongOrderListMgt;

class CRoomSongOrderListMgt : public CObject
{
public:
	CRoomSongOrderListMgt();

	virtual ~CRoomSongOrderListMgt();
	//��ʼ����ҹ�����
	virtual int32_t Initialize();
	//�ָ���ҹ�����
	virtual int32_t Resume();
	//ע����ҹ�����
	virtual int32_t Uninitialize();

	//�������������б�
	SongOrderList *CreateSongOrderList();
	//���ٸ赥
	int32_t DestroySongOrderList(RoomID nRoomID);
	//���·�����������б�
	int32_t UpdateRoomSongOrderList(RoomID nRoomID, SongOrderList *pList);
	//��ȡ������������б�
	SongOrderList *GetRoomSongOrderList(RoomID nRoomID);
	//��ȡ�����ĳ��������Ϣ
	SongOrderInfo *GetSongOrderInfo(RoomID nRoomID, uint32_t nOrderID);
	//���ɶ���id
	uint32_t MakeOrderID();

protected:
	uint32_t							m_nOrderCounter;
	RoomSongOrderListMgt				m_stRoomSongListMgt;
	SongOrderListPool					m_stSongOrderListPool;
};

#define	CREATE_ROOMSONGORDERLISTMGT_INSTANCE		CSingleton<CRoomSongOrderListMgt>::CreateInstance
#define	GET_ROOMSONGORDERLISTMGT_INSTANCE			CSingleton<CRoomSongOrderListMgt>::GetInstance
#define	DESTROY_ROOMSONGORDERLISTMGT_INSTANCE		CSingleton<CRoomSongOrderListMgt>::DestroyInstance

#define g_RoomSongOrderListMgt		GET_ROOMSONGORDERLISTMGT_INSTANCE()



FRAME_ROOMSERVER_NAMESPACE_END

#endif /* SONG_MGT_H_ */
