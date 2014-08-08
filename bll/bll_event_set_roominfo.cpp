/*
 *  bll_event_set_room.cpp
 *
 *  To do：
 *  Created on: 	2012-2-7
 *  Author: 		luocj
 */

#include "bll_event_set_roominfo.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CSetRoomInfoEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL || pSession == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x, pSession=0x%08x}\n",pMsgHead,pMsgBody,pSession);
		return E_NULLPOINTER;
	}
	CSetRoomSessionData * pData =(CSetRoomSessionData *)(pSession->GetSessionData());
	CSetRoomAttrResp *pSetRoomAttrResp = dynamic_cast<CSetRoomAttrResp *>(pMsgBody);
	if(NULL == pSetRoomAttrResp || NULL == pData)
	{
		WRITE_ERROR_LOG("null pointer:{pSetRoomAttrResp=0x%08x, pData=0x%08x}\n",pSetRoomAttrResp,pData);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("set room_info:recv set room info resp from DB{nRoleID=%d, nRoomID=%d}\n",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	CSetRoomResp stSetRoomResp;
	//写数据库失败
	if(pSetRoomAttrResp->nResult < 0)
	{
		WRITE_ERROR_LOG("set room_info:set DB filed{ret=0x%08x}\n",pSetRoomAttrResp->nResult);
		stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, &pData->m_sMsgHead, &stSetRoomResp,
					enmTransType_P2P);
		return pSetRoomAttrResp->nResult;
	}
	CSetRoomReq *pSetRoomReq = &pData->m_pMsgBody;
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pData->m_sMsgHead.nRoomID, pRoom, nRoomIndex);
	if(pRoom == NULL || pSetRoomReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pRoom=0x%08x, pSetRoomReq=0x%08x}\n",pRoom,pSetRoomReq);
		stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, &pData->m_sMsgHead, &stSetRoomResp,
					enmTransType_P2P);
		return E_NULLPOINTER;
	}
	//todo 相关操作
	uint8_t nStringIndex = 0;
	CSetRoomNotify stSetRoomNotify;
	stSetRoomNotify.nSetCount = 0;
	stSetRoomNotify.nStringCount = 0;
	for(int32_t i = 0;i < pSetRoomReq->nSetCount;i++)
	{
		switch(pSetRoomReq->arrRoomInfoType[i])
		{
			case enmRoomInfoType_NAME:
			{
				WRITE_DEBUG_LOG(" set room name do not has notify  \n");
//				pRoom->SetRoomName(pSetRoomReq->arrSetString[nStringIndex].GetString());
				nStringIndex++;
			}
			break;
			case enmRoomInfoType_SIGNATURE:
			{
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_SIGNATURE;
				stSetRoomNotify.arrSetString[stSetRoomNotify.nStringCount] = pSetRoomReq->arrSetString[nStringIndex].GetString();
				stSetRoomNotify.nStringCount++;
				stSetRoomNotify.nSetCount++;
				pRoom->SetRoomSignature(pSetRoomReq->arrSetString[nStringIndex].GetString());
				nStringIndex++;
			}
			break;
			case enmRoomInfoType_PASSWORD:
			{
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_PASSWORD;
				stSetRoomNotify.nSetCount++;
				stSetRoomNotify.arrSetString[stSetRoomNotify.nStringCount] = pSetRoomReq->arrSetString[nStringIndex].GetString();
				stSetRoomNotify.nStringCount++;
				pRoom->SetRoomPassWord(pSetRoomReq->arrSetString[nStringIndex].GetString());
				nStringIndex++;
			}
			break;
			case enmRoomInfoType_FREESHOW_TIME:
			{
				//客户端传来的是分钟计算的
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_FREESHOW_TIME;
				stSetRoomNotify.nSetCount++;
				pRoom->SetMicroTime(pSetRoomReq->nMicTime*60);
			}
			break;
			case enmRoomInfoType_WELCOME_WORDS:
			{
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_WELCOME_WORDS;
				stSetRoomNotify.nSetCount++;
				stSetRoomNotify.arrSetString[stSetRoomNotify.nStringCount] = pSetRoomReq->arrSetString[nStringIndex].GetString();
				stSetRoomNotify.nStringCount++;
				pRoom->SetRoomWelcome(pSetRoomReq->arrSetString[nStringIndex].GetString());
				nStringIndex++;
			}
			break;
			case enmRoomInfoType_NOTICE:
			{
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_NOTICE;
				stSetRoomNotify.nSetCount++;
				stSetRoomNotify.arrSetString[stSetRoomNotify.nStringCount] = pSetRoomReq->arrSetString[nStringIndex].GetString();
				stSetRoomNotify.nStringCount++;
				pRoom->SetRoomNotice(pSetRoomReq->arrSetString[nStringIndex].GetString());
				nStringIndex++;
			}
			break;
			case enmRoomInfoType_OPTION:
			{
				stSetRoomNotify.arrRoomInfoType[stSetRoomNotify.nSetCount] = enmRoomInfoType_OPTION;
				stSetRoomNotify.nSetCount++;
				pRoom->SetRoomOption(pSetRoomReq->nRoomOptionType);
			}
			break;
			default:
			break;
		}
	}
	//发送成功通知给客户端
	stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_OK;
	SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, &pData->m_sMsgHead, &stSetRoomResp,
			enmTransType_P2P);
	//通知房间里的人
	stSetRoomNotify.nSrcRoleID = pData->m_sMsgHead.nRoleID;
	stSetRoomNotify.nMicTime = pSetRoomReq->nMicTime;
	stSetRoomNotify.nRoomOptionType = pSetRoomReq->nRoomOptionType;
	if(stSetRoomNotify.nSetCount > 0)
	{
		SendMessageNotifyToClient(MSGID_RSCL_SET_ROOM_NOTI, &stSetRoomNotify, pData->m_sMsgHead.nRoomID, enmBroadcastType_ExceptPlayr,
				pData->m_sMsgHead.nRoleID);
	}
	WRITE_NOTICE_LOG("set room_info:set room info success{nRoleID=%d, nRoomID=%d}\n",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	return ret;
}
int32_t CSetRoomInfoEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	if(pSession == NULL)
	{
		WRITE_ERROR_LOG("pSession NULL ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	CSetRoomSessionData * pData =(CSetRoomSessionData *)(pSession->GetSessionData());
	if(NULL==pData)
	{
		WRITE_ERROR_LOG("pData null ret=0x%08x \n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("set room_info:recv set room info resp from DB{nRoleID=%d, nRoomID=%d}\n",pData->m_sMsgHead.nRoleID,pData->m_sMsgHead.nRoomID);
	//发送失败回应
	CSetRoomResp stSetRoomResp;
	stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_UNKNOWN;
	SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, &pData->m_sMsgHead, &stSetRoomResp,
			enmTransType_P2P);
	return S_OK;
}
int32_t CSetRoomInfoEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_SET_ROOM_REQ:
		OnEventSetRoomInfo(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}

int32_t CSetRoomInfoEvent::OnEventSetRoomInfo(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
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
		WRITE_ERROR_LOG("get room error!{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID,ret);
		return ret;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	CSetRoomReq *pSetRoomReq = dynamic_cast<CSetRoomReq *>(pMsgBody);
	if(pSetRoomReq == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pSetRoomReq=0x%08x}\n",pSetRoomReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("set room_info:rcev set room info req{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
	CSetRoomResp stSetRoomResp;
	//判断权限
	RoleRank nRoleRank = pRoom->GetRoleRank(pPlayer->GetRoleID());
	//不可以操作
	if(nRoleRank < enmRoleRank_TempAdmin)
	{
		WRITE_ERROR_LOG("set room_info:player has no permission!{nRoleID=%d, nRoleRank=%d}\n",pPlayer->GetRoleID(),nRoleRank);
		stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_NO_PERMISSION;
		SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, pMsgHead, &stSetRoomResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}

	//todo 权限控制
	for(int32_t i = 0;i < pSetRoomReq->nSetCount;i++)
	{
		switch(pSetRoomReq->arrRoomInfoType[i])
		{
			//必须是室主或者副室主
			case enmRoomInfoType_NAME:
			case enmRoomInfoType_SIGNATURE:
			case enmRoomInfoType_PASSWORD:
			case enmRoomInfoType_WELCOME_WORDS:
			case enmRoomInfoType_NOTICE:
			{
				if(nRoleRank<enmRoleRank_MinorHost)
				{
					WRITE_ERROR_LOG("set room_info:player has no permission{nRoleID=%d, nRoleRank=%d, nRoomInfoType=%d}\n",pPlayer->GetRoleID(),nRoleRank,pSetRoomReq->arrRoomInfoType[i]);
					stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_NO_PERMISSION;
					SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, pMsgHead, &stSetRoomResp,
							enmTransType_P2P, nOptionLen, (char *)pOptionData);
					return ret;
				}
			}
			break;
			case enmRoomInfoType_FREESHOW_TIME:
			{
               if(pRoom->IsCloseAdminEditMicTime())
               {
            	   //不可以操作
					if(nRoleRank<=enmRoleRank_Admin)
					{
						WRITE_ERROR_LOG("set room_info:player has no permission edit mic time{nRoleID=%d, nRoleRank=%d}!\n",pPlayer->GetRoleID(),nRoleRank);
						stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_NO_PERMISSION;
						SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, pMsgHead, &stSetRoomResp,
								enmTransType_P2P, nOptionLen, (char *)pOptionData);
						return ret;
					}
               }
			}
			break;
			default:
			break;
		}
	}
	//发送修改设置到数据库
	ret = SendReqToDB(pMsgHead,pSetRoomReq,nOptionLen,pOptionData,pPlayer,pRoom);
	if(ret<0)
	{
		WRITE_ERROR_LOG("set room_info:send session to db filed{ret=0x%08x}\n",ret);
		stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, pMsgHead, &stSetRoomResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}
	return ret;
}
int32_t CSetRoomInfoEvent::SendReqToDB(MessageHeadSS *pMsgHead, CSetRoomReq *pSetRoomReq,const uint16_t nOptionLen , const void *pOptionData ,CPlayer *pPlayer,CRoom *pRoom)
{
	int32_t ret = S_OK;
	if(NULL==pMsgHead||NULL==pSetRoomReq||NULL==pPlayer||NULL==pRoom)
	{
		return E_NULLPOINTER;
	}
	CFrameSession *pSession = NULL;
	ret = g_Frame.CreateSession(MSGID_DBRS_SET_ROOM_RESP, enmSessionType_SetRoom, this, enmSetRoomTimeoutPeriod, pSession);
	if(ret < 0 || pSession == NULL)
	{
		WRITE_ERROR_LOG("set room_info:set room info session is failed!{player=%d}\n", pPlayer->GetRoleID());
		CSetRoomResp stSetRoomResp;
		stSetRoomResp.nSetRoomInfoResult = enmSetRoomInfoResult_UNKNOWN;
		SendMessageResponse(MSGID_RSCL_SET_ROOM_RESP, pMsgHead, &stSetRoomResp,
				enmTransType_P2P, nOptionLen, (char *)pOptionData);
		return ret;
	}

	CSetRoomSessionData * pData = new (pSession->GetSessionData())CSetRoomSessionData();
	memcpy(&pData->m_sMsgHead, pMsgHead, sizeof(pData->m_sMsgHead));
	memcpy(&pData->m_pMsgBody, pSetRoomReq, sizeof(pData->m_pMsgBody));

	//发送请求
	CSetRoomAttrReq stSetRoomAttrReq;
	stSetRoomAttrReq.nSetCount = pSetRoomReq->nSetCount;
	stSetRoomAttrReq.nStringCount = pSetRoomReq->nStringCount;
	stSetRoomAttrReq.nMicTime = pSetRoomReq->nMicTime*60;
	stSetRoomAttrReq.nRoomOptionType = pSetRoomReq->nRoomOptionType;
	for(int32_t i = 0;i < pSetRoomReq->nSetCount;i++)
	{
		stSetRoomAttrReq.arrRoomInfoType[i] = pSetRoomReq->arrRoomInfoType[i];
	}
	for(int32_t i = 0;i < pSetRoomReq->nStringCount;i++)
	{
		stSetRoomAttrReq.arrSetString[i] = pSetRoomReq->arrSetString[i];
	}
	SendMessageRequest(MSGID_RSDB_SET_ROOM_REQ , &stSetRoomAttrReq, pMsgHead->nRoomID,
			enmTransType_ByKey, pMsgHead->nRoleID, enmEntityType_DBProxy, pMsgHead->nRoleID,
			pSession->GetSessionIndex(), 0, NULL, "send SetRoomInfo request");

	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END
