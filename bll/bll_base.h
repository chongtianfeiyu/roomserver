/*
 * bll_base.h
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#ifndef BLL_BASE_H_
#define BLL_BASE_H_

#include "main_frame.h"
#include "public_typedef.h"
#include "dal/to_client_message.h"
#include "dal/to_server_message.h"
#include "dal/dal_player.h"
#include "dal/dal_room.h"
#include "../../lightframe/frame_typedef.h"
#include "../../lightframe/frame_errordef.h"
#include "roomdispatcher_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CBllBase  : public IMessageEventSS, public ISessionEvent
{
public:
	CBllBase()
	{

	}
	virtual ~CBllBase()
	{

	}
public:
/*
消息事件接口
*/
	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{return S_OK;};

public:
/*
会话事件接口
*/
	/*
		会话事件激活
	*/
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL)
	{return S_OK;};
	/*
		会话超时
	*/
	virtual int32_t OnSessionTimeOut(CFrameSession *pSession) {return S_OK;};

public:
	/*
		发送请求消息
	*/
	void SendMessageRequest(uint32_t nMsgID, IMsgBody* pMsgBody, const RoomID nRoomID, TransType nTransType,
			RoleID nRoleID, EntityType nDestType, const int32_t nDestID, const SessionIndex nSessionIndex,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL,
			const char *szDumpContent="send request message");
	/*
		发送相应消息
	*/
	void SendMessageResponse(uint32_t nMsgID,MessageHeadSS * pMsgHead,IMsgBody* pMsgBody,
			TransType nTransType=enmTransType_P2P,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL,
			const char *szDumpContent="send response message");

	void SendMessageNotifyToServer(uint32_t nMsgID,IMsgBody* pMsgBody,const RoomID nRoomID,
			const TransType transType,const RoleID nRoleID,
			const EntityType destType,const int32_t nDestID = 0,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL,
			const char *szDumpContent="send notify message");

	//发送广播消息给房间内某些玩家(一个或者部分或者所有)
	void SendMessageNotifyToClient(uint32_t nMsgID, IMsgBody* pMsgBody, RoomID nRoomID, BroadcastType nType,
			RoleID nRoleID, const uint16_t nOptionLen = 0,const char *pOptionData = NULL,
			const char *szDumpContent="send notify message");

	int32_t SendEvent(const RoleID roleID, uint32_t nMsgID, IMsgBody* pMsgBody);

	int32_t ExitShow(CPlayer *pPlayer,CRoom *pRoom,bool sendNotifyToClient,int32_t nMsgID,const uint16_t nOptionLen = 0, const void *pOptionData = NULL,ExitShowType nExitShowType = enmExitShowType_Other);
	int32_t ExitP2PShow(CPlayer *pSrcPlayer,CRoom *pRoom,bool sendNotifyToClient,const uint16_t nOptionLen = 0, const void *pOptionData = NULL);
	int32_t WaittingToPublic(CRoom *pRoom);

	int32_t ExitRoom(CPlayer *pPlayer,PlayerIndex nPlayerIndex,CRoom *pRoom,int32_t nMsgID,bool sendNotifyToClient = false,bool isNeedKickRobot = true);

	int32_t KickRobot(CRoom *pRoom,uint32_t nKickCount = 1);

	int32_t StaticInRoomTime(CPlayer *pPlayer,CRoom *pRoom);

	int32_t StaticOnMIcTime(CPlayer *pPlayer,CRoom *pRoom);

	/*
	 * 判断玩家的链接信息是否合法
	 */
	bool ConnInfoIsOK(CPlayer *pPlayer,const uint16_t nOptionLen, const void *pOptionData);

	//vip等级从大到小排列。
	void SortPlayerByVipLevel(CPlayer *arrPlayer[],int32_t nPlayerCount )
	{
      int32_t i,j;
      CPlayer *pPlayerTemp = NULL;
      for(i=1;i<nPlayerCount;i++)
      {
    	  pPlayerTemp = arrPlayer[i];
    	  if(pPlayerTemp==NULL)
    	  {
    		  continue;
    	  }
    	  for(j = i-1;j>=0&&(arrPlayer[j]->GetVipLevel()<pPlayerTemp->GetVipLevel());j--)
    	  {
    		  arrPlayer[j+1] = arrPlayer[j];
    	  }
    	  arrPlayer[j+1] = pPlayerTemp;
      }
	}
	//管理身份从大到小排列。
	void SortPlayerByAdminLevel(CPlayer *arrPlayer[],int32_t nPlayerCount,RoomID nRoomID)
	{
		int32_t i,j;
		CPlayer *pPlayerTemp = NULL;
		for(i=1;i<nPlayerCount;i++)
		{
			pPlayerTemp = arrPlayer[i];
			for(j = i-1;j>=0&&(arrPlayer[j]->GetRoleRankInRoom(nRoomID)<pPlayerTemp->GetRoleRankInRoom(nRoomID));j--)
			{
				arrPlayer[j+1] = arrPlayer[j];
			}
			arrPlayer[j+1] = pPlayerTemp;
		}
	}

	//由于是新用户，或者是消费大户，给玩家发送彩条
	void SendColorToPalyer(CRoom *pRoom,CPlayer *pPlayer);

	int32_t ReBulidMediaServer(CSetMediaInfoNoti* pSetMediaInfoNoti);

	int32_t SendPlayerOnMicToMedia(CRoom *pRoom,CPlayer *pPlayer)
	{
		if(pRoom == NULL || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("null pointer:{pPlayer=0x%08x, pRoom=0x%08x}\n",pPlayer,pRoom);
			return E_NULLPOINTER;
		}
		//发送请求者的转发添加通知(自己加自己，先当与上麦通知)
		RoleID arrReciverRoleID[1] = {pPlayer->GetRoleID()};
		SendAddTransmitUserNotifyToMediaServer(pPlayer->GetRoleID(), arrReciverRoleID, 1, pRoom, true);

//		CAddTransmitUserNotice stP2PAddTransmitUserNotice;
//		stP2PAddTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//		stP2PAddTransmitUserNotice.nRoleID = pPlayer->GetRoleID();
//		stP2PAddTransmitUserNotice.nAddCount = 1;
//		stP2PAddTransmitUserNotice.arrAddRoleID[0] = pPlayer->GetRoleID();
//		stP2PAddTransmitUserNotice.nRoomID = pRoom->GetRoomID();
//		//发送请求者的转发添加通知(自己加自己，先当与上麦通知)
//		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stP2PAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_P2P,pPlayer->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Tel));
//		SendMessageNotifyToServer(MSGID_RSMS_ADD_TRANSMITUSER_NOTI,&stP2PAddTransmitUserNotice,pRoom->GetRoomID(),enmTransType_P2P,pPlayer->GetRoleID(),enmEntityType_Media,pRoom->GetMediaServerIDByType(enmNetType_Cnc));
		return S_OK;
	}

	int32_t SendPlayerMicStatusToDB(CRoom *pRoom,CPlayer *pPlayer,MicOperateType nMicOperateType,int32_t nMsgID,int32_t nResult)
	{
		if(pRoom == NULL || pPlayer == NULL)
		{
			WRITE_ERROR_LOG("null pointer:{pPlayer=0x%08x, pRoom=0x%08x}\n",pPlayer,pRoom);
			return E_NULLPOINTER;
		}

		CDateTime dt = CDateTime::CurrentDateTime();
		int32_t dateline = dt.Year() * 10000 + dt.Month() * 100 + dt.Day();

		CStaPlayerMicStatus stStaPlayerMicStatus;
		stStaPlayerMicStatus.nRoomID = pRoom->GetRoomID();
		stStaPlayerMicStatus.nRoleID = pPlayer->GetRoleID();
		stStaPlayerMicStatus.strRoleName = pPlayer->GetRoleName();
		stStaPlayerMicStatus.nMicOperateType = nMicOperateType;
		stStaPlayerMicStatus.nMsgID = nMsgID;
		stStaPlayerMicStatus.nMicResult = nResult;
		stStaPlayerMicStatus.nIdentityType = pPlayer->GetIdentityType();
		stStaPlayerMicStatus.nTime = dateline;


		SendMessageNotifyToServer(MSGID_RSDB_STA_PLAYER_MIC_STATUS,&stStaPlayerMicStatus,pRoom->GetRoomID(),enmTransType_ByKey,pPlayer->GetRoleID(),enmEntityType_DBProxy);

		return S_OK;
	}

public:
	void DumpMessage(const char* szContent, MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const char *pOptionData = NULL);

	//向Media发送上麦通知
	void SendAddTransmitUserNotifyToMediaServer(RoleID nSpeakerID, RoleID arrReciverID[], int32_t nReciverCount, CRoom *pRoom, bool bIsPublicMic);

	//向Media发送上麦通知
	void SendAddTransmitUserNotifyToMediaServer(CPlayer* pSpeaker, CPlayer* arrReciver[], int32_t nReciverCount, CRoom *pRoom, bool bIsPublicMic);

	//向Media发送下麦麦通知
	void SendDelTransmitUserNotifyToMediaServer(RoleID nSpeakerID, CRoom* pRoom, RoleID arrReciverID[], int32_t nReciverCount);
	void SendDelTransmitUserNotifyToMediaServer(CPlayer* pSpeaker, CRoom* pRoom, CPlayer* arrReciver[], int32_t nReciverCount);

//	void SendPublicAddTransmitNotifyWhenEnterRoom()
//	{
//
//	}
//	void SpeakerExitRoomSendNotify(CPlayer* pPlayer, CRoom* pRoom);

private:
	int32_t SendMessage(MessageHeadSS* pMessageHead, IMsgBody *pMsgBody, const int32_t nOptionLen = 0,
			const char *pOptionData = NULL, int32_t index=0, int32_t zoneid=0);
	int32_t WaittingListToPublicMic(CRoom *pRoom);
	int32_t UpdateRoomDataInfo();
	int32_t UpdatePlayerDataInfo();
private:
		uint8_t					m_arrProtocolBuf[32];
};


#define	CREATE_BLLBASE_INSTANCE		CSingleton<CBllBase>::CreateInstance
#define	GET_BLLBASE_INSTANCE		CSingleton<CBllBase>::GetInstance
#define	DESTROY_BLLBASE_INSTANCE	CSingleton<CBllBase>::DestroyInstance

#define g_BllBase GET_BLLBASE_INSTANCE()



FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_BASE_H_ */
