/*
 * server_datacenter.h
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#ifndef SERVER_DATACENTER_H_
#define SERVER_DATACENTER_H_

#include "common/common_object.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/lightframe_impl.h"
#include "public_typedef.h"

#include "player_mgt.h"
#include "room_mgt.h"
#include "channel_mgt.h"

#include "frame_protocolhead.h"
#include <map>
using namespace std;

FRAME_ROOMSERVER_NAMESPACE_BEGIN

typedef map<RoleID, ConnInfo *> CConnInfoMgt;

class MediaServerInfo
{
public:
	MediaServerInfo()
	{
		m_nServerID = enmInvalidServerID;
		m_nAddress = 0;
		m_nPort = 0;
	}
	ServerID	m_nServerID;
	uint32_t 	m_nAddress;
	uint16_t	m_nPort;
};

class CServerDataCenter : public IDataCenter
{
public:
	CServerDataCenter();
	virtual ~CServerDataCenter();

public:
	virtual const char* GetName();

	//��Frame�ṩ�Լ������ڴ��С�Ĳ�ѯ�ӿ�
	virtual size_t GetSize();

	//��ȡ�����ڴ�Key(����0��ʾ��Frame����)
	virtual int32_t GetKey(){return 0;}

	//��ʼ���ӿڵ��õ�ʱ�� �����ڴ��ѷ���
	virtual int32_t Initialize();
	virtual int32_t Resume();
	virtual int32_t Uninitialize();

public:
	//��ȡ��ҹ���������
	CPlayerMgt& GetPlayerMgt()
	{
		return *m_pPlayerMgt;
	}

	CConnInfoMgt &GetConnInfoMgt()
	{
		return m_stConnInfoMgt;
	}

	CRoomMgt& GetRoomMgt()
	{
		return *m_pRoomMgt;
	}

	CChannelMgt& GetChannelMgt()
	{
		return *m_pChannelMgt;
	}

	int32_t GetAllPlayers(const RoomID nRoomID, CPlayer *arrPlayer[],
			const int32_t arrSize, int32_t &nPlayerCount);

	int32_t GetAllPlayers(CRoom *pRoom, CPlayer *arrPlayer[],
			const int32_t arrSize, int32_t &nPlayerCount);

	int32_t GetAllPlayers(CRoom *pRoom, RoleID arrRoleID[],
			const int32_t arrSize, int32_t &nPlayerCount);

	void RegistMediaServerInfo(MediaServerInfo stMediaServerInfo);

	MediaServerInfo GetMediaServerInfo(ServerID nServerID);

	ConnInfo *GetConnInfo(RoleID nRoleID);

protected:

	CPlayerMgt				*m_pPlayerMgt;								//��ҹ���������
	CRoomMgt				*m_pRoomMgt;								//�������������
	CChannelMgt				*m_pChannelMgt;								//Ƶ������������
	CConnInfoMgt			m_stConnInfoMgt;

	int32_t					m_nMediaServerCount;
	MediaServerInfo			m_arrMediaServerInfo[MaxMediaCount];
};


#define	CREATE_DALDATACENTER_INSTANCE		CSingleton<CServerDataCenter>::CreateInstance
#define	GET_DALDATACENTER_INSTANCE			CSingleton<CServerDataCenter>::GetInstance
#define	DESTROY_DALDATACENTER_INSTANCE		CSingleton<CServerDataCenter>::DestroyInstance

#define g_DataCenter		GET_DALDATACENTER_INSTANCE()
#define g_PlayerMgt			GET_DALDATACENTER_INSTANCE().GetPlayerMgt()
#define g_ConnInfoMgt		GET_DALDATACENTER_INSTANCE().GetConnInfoMgt()
#define g_RoomMgt			GET_DALDATACENTER_INSTANCE().GetRoomMgt()
#define g_ChannelMgt		GET_DALDATACENTER_INSTANCE().GetChannelMgt()


FRAME_ROOMSERVER_NAMESPACE_END

#endif /* SERVER_DATACENTER_H_ */
