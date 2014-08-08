/*
 * main_frame.cpp
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#include "ctl/server_datacenter.h"
#include "def/def_message_id.h"
#include "config/someconfig.h"
#include "def/dal_def.h"
#include "main_frame.h"
#include "bll/bll_event_login.h"
#include "bll/bll_event_logout.h"
#include "dal/from_client_message.h"
#include "bll/bll_event_closeprivate.h"
#include "bll/bll_event_openprivate.h"
#include "bll/bll_event_p2p_req.h"
#include "bll/bll_event_p2p_response.h"
#include "bll/bll_event_operate_av.h"
#include "bll/bll_event_getuserlist.h"
#include "bll/bll_event_text.h"
#include "bll/bll_event_rowmic.h"
#include "bll/bll_event_kickuser.h"
#include "bll/bll_event_add_black.h"
#include "bll/bll_event_shutup.h"
#include "bll/bll_event_unlock_shutup.h"
#include "bll/bll_event_lockip.h"
#include "bll/bll_event_online.h"
#include "bll/bll_event_offline.h"
#include "bll/bll_event_pushon_show.h"
#include "bll/bll_event_pulloff_show.h"
#include "bll/bll_event_set_roominfo.h"
#include "bll/bll_event_getwaitting.h"
#include "bll/bll_event_getroominfo.h"
#include "bll/bll_event_modifytitle.h"
#include "bll/bll_event_update_viplevel.h"
#include "bll/bll_event_lockroom.h"
#include "bll/bll_event_audit_roomname.h"
#include "bll/bll_event_modify_capacity.h"
#include "bll/bll_event_buildroomdatanotice.h"
#include "bll/bll_event_buildplayerdatanotice.h"
#include "bll/bll_event_updateselfsetting.h"
#include "bll/bll_event_setmic.h"
#include "bll/bll_event_update_globalstate.h"
#include "bll/bll_event_heartbeat.h"
#include "bll/bll_event_item_needrebulid.h"
#include "bll/bll_event_redispatch_media.h"
#include "bll/bll_event_modify_robotconfig.h"
#include "bll/bll_event_sendcolor_consume.h"
#include "bll/bll_event_edit_blacklist.h"
#include "bll/bll_event_web_openprivatemic.h"
#include "bll/bll_event_update_identity.h"
#include "bll/bll_event_update_magnate_level.h"
#include "requestsong/bll_event_updatesonglist.h"
#include "requestsong/bll_event_getsonglist.h"
#include "requestsong/bll_event_getorderlist.h"
#include "requestsong/bll_event_requestsong.h"
#include "requestsong/bll_event_processorder.h"
#include "requestsong/bll_event_fightforticket.h"
#include "requestsong/bll_event_songvote.h"
#include "bll/bll_event_registaddressinfo.h"
#include "bll/bll_event_offline_artistlist.h"


#include "hallserver_message_define.h"
#include "public_message_define.h"
#include "roomdispatcher_message_define.h"
#include "itemserver_message_define.h"
#include "webagent_message_define.h"
#include "mediaserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

CMainFrame::CMainFrame()
:CLightFrame(SERVER_NAME_STRING)
{

}

CMainFrame::~CMainFrame()
{
}

//框架初始话
int32_t CMainFrame::Initialize()
{
	int32_t ret=S_OK;
	AddConfigCenter(0,DEFAULT_ROOM_CONFIG,&g_SomeConfig);

	AddConfigCenter(0,DEFAULT_REBOT_CONFIG,&g_RebotConfig);

	//添加sever自己需要的数据中心
	ret=AddDataCenter(&GET_DALDATACENTER_INSTANCE());
	if(ret<0)
	{
		return ret;
	}

	ret = CLightFrame::Initialize();
	if(0 > ret)
	{
		//WriteLog
		return ret;
	}
	//注册所有的消息
	RegistMsg();

	g_FrameLogEngine.RegistLog("player_count_sta");

	return S_OK;
}

//恢复框架
int32_t CMainFrame::Resume()
{
	return CLightFrame::Resume();

}
//销毁框架
int32_t CMainFrame::Terminate()
{
	return CLightFrame::Terminal();

}
void CMainFrame::RegistMsg()
{
	INITIALIZE_MESSAGEMAP_DECL(roomserver);

	//INITIALIZE_MESSAGEMAP_DECL(client_message);


	INITIALIZE_MESSAGEMAP_DECL(from_client);
	RegistMsgHandler(MSGID_CLRM_GETARTISTOFFLINE_REQ, &GET_GETARTISTEVENT_INSTANCE());

	RegistMsgHandler(MSGID_CLRS_LOGIN_REQ, &CREATE_LOGINEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_LOGOUT_REQ, &CREATE_LOGOUTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_OPEN_PEIVATE_REQ, &CREATE_OPENPRIVATEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_CLOSE_PEIVATE_REQ, &CREATE_CLOSEPRIVATEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SENDP2P_REQ,   &CREATE_P2PREQEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_RECVP2P_REQ,  &CREATE_P2PREPONSEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_OPERATE_VIDEO_AUDIO_REQ,  &CREATE_OPRATEAVEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_GETUSERLIST_REQ,  &CREATE_GETUSERLISTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SENDTEXT_REQ,  &CREATE_TEXTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_REQUESTSHOW_REQ,  &CREATE_ROWMICEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_EXITSHOW_REQ,  &CREATE_ROWMICEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_KICK_USER_REQ,  &CREATE_KICKUSEREVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_ADD_BLACK_REQ,  &CREATE_ADDBLOCKEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SHUT_UP_REQ,  &CREATE_SHUTUPEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_UNLOCK_SHUT_UP_REQ,  &CREATE_UNLOCKSHUTUPEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_LOCK_IP_REQ,  &CREATE_LOCKIPEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_CLRS_ADD_TITLE_REQ,  &CREATE_ADDTITLEEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_CLRS_DEL_TITLE_REQ,  &CREATE_DELTITLEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_PUSH_ON_SHOW_REQ,  &CREATE_PUSHONSHOWEVENT_INSTANCE());
	RegistMsgHandler(MSGID_RSCL_ANSWER_PULL_SHOW_REQ,  &CREATE_PUSHONSHOWEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_PULL_OFF_SHOW_REQ,  &CREATE_PULLOFFSHOWEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SET_ROOM_REQ,  &CREATE_SETROOMINFOEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_GETWAITTINGLIST_REQ,  &CREATE_GETWAITTINGEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_GET_ROOM_REQ,  &CREATE_GETROOMINFOEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SET_MIC_REQ,  &CREATE_SETMICSTATUSEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_HEARTBEAT,  &CREATE_HEARTBEATEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_UPDATESONGLIST_REQ,  &CREATE_UPDATESONGLISTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_FIGHTFORTICKET_REQ,  &CREATE_FIGHTFORTICKETEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_SONGVOTE_REQ,  &CREATE_SONGVOTEEVENT_INSTANCE());

	RegistMsgHandler(MSGID_CLRS_GETSONGLIST_REQ,  &CREATE_GETSONGLISTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_REQUESTSONG_REQ,  &CREATE_REQUESTSONGEVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_PROCESSORDER_REQ,  &CREATE_PROCESSORDEREVENT_INSTANCE());
	RegistMsgHandler(MSGID_CLRS_GETORDERLIST_REQ,  &CREATE_GETORDERLISTEVENT_INSTANCE());
	INITIALIZE_MESSAGEMAP_DECL(hallserver);
	RegistMsgHandler(MSGID_HSTA_ONLINE_NOTI, &CREATE_ONLINEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_HSTA_OFFLINE_NOTI, &CREATE_OFFLINEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_HSRS_SETSELFSETTING_NOTI, &CREATE_UPDATESELFSETTINGEVENT_INSTANCE());
	RegistMsgHandler(MSGID_HSRS_UPDATEGLOBALSTATE_NOTI, &CREATE_UPDATESTATEEVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(from_server);
//	RegistMsgHandler(MSGID_HSRS_GETMEIDAINFO_RESP, &CREATE_LOGINEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_HSRS_GET_ROLE_GLOBALINFO_RESP, &CREATE_LOGINEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_GETROLEINFO_RESP, &CREATE_LOGINEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_GETROOMINFO_RESP, &CREATE_LOGINEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_ISRS_GET_ITEMINFO_RESP, &CREATE_LOGINEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_DEL_TITLE_RESP, &CREATE_DELTITLEEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_SET_TITLE_RESP, &CREATE_ADDTITLEEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_SET_ROOM_RESP, &CREATE_SETROOMINFOEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_DBRS_ADD_BLACK_RESP,  &CREATE_ADDBLOCKEVENT_INSTANCE());

	RegistMsgHandler(MSGID_HSTA_MODIFY_TITLE_NOTIFY, &CREATE_MODIFYTITLEEVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(public_message);
	RegistMsgHandler(MSGID_SSRS_UPDATELEVEL_NOTI, &CREATE_UPDATEVIPLEVELEVENT_INSTANCE());
	RegistMsgHandler(MSGID_WEBOT_LOCKROOM_NOTIFY, &CREATE_LOCKROOMEVENT_INSTANCE());
	RegistMsgHandler(MSGID_WEBOT_AUDIT_ROOMNAME_NOTIFY, &CREATE_AUDITROOMNAMEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_WEBOT_MODIFY_ROOMCAPACITY_NOTIFY, &CREATE_MODIFYCAPACITYEVENT_INSTANCE());
	RegistMsgHandler(MSGID_ISRS_UPDATEMAGNATELEVEL_NOTI, &CREATE_UPDATEMAGNATELEVELEVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(roomdispatcher);
	RegistMsgHandler(MSGID_RDRS_BUILDROOMDATA_NOTI, &CREATE_BUILDROOMDATANOTICEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_RDRS_BUILDPLAYERDATA_NOTI, &CREATE_BUILDPLAYERDATANOTICEEVENT_INSTANCE());
	RegistMsgHandler(MSGID_RDRS_SETMEIDAINFO_NOTI, &CREATE_REDISPATCHMEDIAEVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(itemserver);
	RegistMsgHandler(MSGID_ISRS_NEEDREBULID_NOTICE, &CREATE_ITEMREBULIDEVENT_INSTANCE());
	RegistMsgHandler(MSGID_ISOS_UPDATEIDENTITY_NOTI, 	&CREATE_UPDATEIDENTITYEVENT_INSTANCE());
//	RegistMsgHandler(MSGID_ISRS_BIGCONSUME_NOTI, 	&CREATE_SENDCOLOREVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(webagent);
	RegistMsgHandler(MSGID_WSRS_MODIFY_ROBOTCONFIG_NOTI, &CREATE_MODIFYROBOTCONFIGEVENT_INSTANCE());
	RegistMsgHandler(MSGID_WEBRS_EDIT_BLACKLIST_NOTICE, &CREATE_EDITBLACKLISTEVENT_INSTANCE());
	RegistMsgHandler(MSGID_WEBOT_EDIT_PRIVATE_NOTICE, &CREATE_WEBOPENPRIVATEMICEVENT_INSTANCE());

	INITIALIZE_MESSAGEMAP_DECL(mediaserver);
	RegistMsgHandler(MSGID_MSRS_REGISTADDRESSINFO_NOTICE, &CREATE_REGISTADDRESSINFOEVENT_INSTANCE());
}


void CMainFrame::OnTestRequest()
{
	printf("Enter msgid(hex):\n");
	int32_t command = 0;
	scanf("%x", &command);
	switch( command )
	{
	case MSGID_CLRM_GETARTISTOFFLINE_REQ:
		GetRoomArtistReq( command );
		break;

	default:
		printf("Invalid command!\n");
		break;
	}
}

void CMainFrame::GetRoomArtistReq(uint32_t msgid)
{
	printf("Enter roleid:");

	RoleID roomID = 0;
	scanf("%d", &roomID);
//	VipLevel nVipLevel = 0;
//	scanf("%d", &nVipLevel);


	MessageHeadSS *pMessageHead = new(g_messageHead)MessageHeadSS;
	GenerateMessageHead(msgid, roomID, enmEntityType_Tunnel, enmEntityType_Room, *pMessageHead, sizeof(CGetArtistListReq));

	CGetArtistListReq *pMessageBody = new(g_messageBody)CGetArtistListReq;
	pMessageBody->nRoomID	= roomID;


	//uint8_t *pOptionData = NULL;
	//uint32_t offset = 0;
	//CFrameCodeEngine::MessageEncode(pOptionData, sizeof(ConnUin), offset, stInfo);

	g_Frame.PushCommand(pMessageHead,pMessageBody, 0, NULL);
}

//生成消息头
void CMainFrame::GenerateMessageHead(uint32_t nMessageID, RoleID nRoleID, EntityType nSourceType,
		EntityType nDestType, MessageHeadSS& stMessageHead, int32_t MessageBodySize)
{
	stMessageHead.nMessageID = nMessageID;
	stMessageHead.nMessageType = enmMessageType_Request;
	stMessageHead.nSourceType = nSourceType;
	stMessageHead.nDestType = nDestType;
	stMessageHead.nSourceID = 1;
	stMessageHead.nDestID = 1;
	stMessageHead.nRoleID = nRoleID;
	stMessageHead.nSequence = 0;
	stMessageHead.nSessionIndex = enmInvalidSessionIndex;
	stMessageHead.nRoomID = 0;

	stMessageHead.nTransType = enmTransType_ByKey;
	stMessageHead.nHeadSize = sizeof(MessageHeadSS);
	stMessageHead.nTotalSize = sizeof(MessageHeadSS) + MessageBodySize;
}

FRAME_ROOMSERVER_NAMESPACE_END

