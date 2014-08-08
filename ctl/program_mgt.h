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

//歌曲节目信息
class SongProgramInfo
{
public:
	RoomID					nRoomID;
	uint32_t				nOrderID;
	ProgramStatus			nProgramStatus;							//节目状态
	int64_t					nStatusEndTime;							//此状态结束时间
	int64_t					nShowResultStartTime;					//展示结果状态的开始时间
	uint16_t				nTotalTicketCount;						//本轮总共有效票数(不包括点歌者的票数)
	uint16_t				nHaveTicketPlayerCount;					//拥有投票玩家的个数
	RoleID					arrVoter[MaxVoteTicketsCount];			//投票玩家
	SongVoteResult			arrVoteResult[MaxVoteTicketsCount];		//投票结果
	uint16_t				nGerneralGiveupCount;					//大众评审弃权数
	uint16_t				nGerneralGoodCount;						//大众评审好票数
	uint16_t				nGerneralBadCount;						//大众评审差票数
	RoleID					nRequesterRoleID;						//点歌者
	uint16_t				nRequesterGiveupCount;					//点歌者弃权数
	uint16_t				nRequesterGoodCount;					//点歌者好票数
	uint16_t				nRequesterBadCount;						//点歌者差票数
	uint16_t				nMarkResult;							//评论结果

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

	//是否已经有票了
	bool IsHaveTicket(RoleID nRoleID);
	//是否投过票了
	bool IsVoted(RoleID nRoleID);
	//是否全部投完票了
	bool IsAllVoted();
	//投票操作
	bool Vote(RoleID nRoleID, SongVoteResult nResult, uint8_t nTicketCount);
	//计算结果
	SongVoteResult ComputeResult();
	//返回剩余票数
	uint16_t AddVoter(RoleID nRoleID);
};

typedef map<RoomID, SongProgramInfo *>		RoomSongProgramMgt;

//房间歌曲节目管理器
class CRoomSongProgramMgt
{
public:
	CRoomSongProgramMgt();

	virtual ~CRoomSongProgramMgt();

	//创建歌曲节目信息
	SongProgramInfo *CreateSongProgramInfo();
	//销毁歌曲节目信息
	int32_t DestroySongProgramInfo(RoomID nRoomID);
	//更新歌曲节目信息
	int32_t UpdateSongProgramInfo(RoomID nRoomID, SongProgramInfo *pInfo);
	//获取歌曲节目信息
	SongProgramInfo *GetSongProgramInfo(RoomID nRoomID);
	//获取玩家的票数
	uint16_t GetPlayerTicketCount(RoomID nRoomID, RoleID nRoleID);
	//开始节目
	int32_t StartProgram(RoomID nRoomID, uint32_t nOrderID);
	//结束节目
	int32_t StopProgram(RoomID nRoomID);

protected:
	RoomSongProgramMgt			m_stRoomSongVoteMgt;
};

#define	CREATE_ROOMSONGPROGRAMMGT_INSTANCE		CSingleton<CRoomSongProgramMgt>::CreateInstance
#define	GET_ROOMSONGPROGRAMMGT_INSTANCE			CSingleton<CRoomSongProgramMgt>::GetInstance
#define	DESTROY_ROOMSONGPROGRAMMGT_INSTANCE		CSingleton<CRoomSongProgramMgt>::DestroyInstance

#define g_RoomSongProgramMgt		GET_ROOMSONGPROGRAMMGT_INSTANCE()


//玩家作为超级评委投票信息
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

	uint16_t			nYear;					//年份
	uint16_t			nMonth;					//月份
	uint16_t			nDay;					//日期
	RoleID				nRoleID;
	uint16_t			nSuperVoteCount;		//作为超级评委次数
};

typedef map<RoleID, PlayerVoteInfo *>		PlayerVoteMgt;
typedef map<RoomID, PlayerVoteMgt *>		RoomSuperVoteMgt;

class CRoomSuperVoteMgt
{
public:
	CRoomSuperVoteMgt();

	virtual ~CRoomSuperVoteMgt();

	//创建超级评委投票信息管理器
	PlayerVoteMgt *CreatePlayerVoteMgt();

	PlayerVoteInfo *CreatePlayerVoteInfo();
	//取得玩家的超级投票信息
	PlayerVoteInfo *GetPlayerVoteInfo(RoomID nRoomID, RoleID nRoleID);
	//刷新投票信息
	void FlushPlayerVoteInfo(PlayerVoteInfo *pPlayerVoteInfo);
	//更新玩家的投票次数(作为超级评委)
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
