/*
 *  bll_event_operate_av.cpp
 *
 *  To do：
 *  Created on: 	2011-12-20
 *  Author: 		luocj
 */

#include "bll_event_operate_av.h"
#include "../dal/to_server_message.h"
#include "../dal/to_client_message.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t COprateAVReqEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t COprateAVReqEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t COprateAVReqEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_OPERATE_VIDEO_AUDIO_REQ:
		OnEventOperateAVReq(pMsgHead,pMsgBody,nOptionLen,pOptionData);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * 处理修改默认的音视频（来自客户端）
 */
int32_t COprateAVReqEvent::OnEventOperateAVReq(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	int32_t ret = S_OK;

	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgHead=0x%08x, pMsgBody=0x%08x}\n",pMsgHead,pMsgBody);
		return E_NULLPOINTER;
	}
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//没有次玩家
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nRoleId=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//获取房间对象
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error{nRoomID=%d, ret=0x%08x}\n",pMsgHead->nRoomID,ret);
		return S_OK;
	}
	//判断玩家是否在房间roomID中
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room{nRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	COperateVAReq *pOperateVAReq = dynamic_cast<COperateVAReq *>(pMsgBody);
	if(pOperateVAReq==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pOperateVAReq=0x%08x}\n",pOperateVAReq);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("operateAV:rcev player req{nRoleId=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pOperateVAReq->nDestRoleID);
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pOperateVAReq->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//没有次玩家
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nDestRoleID=%d}",pOperateVAReq->nDestRoleID);
		return ret;
	}
	COperateVAResp stOperateVAResp;
	//判断被关者是否在麦上
	if(!(pDestPlayer->IsOnMicInRoom(pMsgHead->nRoomID)))
	{
		//发送不再卖上的回应
		WRITE_ERROR_LOG("player does no on MIC{nDestRoleID=%d}",pOperateVAReq->nDestRoleID);
		stOperateVAResp.nOperateResult = OPERATERESULT_NOT_ON_SHOW;
		SendMessageResponse(MSGID_RSCL_OPERATE_VIDEO_AUDIO_RESP, pMsgHead, &stOperateVAResp,enmTransType_P2P,nOptionLen,(char *)pOptionData);
		return ret;
	}
	//通知内容
	COperateVANotify stOperateVANotify;
	stOperateVANotify.nOperateAVType = pOperateVAReq->nOperateAVType;
	stOperateVANotify.nSrcRoleID = pMsgHead->nRoleID;
	//自己关闭自己的音频，或者视频
	if(pMsgHead->nRoleID == pOperateVAReq->nDestRoleID)
	{
		switch(pOperateVAReq->nOperateAVType)
		{
			case OPERATEAVTYPE_OPEN_VIDEO:
			{
				ret = pDestPlayer->SetRoleVideoInRoom(enmAVType_Open,pMsgHead->nRoomID);
			}
			break;
			case OPERATEAVTYPE_OPEN_AUDIO:
			{
				ret = pDestPlayer->SetRoleAudioInRoom(enmAVType_Open,pMsgHead->nRoomID);
			}
			break;
			case OPERATEAVTYPE_CLOSE_VIDEO:
			{
				ret = pDestPlayer->SetRoleVideoInRoom(enmAVType_Close,pMsgHead->nRoomID);
			}
			break;
			case OPERATEAVTYPE_CLOSE_AUDIO:
			{
				ret = pDestPlayer->SetRoleAudioInRoom(enmAVType_Close,pMsgHead->nRoomID);
			}
			break;
			default:
			break;
		}
		if(ret<0)
		{
			WRITE_ERROR_LOG("player does no in room{nRoleID=%d,nRoomID=%d}!\n",pOperateVAReq->nDestRoleID,pMsgHead->nRoomID);
			return ret;
		}
		//给关的人发响应
		stOperateVAResp.nOperateResult = OPERATERESULT_OK;
		SendMessageResponse(MSGID_RSCL_OPERATE_VIDEO_AUDIO_RESP, pMsgHead, &stOperateVAResp,enmTransType_P2P,nOptionLen,(char *)pOptionData);
		//to do通知房间所有人
		SendMessageNotifyToClient(MSGID_RSCL_OPERATE_VIDEO_AUDIO_NOTI, &stOperateVANotify, pMsgHead->nRoomID, enmBroadcastType_ExceptPlayr,
				pOperateVAReq->nDestRoleID, 0, NULL, "send operate_av notify");
		return ret;
	}
	//给关的人发响应
	stOperateVAResp.nOperateResult = OPERATERESULT_OK;
	SendMessageResponse(MSGID_RSCL_OPERATE_VIDEO_AUDIO_RESP, pMsgHead, &stOperateVAResp,enmTransType_P2P,nOptionLen,(char *)pOptionData);

	//发送修改通知给mediaServer(不是自己关自己的时候发送)
	if(!(pDestPlayer->IsReboot()))
	{
		ret = SendNotifyToMediaServer(pMsgHead->nRoleID,pMsgHead->nRoomID,pOperateVAReq, pPlayer->GetConnInfo().nServerID);
		if(ret<0)
		{
			WRITE_ERROR_LOG("send modify notify to mediaserver filed!{ret=0x%08x}\n",ret);
		}
	}
	return ret;
}
/*
 * 发送modify通知到mediaServer
 */
int32_t COprateAVReqEvent::SendNotifyToMediaServer(RoleID nModifyRoleID,RoomID nRoomID,COperateVAReq *pOperateVAReq,int32_t nMediaID/*,int32_t nCncMediaID*/)
{
	int32_t ret = S_OK;
	if(pOperateVAReq==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pOperateVAReq=0x%08x}\n",pOperateVAReq);
		return E_NULLPOINTER;
	}
	CModifyMediaNotice stModifyMediaNotice;
	stModifyMediaNotice.nUserStatus = enmInvalidUserStatus;
	stModifyMediaNotice.nRoleID = pOperateVAReq->nDestRoleID;
	stModifyMediaNotice.nRoomID = nRoomID;
	stModifyMediaNotice.nModifyRoleID = nModifyRoleID;

	switch(pOperateVAReq->nOperateAVType)
	{
		case OPERATEAVTYPE_OPEN_VIDEO:
		{
			stModifyMediaNotice.nModifyType = enmVedioModifyType;
			stModifyMediaNotice.nModifyValue = enmOpenModifyValue;
		}
		break;
		case OPERATEAVTYPE_OPEN_AUDIO:
		{
			stModifyMediaNotice.nModifyType = enmAudioModifyType;
			stModifyMediaNotice.nModifyValue = enmOpenModifyValue;
		}
		break;
		case OPERATEAVTYPE_CLOSE_VIDEO:
		{
			stModifyMediaNotice.nModifyType = enmVedioModifyType;
			stModifyMediaNotice.nModifyValue = enmCloseModifyValue;
		}
		break;
		case OPERATEAVTYPE_CLOSE_AUDIO:
		{
			stModifyMediaNotice.nModifyType = enmAudioModifyType;
			stModifyMediaNotice.nModifyValue = enmCloseModifyValue;
		}
		break;
		default:
		{
			ret = E_INVALIDARGUMENT;
			return ret;
		}
		break;
	}

	SendMessageNotifyToServer(MSGID_RSMS_MODIFY_TRANSMIT_NOTI,&stModifyMediaNotice,nRoomID,enmTransType_P2P,pOperateVAReq->nDestRoleID,enmEntityType_Media,nMediaID);
//	SendMessageNotifyToServer(MSGID_RSMS_MODIFY_TRANSMIT_NOTI,&stModifyMediaNotice,nRoomID,enmTransType_P2P,pOperateVAReq->nDestRoleID,enmEntityType_Media,nCncMediaID);
	return ret;
}

FRAME_ROOMSERVER_NAMESPACE_END
