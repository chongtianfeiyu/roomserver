/*
 *  bll_event_closeprivate.cpp
 *
 *  To do��
 *  Created on: 	2011-12-19
 *  Author: 		luocj
 */

#include "bll_event_closeprivate.h"
#include "../dal/to_server_message.h"
#include "ctl/server_datacenter.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CClosePrivateEvent::OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody, CFrameSession* pSession,const uint16_t nOptionLen , const void *pOptionData )
{
	return S_OK;
}
int32_t CClosePrivateEvent::OnSessionTimeOut(CFrameSession *pSession)
{
	return S_OK;
}
int32_t CClosePrivateEvent::OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,const uint16_t nOptionLen , const void *pOptionData )
{
	if(pMsgBody==NULL || pMsgHead==NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	switch(pMsgHead->nMessageID)
	{
		case MSGID_CLRS_CLOSE_PEIVATE_REQ:
		OnEventClosePrivate(pMsgHead,pMsgBody);
		break;
		default:
		break;
	}
	return S_OK;
}
/*
 * ���ر�˽�� ���������Կͻ��ˣ� δ�����  ^ ^
 */
int32_t CClosePrivateEvent::OnEventClosePrivate(MessageHeadSS *pMsgHead, IMsgBody* pMsgBody)
{
	int32_t ret = S_OK;
	if(pMsgBody == NULL || pMsgHead == NULL)
	{
		WRITE_ERROR_LOG("null pointer:{pMsgBody=0x%08x, pMsgHead=0x%08x}\n",pMsgBody,pMsgHead);
		return E_NULLPOINTER;
	}
	CClosePrivateMicReq *pClosePrivateMicReqMsg = dynamic_cast<CClosePrivateMicReq *>(pMsgBody);
	if(pClosePrivateMicReqMsg == NULL)
	{
		WRITE_ERROR_LOG("pClosePrivateMicReqMsg null {ret=0x%08x}\n",E_NULLPOINTER);
		return E_NULLPOINTER;
	}
	WRITE_NOTICE_LOG("recv player close private mic{nSrcRoleID=%d, nRoomID=%d, nDestRoleID=%d}\n",pMsgHead->nRoleID,pMsgHead->nRoomID,pClosePrivateMicReqMsg->nDestRoleID);
	CPlayer *pPlayer = NULL;
	PlayerIndex nPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pMsgHead->nRoleID, pPlayer, nPlayerIndex);
	//û�д����
	if(ret < 0 || pPlayer == NULL)
	{
		WRITE_ERROR_LOG("player does no exist{nSrcRoleID=%d}",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ�������
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	ret = g_RoomMgt.GetRoom(pMsgHead->nRoomID, pRoom, nRoomIndex);
	if(ret < 0 || pRoom == NULL)
	{
		WRITE_ERROR_LOG("get room error!{ret=0x%08x} \n",ret);
		return S_OK;
	}
	//�ж�����Ƿ��ڷ���roomID��
	if(!(pRoom->IsPlayerInRoom(nPlayerIndex)))
	{
		WRITE_ERROR_LOG("player does no in room {nSrcRoleID=%d, nRoomID=%d}",pMsgHead->nRoleID,pMsgHead->nRoomID);
		return E_RS_PLAYRENOTINROOM;
	}
	//�˴������������Ƿ��ڽ��ձ����ߵ���Ƶ
	ret = pPlayer->DelWatchPrivateMic(pClosePrivateMicReqMsg->nDestRoleID,pMsgHead->nRoomID);
	if(ret==(int32_t)E_RS_PLAYERNOTWATCHTHISMIC)
	{
		WRITE_ERROR_LOG("player does no watch this private mic{nSrcRoleID=%d, nDestRoleID=%d}",pMsgHead->nRoleID,pClosePrivateMicReqMsg->nDestRoleID);
		return ret;
	}
	else if(ret<0)
	{
		WRITE_ERROR_LOG("player not in this room {nSrcRoleID=%d}!\n",pMsgHead->nRoleID);
		return ret;
	}
	//��ȡ���ر�˽�����ҵĶ���
	CPlayer *pDestPlayer = NULL;
	PlayerIndex nDestPlayerIndex = enmInvalidPlayerIndex;
	ret = g_PlayerMgt.GetPlayer(pClosePrivateMicReqMsg->nDestRoleID, pDestPlayer, nDestPlayerIndex);
	//û�д����
	if(ret < 0 || pDestPlayer == NULL)
	{
		WRITE_WARNING_LOG("player does no exist{nDestRoleID=%d}",pClosePrivateMicReqMsg->nDestRoleID);
		WRITE_NOTICE_LOG("player close privateMic {roleid=%d,privatemic=%d}",pMsgHead->nRoleID,pClosePrivateMicReqMsg->nDestRoleID);
		return ret;
	}
    if(!pDestPlayer->IsStateInRoom(enmPlayerState_PrivateMic,pMsgHead->nRoomID))
    {
    	WRITE_NOTICE_LOG("player close privateMic {roleid=%d,privatemic=%d}",pMsgHead->nRoleID,pClosePrivateMicReqMsg->nDestRoleID);
    	return ret;
    }
    WRITE_NOTICE_LOG("player close privateMic {roleid=%d,privatemic=%d}",pMsgHead->nRoleID,pClosePrivateMicReqMsg->nDestRoleID);

    //����֪ͨ��mediaserver
    RoleID arrReciver[1] = {pMsgHead->nRoleID};
    g_BllBase.SendDelTransmitUserNotifyToMediaServer(pClosePrivateMicReqMsg->nDestRoleID, pRoom, arrReciver, 1);

    return ret;

//	����֪ͨ��mediaserver
//	CDelTransmitUserNotice stDelTransmitUserNotice;
//	stDelTransmitUserNotice.nUserStatus = enmInvalidUserStatus;
//	stDelTransmitUserNotice.nRoleID = pClosePrivateMicReqMsg->nDestRoleID;
//	stDelTransmitUserNotice.nDelCount = 1;
//	stDelTransmitUserNotice.arrDelRoleID[0]=pMsgHead->nRoleID;
//	stDelTransmitUserNotice.nRoomID = pMsgHead->nRoomID;
//
//	SendMessageNotifyToServer(MSGID_RSMS_DELETE_TRANSMITUSER_NOTI,&stDelTransmitUserNotice,pMsgHead->nRoomID,enmTransType_P2P,
//			pClosePrivateMicReqMsg->nDestRoleID,enmEntityType_Media, pRoom->GetMediaServerIDByType(pPlayer->GetPlayerNetType()));
//	return ret;
}
FRAME_ROOMSERVER_NAMESPACE_END

