/*
 * bll_event_text.cpp
 *
 *  Created on: 2011-12-24
 *      Author: jimm
 */

#include "bll_event_text.h"
#include "def/server_errordef.h"
#include "ctl/server_datacenter.h"
#include "common/common_datetime.h"
#include "dal/to_client_message.h"
#include "taskserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CTextMessageEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		CFrameSession* pSession, const uint16_t nOptionLen, const void *pOptionData)
{
	return S_OK;
}
int32_t CTextMessageEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CTextMessageEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
		const uint16_t nOptionLen, const void *pOptionData)
{
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}

	CSendTextResp stSendTextResp;
	CSendTextNoti stSendTextNoti;

	CSendTextReq *pSendTextMsg = dynamic_cast<CSendTextReq *>(pMsgBody);
	if(NULL == pSendTextMsg)
	{
		WRITE_ERROR_LOG("null pointer:{pSendTextMsg=0x%08x, nRoleID=%d, nRoomID=%d}\n",pSendTextMsg,pMsgHead->nRoleID,pMsgHead->nRoomID);
		stSendTextResp.nResult = enmTextResult_Unknown;
		stSendTextResp.strFailReason = "unknown error";
		SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);

		return E_NULLPOINTER;
	}

	WRITE_NOTICE_LOG("send message:recv request{nRoleID=%d, nRoomID=%d}", pMsgHead->nRoleID,pMsgHead->nRoomID);

	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	int32_t ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("get player object error{RoleID=%d, ret=0x%08x}", pMsgHead->nRoleID, ret);
		stSendTextResp.nResult = enmTextResult_Other;
		stSendTextResp.strFailReason = "get player error";

		SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);
		return ret;
	}

	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room object error{nRoomID=%d,ret=0x%08x}", pMsgHead->nRoomID, ret);
		stSendTextResp.nResult = enmTextResult_Other;
		stSendTextResp.strFailReason = "get room error";
		SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);
		return ret;
	}

	VerifyPlayer(pMsgHead, pSendTextMsg, pRoom, pPlayer, nPlayerIndex, stSendTextResp, stSendTextNoti);

	if(stSendTextResp.nResult == enmTextResult_OK)
	{
		if((pSendTextMsg->nTextType == enmTextType_Public) ||
				(pSendTextMsg->nTextType == enmTextType_Temp_Notice) ||
				(pSendTextMsg->nTextType == enmTextType_Notice))
		{
			SendMessageNotifyToClient(MSGID_RSCL_SENDTEXT_NOTI, &stSendTextNoti, pMsgHead->nRoomID, enmBroadcastType_ExceptPlayr,
					pMsgHead->nRoleID, 0, NULL, "send text notify");
		}
		else if(pSendTextMsg->nTextType == enmTextType_Private)
		{
			CPlayer *pDestPlayer = NULL;
			PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
			ret = g_PlayerMgt.GetPlayer(pSendTextMsg->nDestID, pDestPlayer, nDestPlayerIndex);
			if(ret < 0 || pDestPlayer == NULL)
			{
				WRITE_ERROR_LOG("get dest player object error{DestRoleID=%d,ret=0x%08x}", pSendTextMsg->nDestID, ret);

				stSendTextResp.nResult = enmTextResult_Other;
				stSendTextResp.strFailReason = "get player error";
				SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);
				return ret;
			}
			if(!(pRoom->IsPlayerInRoom(nDestPlayerIndex)) && !(pRoom->IsRebotPlayerInRoom(pSendTextMsg->nDestID)))
			{
				WRITE_ERROR_LOG("DestPlayer do not in room{nDestRoleID=%d, nRoomID=%d}", pSendTextMsg->nDestID,pRoom->GetRoomID());
				stSendTextResp.nResult = enmTextResult_NotInRoom;
				stSendTextResp.strFailReason = "NotInRoom";
				SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);
				return ret;
			}
			SendMessageNotifyToClient(MSGID_RSCL_SENDTEXT_NOTI, &stSendTextNoti, pMsgHead->nRoomID, enmBroadcastType_ExpectPlayr,pDestPlayer->GetRoleID(), 0, NULL, "send text notify");
		}
		if(pSendTextMsg->nFontStyle == enmFontStyleType_COLORTEXT)
		{
			//如果是发送的彩条，给taskserver通知
			CPlayerActionInfoNoti stPlayerActionInfoNoti;
			stPlayerActionInfoNoti.nRoleID = pMsgHead->nRoleID;
			stPlayerActionInfoNoti.nCount = 1;
			stPlayerActionInfoNoti.arrActionID[0] = enmSendColourBarType;
			SendMessageNotifyToServer(MSGID_TOTASK_ACTION_NOTIFY, &stPlayerActionInfoNoti, pMsgHead->nRoomID, enmTransType_Broadcast, pMsgHead->nRoleID, enmEntityType_Task);
		}
	}
	SendMessageResponse(MSGID_RSCL_SENDTEXT_RESP, pMsgHead, &stSendTextResp,enmTransType_P2P);
	return S_OK;
}

int32_t CTextMessageEvent::VerifyPlayer(MessageHeadSS * pMsgHead, CSendTextReq *pSendTextMsg, CRoom *pRoom, CPlayer *pPlayer,
		PlayerIndex nPlayerIndex, CSendTextResp &stSendTextResp, CSendTextNoti &stSendTextNoti)
{
	//玩家不在此房间
	if(!pRoom->IsPlayerInRoom(nPlayerIndex))
	{
		stSendTextResp.nResult = enmTextResult_NotInRoom;
		stSendTextResp.strFailReason = "player not in room";

		return S_OK;
	}

	//在禁言列表中
	int32_t nLeftTime = 0;
	if(pRoom->IsInProhibitList(pMsgHead->nRoleID,nLeftTime))
	{
		stSendTextResp.nResult = enmTextResult_InProhibited;
		stSendTextResp.strFailReason = "in prohibit list";
		stSendTextResp.nLeftTime = nLeftTime;
		return S_OK;
	}
	//判断权限
	//发送悄悄话
	if(pSendTextMsg->nTextType == enmTextType_Private)
	{
		//vip及临时管理以上及有身份的玩家可以发悄悄话
		if((pPlayer->GetVipLevel() < g_SomeConfig.GetCanPrivateTextVipLevel()) &&
				(pRoom->GetRoleRank(pPlayer->GetRoleID()) < g_SomeConfig.GetCanPrivateTextRoleRank()))
		{
			//关闭纯屌丝用户
			if(pPlayer->GetIdentityType() < g_SomeConfig.GetCanPrivateTextIdentityType())
			{
				stSendTextResp.nResult = enmTextResult_NoPermission;
				stSendTextResp.strFailReason = "NoPermission";
				return S_OK;
			}else if(pPlayer->GetIdentityType() == g_SomeConfig.GetCanPrivateTextIdentityType())
			{
				if(g_SomeConfig.IsClosePoorMan() && (g_SomeConfig.GetCanPrivateTextIdentityType() == enmIdentityType_None))
				{
					stSendTextResp.nResult = enmTextResult_NoPermission;
					stSendTextResp.strFailReason = "NoPermission";
					return S_OK;
				}
			}
		}
	}
	if(pSendTextMsg->nTextType == enmTextType_Temp_Notice)
	{
		//管理员以上可以发临时公告
		if(!(pRoom->GetRoleRank(pPlayer->GetRoleID()) >= enmRoleRank_Admin))
		{
			stSendTextResp.nResult = enmTextResult_NoPermission;
			stSendTextResp.strFailReason = "NoPermission";
			return S_OK;
		}
	}
	if(pSendTextMsg->nTextType == enmTextType_Notice)
	{
		//管理员以上可以发公告
		if(!(pRoom->GetRoleRank(pPlayer->GetRoleID()) >= enmRoleRank_MinorHost))
		{
			stSendTextResp.nResult = enmTextResult_NoPermission;
			stSendTextResp.strFailReason = "NoPermission";
			return S_OK;
		}
	}
//	if((pSendTextMsg->nDestID == 0) && (pSendTextMsg->nFontStyle != enmFontStyleType_COLORTEXT))
//	{
//		//对所有说话（有管理权限，或者vip才可以）
//		if(!(pPlayer->GetVipLevel() > enmVipLevel_REGISTER || pRoom->GetRoleRank(pMsgHead->nRoleID) > enmRoleRank_None))
//		{
//			stSendTextResp.nResult = enmTextResult_NoPermission;
//			stSendTextResp.strFailReason = "NoPermission";
//			return S_OK;
//		}
//
//	}
	if((pSendTextMsg->nTextType == enmTextType_Public) && (pSendTextMsg->nFontStyle != enmFontStyleType_COLORTEXT))
	{
//		TextResult nTextResult = enmTextResult_OK;
//		if(!(pRoom->IsCanPublicChat(pPlayer->GetVipLevel(),nTextResult)) && (pRoom->GetRoleRank(pPlayer->GetRoleID()) < enmRoleRank_TempAdmin))
		if((pPlayer->GetVipLevel() < g_SomeConfig.GetCanPublicTextVipLevel()) && (pRoom->GetRoleRank(pPlayer->GetRoleID()) < enmRoleRank_TempAdmin))
		{
			stSendTextResp.nResult = enmTextResult_NoPermission;
			stSendTextResp.strFailReason = "PublicClosed";
			return S_OK;
		}
	}
	if(pSendTextMsg->nFontStyle == enmFontStyleType_COLORTEXT)
	{
		if(!(pRoom->IsSendColorOpen()) && (pRoom->GetRoleRank(pPlayer->GetRoleID()) < enmRoleRank_TempAdmin))
		{
			stSendTextResp.nResult = enmTextResult_ColorClosed;
			stSendTextResp.strFailReason = "ColorClosed";
			return S_OK;
		}
	}
	stSendTextResp.nResult = enmTextResult_OK;

	stSendTextNoti.nTextType = pSendTextMsg->nTextType;
	stSendTextNoti.nSrcID = pMsgHead->nRoleID;
	stSendTextNoti.nDestID = pSendTextMsg->nDestID;
	stSendTextNoti.strMessage = pSendTextMsg->strMessage;
	stSendTextNoti.nFontSize = pSendTextMsg->nFontSize;
	stSendTextNoti.nFontColor = pSendTextMsg->nFontColor;
	stSendTextNoti.nFontStyle = pSendTextMsg->nFontStyle;
	stSendTextNoti.strFontType = pSendTextMsg->strFontType;
	stSendTextNoti.nAutoAnswer = pSendTextMsg->nAutoAnswer;
	stSendTextNoti.nRejectAutoAnswer = pSendTextMsg->nRejectAutoAnswer;

	WRITE_NOTICE_LOG("send message:{nSrcID=%d, nDestID=%d, strMessage=%s, nFontStyle=%d, strFontType=%s, nTextType=%d}",pMsgHead->nRoleID,
			pSendTextMsg->nDestID,
			pSendTextMsg->strMessage.GetString(),
			pSendTextMsg->nFontStyle,
			pSendTextMsg->strFontType.GetString(),
			pSendTextMsg->nTextType);
	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END

