/*
 * bll_timer_program.h
 *
 *  Created on: 2013-3-23
 *      Author: jimm
 */

#ifndef BLL_TIMER_PROGRAM_H_
#define BLL_TIMER_PROGRAM_H_

#include "def/server_namespace.h"
#include "bll/bll_base.h"
#include "frame_typedef.h"
#include "frame_errordef.h"
#include "ctl/song_mgt.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define MaxProgramTimerCount		4096

class CProgramTimer : public CBllBase
{
public:
	CProgramTimer()
	{
		m_nProgramTimerCount = 0;
	}

	virtual ~CProgramTimer()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	int32_t CreateProgramTimer(RoomID nRoomID, uint32_t nOrderID, ProgramStatus nProgramStatus);
	//删除节目定时器
	int32_t DeleteProgramTimer(RoomID nRoomID, uint32_t nOrderID);

private:
	//删除节目定时器信息，没有移除会话的语句
	int32_t DeleteProgramTimerInfo(RoomID nRoomID, uint32_t nOrderID);

	int32_t FightForTicketTimeout(CFrameSession *pSession);

	int32_t EnjoyProgramTimeout(CFrameSession *pSession);

	int32_t VoteingTimeout(CFrameSession *pSession);

	int32_t ShowResultTimeout(CFrameSession *pSession);

	class CProgramSessionData
	{
	public:
		RoomID			nRoomID;
		uint32_t		nOrderID;
		ProgramStatus	nProgramStatus;

		CProgramSessionData()
		{
			nRoomID = enmInvalidRoomID;
			nOrderID = 0;
			nProgramStatus = enmProgramStatus_WaitingForStart;
		}
	};

private:

	class ProgramTimerInfo
	{
	public:
		ProgramTimerInfo()
		{
			nSessionIndex = enmInvalidSessionIndex;
			nRoomID = enmInvalidRoomID;
			nOrderID = 0;
			nProgramStatus = enmProgramStatus_WaitingForStart;
		}

		SessionIndex 	nSessionIndex;
		RoomID			nRoomID;
		ProgramStatus	nProgramStatus;
		uint32_t		nOrderID;
	};

	int32_t						m_nProgramTimerCount;
	ProgramTimerInfo			m_arrProgramTimerInfo[MaxProgramTimerCount];
};

#define	CREATE_PROGRAM_TIMER_INSTANCE		CSingleton<CProgramTimer>::CreateInstance
#define	GET_PROGRAM_TIMER_INSTANCE			CSingleton<CProgramTimer>::GetInstance
#define	DESTROY_PROGRAM_TIMER_INSTANCE		CSingleton<CProgramTimer>::DestroyInstance

#define g_ProgramTimer			GET_PROGRAM_TIMER_INSTANCE()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_TIMER_PROGRAM_H_ */
