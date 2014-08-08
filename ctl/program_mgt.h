/*
 * program_mgt.h
 *
 *  Created on: 2013-3-22
 *      Author: jimm
 */

#ifndef PROGRAM_MGT_H_
#define PROGRAM_MGT_H_

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

//������Ŀ��Ϣ
class SongProgramInfo
{
public:
	RoomID					nRoomID;
	uint32_t				nOrderID;
	ProgramStatus			nProgramStatus;							//��Ŀ״̬
	int64_t					nStatusEndTime;							//��״̬����ʱ��
	int64_t					nShowResultStartTime;					//չʾ���״̬�Ŀ�ʼʱ��
	uint16_t				nTotalTicketCount;						//�����ܹ���ЧƱ��(����������ߵ�Ʊ��)
	uint16_t				nHaveTicketPlayerCount;					//ӵ��ͶƱ��ҵĸ���
	RoleID					arrVoter[MaxVoteTicketsCount];			//ͶƱ���
	SongVoteResult			arrVoteResult[MaxVoteTicketsCount];		//ͶƱ���
	uint16_t				nGerneralGiveupCount;					//����������Ȩ��
	uint16_t				nGerneralGoodCount;						//���������Ʊ��
	uint16_t				nGerneralBadCount;						//���������Ʊ��
	RoleID					nRequesterRoleID;						//�����
	uint16_t				nRequesterGiveupCount;					//�������Ȩ��
	uint16_t				nRequesterGoodCount;					//����ߺ�Ʊ��
	uint16_t				nRequesterBadCount;						//����߲�Ʊ��
	uint16_t				nMarkResult;							//���۽��

	SongProgramInfo()
	{
		Reset();
	}

	void Reset()
	{
		nRoomID = enmInvalidRoomID;
		nOrderID = 0;
		nProgramStatus = enmProgramStatus_WaitingForStart;
		nStatusEndTime = 0;
		nShowResultStartTime = 0;
		nTotalTicketCount = 0;
		nHaveTicketPlayerCount = 0;
		for(int32_t i = 0; i < MaxVoteTicketsCount; ++i)
		{
			arrVoter[i] = enmInvalidRoleID;
			arrVoteResult[i] = enmSongVoteResult_NotVote;
		}
		nGerneralGiveupCount = 0;
		nGerneralGoodCount = 0;
		nGerneralBadCount = 0;
		nRequesterRoleID = enmInvalidRoleID;
		nRequesterGiveupCount = 0;
		nRequesterGoodCount = 0;
		nRequesterBadCount = 0;
		nMarkResult = 0;
	}

	//�Ƿ��Ѿ���Ʊ��
	bool IsHaveTicket(RoleID nRoleID);
	//�Ƿ�Ͷ��Ʊ��
	bool IsVoted(RoleID nRoleID);
	//�Ƿ�ȫ��Ͷ��Ʊ��
	bool IsAllVoted();
	//ͶƱ����
	bool Vote(RoleID nRoleID, SongVoteResult nResult, uint8_t nTicketCount);
	//������
	SongVoteResult ComputeResult();
	//����ʣ��Ʊ��
	uint16_t AddVoter(RoleID nRoleID);
};

typedef map<RoomID, SongProgramInfo *>		RoomSongProgramMgt;

//���������Ŀ������
class CRoomSongProgramMgt
{
public:
	CRoomSongProgramMgt();

	virtual ~CRoomSongProgramMgt();

	//����������Ŀ��Ϣ
	SongProgramInfo *CreateSongProgramInfo();
	//���ٸ�����Ŀ��Ϣ
	int32_t DestroySongProgramInfo(RoomID nRoomID);
	//���¸�����Ŀ��Ϣ
	int32_t UpdateSongProgramInfo(RoomID nRoomID, SongProgramInfo *pInfo);
	//��ȡ������Ŀ��Ϣ
	SongProgramInfo *GetSongProgramInfo(RoomID nRoomID);
	//��ȡ��ҵ�Ʊ��
	uint16_t GetPlayerTicketCount(RoomID nRoomID, RoleID nRoleID);
	//��ʼ��Ŀ
	int32_t StartProgram(RoomID nRoomID, uint32_t nOrderID);
	//������Ŀ
	int32_t StopProgram(RoomID nRoomID);

protected:
	RoomSongProgramMgt			m_stRoomSongVoteMgt;
};

#define	CREATE_ROOMSONGPROGRAMMGT_INSTANCE		CSingleton<CRoomSongProgramMgt>::CreateInstance
#define	GET_ROOMSONGPROGRAMMGT_INSTANCE			CSingleton<CRoomSongProgramMgt>::GetInstance
#define	DESTROY_ROOMSONGPROGRAMMGT_INSTANCE		CSingleton<CRoomSongProgramMgt>::DestroyInstance

#define g_RoomSongProgramMgt		GET_ROOMSONGPROGRAMMGT_INSTANCE()


//�����Ϊ������ίͶƱ��Ϣ
class PlayerVoteInfo
{
public:
	PlayerVoteInfo()
	{
		nYear = 0;
		nMonth = 0;
		nDay = 0;
		nRoleID = enmInvalidRoleID;
		nSuperVoteCount = 0;
	}

	uint16_t			nYear;					//���
	uint16_t			nMonth;					//�·�
	uint16_t			nDay;					//����
	RoleID				nRoleID;
	uint16_t			nSuperVoteCount;		//��Ϊ������ί����
};

typedef map<RoleID, PlayerVoteInfo *>		PlayerVoteMgt;
typedef map<RoomID, PlayerVoteMgt *>		RoomSuperVoteMgt;

class CRoomSuperVoteMgt
{
public:
	CRoomSuperVoteMgt();

	virtual ~CRoomSuperVoteMgt();

	//����������ίͶƱ��Ϣ������
	PlayerVoteMgt *CreatePlayerVoteMgt();

	PlayerVoteInfo *CreatePlayerVoteInfo();
	//ȡ����ҵĳ���ͶƱ��Ϣ
	PlayerVoteInfo *GetPlayerVoteInfo(RoomID nRoomID, RoleID nRoleID);
	//ˢ��ͶƱ��Ϣ
	void FlushPlayerVoteInfo(PlayerVoteInfo *pPlayerVoteInfo);
	//������ҵ�ͶƱ����(��Ϊ������ί)
	uint16_t UpdatePlayerVoteInfo(RoomID nRoomID, RoleID nRoleID);

	int32_t ClearAllVoteInfo();

	int32_t ClearRoomPlayerVoteInfo(RoomID nRoomID);

protected:
	RoomSuperVoteMgt			m_stRoomSuperVoteMgt;
};

#define	CREATE_ROOMSUPERVOTEMGT_INSTANCE		CSingleton<CRoomSuperVoteMgt>::CreateInstance
#define	GET_ROOMSUPERVOTEMGT_INSTANCE			CSingleton<CRoomSuperVoteMgt>::GetInstance
#define	DESTROY_ROOMSUPERVOTEMGT_INSTANCE		CSingleton<CRoomSuperVoteMgt>::DestroyInstance

#define g_RoomSuperVoteMgt		GET_ROOMSUPERVOTEMGT_INSTANCE()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* PROGRAM_MGT_H_ */
