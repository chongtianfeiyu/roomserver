/*
 *  bll_event_add_black.cpp
 *
 *  To do：
 *  Created on: 	2012-1-10
 *  Author: 		luocj
 */

#include "bll_event_add_black.h"
#include "dal/to_server_message.h"
#include "dal/from_server_message.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CAddBlockEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x, pSession=0x%08x}\n",pMsgBody,pMsgHead,pSession);
		return E_NULLPOINTER;
	}

	int32_t ret = S_OK;
	CAddBlackSessionData * pData =(CAddBlackSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("recv add black to DB response!{nRoleID=%d, nRoomID=%d} ",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room filed!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	//获取玩家对象
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pData->nDestRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("get player filed!!{ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CAddPlayerToBlackResp *pAddPlayerToBlackResp = dynamic_cast<CAddPlayerToBlackResp *>(pMsgBody);
	if(NULL == pAddPlayerToBlackResp)
	{
		WRITE_ERROR_LOG("pMsgBody transform to class child failed{ret=0x%08x}\n",E_NULLPOINTER);
		SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
		return E_NULLPOINTER;
	}

	if(pAddPlayerToBlackResp->nResult != S_OK)
	{
		WRITE_ERROR_LOG("get item info failed ret = 0x%8X!\n",pAddPlayerToBlackResp->nResult);
		SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
		return pAddPlayerToBlackResp->nResult;
	}
	//其他操作:从房间队列删除、加入被踢队列
	if(!(pPlayer->IsReboot()))
	{
		ret = pRoom->AddBlackList(pPlayer->GetRoleID());
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because  add to blacklist error!{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
			SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
			return ret;
		}
	}
	//发送通知和回应
	SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_OK,pData->m_nOptionLen,pData->m_arrOptionData);
	SendNotifyToUser(pData->m_sMsgHead.nRoomID,pData->m_sMsgHead.nRoleID,pPlayer->GetRoleID(),pData->m_nOptionLen,pData->m_arrOptionData);
	//下麦
	ret = ExitRoom(pPlayer,nPlayerIndex,pRoom,pData->m_sMsgHead.nMessageID);
	if(ret<0)
	{
		WRITE_ERROR_LOG("add player to black filed because exit room filed!{nRoleID=%d, nRoomID=%d}",pPlayer->GetRoleID(),pRoom->GetRoomID());
		return ret;
	}
	return S_OK;
}
int32_t CAddBlockEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSession=0x%08x}\n",pSession);
		return E_NULLPOINTER;
	}
	CAddBlackSessionData * pData =(CAddBlackSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("null pointer:{pData=0x%08x}\n",pData);
		return E_NULLPOINTER;
	}
	WRITE_DEBUG_LOG("add black time out {nRoleID=%d}",pData->m_sMsgHead.nRoleID);
	SendResponseToUser(&pData->m_sMsgHead,enmAddBlackResult_Umknown,pData->m_nOptionLen,pData->m_arrOptionData);
	return S_OK;
}
int32_t CAddBlockEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_ADD_BLACK_REQ:
		OnEventAddBlock(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CAddBlockEvent::OnEventAddBlock(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CAddBlackReq *pAddBlackReq = dynamic_cast<CAddBlackReq *>(pMsgBody);
	if(pAddBlackReq==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pAddBlackReq=0x%08x}\n",pAddBlackReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player add user to black req!{nSrcRoleID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pAddBlackReq->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist!{nSrcRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{ret=0x%08x} \n",ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does not in room{nSrcRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	//判断被拉黑人的状态
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pAddBlackReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//没有此玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("dest player does no exist{nDestRoleID=%d}",pAddBlackReq->nDestRoleID);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex))&&!(pRoom->IsRebotPlayerInRoom(pAddBlackReq->nDestRoleID)))
	{
		WRITE_ERROR_LOG("player does not in room{nDestRoleID=%d, nRoomID=%d}",pAddBlackReq->nDestRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}

	RoleRank nSrcRoleRank = pRoom->GetRoleRank(pMsgHead->nRoleID);
	//室主以上可以拉黑
	if(nSrcRoleRank<enmRoleRank_Host)
	{
		// todo 发送权限不够回应
		SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission,nOptionLen,pOptionData);
		return E_RS_NOPERMISSIONS;
	}
    if(pRoom->IsInBlackList(pAddBlackReq->nDestRoleID))
    {
    	WRITE_DEBUG_LOG("has error!");
    	SendResponseToUser(pMsgHead,enmAddBlackResult_OK,nOptionLen,pOptionData);
    	SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pAddBlackReq->nDestRoleID,nOptionLen,pOptionData);
    	return S_OK;
    }
    bool isOffice = pPlayer->IsOfficialPlayer();
    if(!isOffice)
    {
    	//加入黑名单的操作
		ret = AddPlayerToBlack(pRoom,pAddBlackReq->nDestRoleID);
		if(ret == (int32_t)E_RS_NOPERMISSIONS)
		{
			// todo 发送权限不够回应
			SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission,nOptionLen,pOptionData);
			return ret;
		}
		else if(ret == (int32_t)E_RS_NOPERMISSIONS_KING)
		{
			// todo 发送权限不够回应
			SendResponseToUser(pMsgHead,enmAddBlackResult_NO_Permission_King,nOptionLen,pOptionData);
			return ret;
		}
		else if(ret<0)
		{
			//todo 发送错误回应
			SendResponseToUser(pMsgHead,enmAddBlackResult_Umknown,nOptionLen,pOptionData);
			return ret;
		}
    }
	//如果是机器人，不操作数据库
	if(pDestPlayer->IsReboot())
	{
		//发送通知和回应
		SendResponseToUser(pMsgHead,enmAddBlackResult_OK,nOptionLen,pOptionData);
		SendNotifyToUser(pMsgHead->nRoomID,pMsgHead->nRoleID,pAddBlackReq->nDestRoleID,nOptionLen,pOptionData);
		//退房
		ret = ExitRoom(pDestPlayer,nDestPlayerIndex,pRoom,pMsgHead->nMessageID);
		if(ret<0)
		{
			WRITE_ERROR_LOG("add player to black filed because exit room filed{nDestRoleID=%d, nRoomID=%d}",pDestPlayer->GetRoleID(),pMsgHead->nRoomID);
			return ret;
		}
		return S_OK;
	}
	//发送添加到数据库请求
	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_DBRS_ADD_BLACK_RESP, enmSessionType_AddToBlack, this, enmAddBlackTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("create add black session is failed!{RoleID=%d}\n", pMsgHead->nRoleID);
		//todo 发送错误回应
		SendResponseToUser(pMsgHead,enmAddBlackResult_Umknown,nOptionLen,pOptionData);

		return ret;
	}

	CAddBlackSessionData * pData = new (pSession->GetSessionData())CAddBlackSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	//此时的可选字段里面包含着玩家的连接信息ConnUin
	if(nOptionLen > 0)
	{
		pData->m_nOptionLen = nOptionLen;
		memcpy(pData->m_arrOptionData, pOptionData, nOptionLen);
	}
	pData->nDestRoleID = pAddBlackReq->nDestRoleID;
	//发送请求
	CAddPlayerToBlackReq stAddPlayerToBlackReq;
	//为0是表示获取房间里所有人的
	stAddPlayerToBlackReq.nRoleID = pAddBlackReq->nDestRoleID;
	stAddPlayerToBlackReq.nRoomID = pMsgHead->nRoomID;

	SendMessageRequest(MSGID_RSDB_ADD_BLACK_REQ, &stAddPlayerToBlackReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send add black request");
	WRITE_NOTICE_LOG("player add user to balck!{nSrcRoleID=%d, nDestRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pAddBlackReq->nDestRoleID);
	return ret;
}
/*
 *  把玩家加入黑名单
 */
int32_t CAddBlockEvent::AddPlayerToBlack(CRoom *pRoom,RoleID nAddBlockRoleID)
{
	int32_t ret = S_OK;
	if(NULL==pRoom)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pMsgHead=0x%08x}\n",pRoom);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(nAddBlockRoleID, pPlayer, nPlayerIndex);
	//没有被 加黑名单玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("dest player does no exist{nAddBlockRoleID=%d}",nAddBlockRoleID);
		return ret;
	}
	//判断被加黑名单玩家的RoleRank
	RoleRank nRoleRank = pRoom->GetRoleRank(nAddBlockRoleID);
	if(nRoleRank>=enmRoleRank_Admin)
	{
		WRITE_ERROR_LOG("add player to black filed because no permissions{nAddBlockRoleID=%d, nRoleRank=%d}",nAddBlockRoleID,nRoleRank);
		return E_RS_NOPERMISSIONS;
	}
	//银色皇冠以上用户不让拉黑
	if(pPlayer->GetVipLevel()>=enmVipLevel_Silver_KING)
	{
		WRITE_ERROR_LOG("add player to black filed because no permissions to king!{nAddBlockRoleID=%d, nVipLevel=0x%02X}",nAddBlockRoleID,pPlayer->GetVipLevel());
		return E_RS_NOPERMISSIONS_KING;
	}
	return ret;
}

/*
 * 发送通知给房间里的玩家
 */
int32_t CAddBlockEvent::SendNotifyToUser(RoomID nRoomID,RoleID nSrcRoleID,RoleID nDestRoleID,const uint16_t nOptionLen , const void *pOptionData )
{
	CAddBlackNoti stAddBlackNoti;
	stAddBlackNoti.nSrcRoleID = nSrcRoleID;
	stAddBlackNoti.nDestRoleID = nDestRoleID;

	//发送给除发起踢人的其他人
	SendMessageNotifyToClient(MSGID_RSCL_ADD_BLACK_NOTI, &stAddBlackNoti, nRoomID, enmBroadcastType_ExceptPlayr,
			nSrcRoleID,nOptionLen,(char*)pOptionData, "send add user to black notify");
	return S_OK;
}
/*
 * 发送响应到客户端
 */
int32_t CAddBlockEvent::SendResponseToUser(MessageHeadSS *pMsgHead,AddBlackResult nAddBlackResult,const uint16_t nOptionLen , const void *pOptionData)
{
	CAddBlackResp stAddBlackResp;
	stAddBlackResp.nAddBlackResult = nAddBlackResult;
	SendMessageResponse(MSGID_RSCL_ADD_BLACK_RESP, pMsgHead, &stAddBlackResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
