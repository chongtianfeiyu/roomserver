/*
 *  bll_event_getroominfo.cpp
 *
 *  To do：
 *  Created on: 	2012-2-9
 *  Author: 		luocj
 */

#include "bll_event_getroominfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CGetRoomInfoEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CGetRoomInfoEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CGetRoomInfoEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_GET_ROOM_REQ:
		OnEventGetRoomInfo(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CGetRoomInfoEvent::OnEventGetRoomInfo(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("rcev player get room info req{nRoleID=%d, nRoomID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有此玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleID=%d}",pMsgHead->nRoleID);
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
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//发送回应道客户端
	SendResponseToUser(pMsgHead,pRoom,nOptionLen,pOptionData);

	return ret;
}

/*
 * 发送响应到客户端
 */
int32_t CGetRoomInfoEvent::SendResponseToUser(MessageHeadSS *pMsgHead,CRoom *pRoom,const uint16_t nOptionLen , const void *pOptionData)
{
	CGetRoomInfoRespToClient stCGetRoomInfoResp;
	stCGetRoomInfoResp.nStringCount = 0;
	stCGetRoomInfoResp.nGetCount = 0;
	for(uint8_t i = 0;i < enmRoomInfoType_Count;i++)
	{
		switch (i)
		{
			case enmRoomInfoType_NAME:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_NAME;
				stCGetRoomInfoResp.arrSetString[stCGetRoomInfoResp.nStringCount] = pRoom->GetRoomName();
				stCGetRoomInfoResp.nStringCount++;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_SIGNATURE:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_SIGNATURE;
				stCGetRoomInfoResp.arrSetString[stCGetRoomInfoResp.nStringCount] = pRoom->GetRoomSignature();
				stCGetRoomInfoResp.nStringCount++;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_PASSWORD:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_PASSWORD;
				stCGetRoomInfoResp.arrSetString[stCGetRoomInfoResp.nStringCount] = pRoom->GetRoomPassWord();
				stCGetRoomInfoResp.nStringCount++;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_FREESHOW_TIME:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_FREESHOW_TIME;
				stCGetRoomInfoResp.nMicTime = pRoom->GetMicroTime()/60;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_WELCOME_WORDS:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_WELCOME_WORDS;
				stCGetRoomInfoResp.arrSetString[stCGetRoomInfoResp.nStringCount] = pRoom->GetRoomWelcome();
				stCGetRoomInfoResp.nStringCount++;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_NOTICE:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_NOTICE;
				stCGetRoomInfoResp.arrSetString[stCGetRoomInfoResp.nStringCount] = pRoom->GetRoomNotice();
				stCGetRoomInfoResp.nStringCount++;
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_OPTION:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_OPTION;
				stCGetRoomInfoResp.nRoomOptionType = pRoom->GetRoomOption();
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_Mic_First_Status:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_Mic_First_Status;
				stCGetRoomInfoResp.nFirstMicStatus = pRoom->GetMicStatusByMicIndex(1);
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_Mic_Second_Status:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_Mic_Second_Status;
				stCGetRoomInfoResp.nSecondMicStatus = pRoom->GetMicStatusByMicIndex(2);
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			case enmRoomInfoType_Mic_Third_Status:
			{
				stCGetRoomInfoResp.arrRoomInfoType[stCGetRoomInfoResp.nGetCount] = enmRoomInfoType_Mic_Third_Status;
				stCGetRoomInfoResp.nThirdMicStatus = pRoom->GetMicStatusByMicIndex(3);
				stCGetRoomInfoResp.nGetCount++;
			}
			break;
			default:
			break;
		}
	}
	SendMessageResponse(MSGID_RSCL_GET_ROOM_RESP, pMsgHead, &stCGetRoomInfoResp, enmTransType_P2P,nOptionLen,(char*)pOptionData);
	return S_OK;
}


FRAME_ROOMSERVER_NAMESPACE_END
