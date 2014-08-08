/*
 * from_client_message.h
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#ifndef FROM_CLIENT_MESSAGE_H_
#define FROM_CLIENT_MESSAGE_H_

#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_msg_impl.h"
#include "common/common_string.h"
#include "def/dal_def.h"
#include "public_typedef.h"
#include "def/def_message_id.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//��˽������
class COpenPrivateMicReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;
};

//�ر�˽������
class CClosePrivateMicReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;
};

//������ĳ��һ��һ��Ƶ
class CP2PMicReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;
};

//����ĳ��һ��һ��Ƶ
class CP2PAnswerReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ResponseP2PType nResponseP2PType;
	RoleID nDestRoleID;
};

//����رջ��ߴ�ĳ�˵�����Ƶ
class COperateVAReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	OperateAVType nOperateAVType;
	RoleID nDestRoleID;
};

//��½����
class CLoginReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	CString<MaxRoomPasswordLength> strRoomPassword;
	uint16_t nClientInfo;
};

//�ǳ�����
class CLogoutReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
};

class CGetUserListReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
};

class CSendTextReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	TextType nTextType;
	RoleID nDestID;
	CString<MaxTextMessageSize> strMessage;
	uint8_t nFontSize;
	uint32_t nFontColor;
	uint8_t nFontStyle;
	CString<MaxFontTypeStringSize> strFontType;
	uint8_t		nAutoAnswer;      //�Լ��Ƿ������Զ��ظ�
	uint8_t		nRejectAutoAnswer; //�Լ��Ƿ����ò�������
};

//��������
class CRequestShowReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RequestShowType nRequestShowType;
};

//��������
class CExitShowReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	CExitShowReq()
	{
		nExitShowType = enmExitShowType_Other;
	}
public:
	ExitShowType nExitShowType;

	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
};

//������������
class CPushOnShowReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestID;
	uint8_t nIndex;
};

//������������
class CPullOffShowReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestID;
};

//��������
class CKickUserReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	KickType nKickType;
	RoleID nKickRoleID;
	CString<MaxTextMessageSize> strReason;
	uint16_t nTime;
};

//�Ӻ�����
class CAddBlackReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;
};

//��������
class CShutUPReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;

};

//�����������
class CUnLockShutUPReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;

};

//��IP����
class CLockIpReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nDestRoleID;
	CString<MaxTextMessageSize> strReason;
	uint16_t nTime;
};

//���ӹ���Ȩ������
class CAddTitleReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleRank nAddRoleRank;
	RoleID nDestRoleID;
};

//ɾ������Ȩ������
class CDelTitleReqFromClient : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleRank nAddRoleRank;
	RoleID nDestRoleID;
};

//���÷�������
class CSetRoomReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	CSetRoomReq()
	{
		nSetCount = 0;
		memset(arrRoomInfoType,0,sizeof(arrRoomInfoType));
		nStringCount = 0;
		for(int32_t i = 0;i<MaxSetRoomTypeCount;i++)
		{
			arrSetString[i] = "";
		}
		nMicTime = 0;
		nRoomOptionType = 0;
	}
	uint8_t							nSetCount;
	RoomInfoType 					arrRoomInfoType[MaxSetRoomTypeCount];
	uint8_t							nStringCount;
	CString<MaxSetRoomTextSize> 	arrSetString[MaxSetRoomTypeCount];
	uint8_t	                        nMicTime;
	RoomOptionType                  nRoomOptionType;
};

//��ȡ�����������
class CGetWaittingListReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
};
//��ȡ������Ϣ����
class CGetRoomInfoReqFromClient : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
};

//�������״̬���򿪻��߹رգ�
class CSetMicStatusReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	MicStatus					nMicStatus;
	uint8_t						nMicIndex;
};
//�������״̬���򿪻��߹رգ�
class CPlayerHeartBeat : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID						nRoleID;
	uint8_t						nRoomCount;
	RoomID						arrRoomID[MaxEnterRoomCount];
};

//�������״̬���򿪻��߹رգ�
class CAnswerPullOnShowReq : public IMsgBody//, IDumpMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID						nInviteRoleID;
	AnswerType					nAnswerType;
	uint8_t						nMicIndex;
};

class CUpdateSongListReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RoomID						nRoomID;
	uint16_t					nSongCount;
	CString<MaxSongNameLength>	arrSongName[MaxSongCountPerPlayer];
	uint8_t						arrIsHot[MaxSongCountPerPlayer];
};

class CGetSongListReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RoomID						nRoomID;
	RoleID						nSongerRoleID;
};

class CRequestSongReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RoomID						nRoomID;
	RoleID						nSongerRoleID;
	CString<MaxSongNameLength>	strSongName;
	CString<MaxWishWordsLength>	strWishWords;
	uint16_t					nSongCount;
};

class CProcessOrderReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RoomID						nRoomID;
	uint32_t					nOrderID;
	uint8_t						nIsAccept;
};

class CGetOrderListReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RoomID						nRoomID;
};

class CFightForTicketReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
};

class CSongVoteReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	SongVoteResult				nSongVoteResult;
};


class CGetArtistListReq : public IMsgBody
{
	SIZE_INTERFACE();
public:
	RoomID  nRoomID;
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t nRet = S_OK;
		nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
		if(0 > nRet)
		{
			return nRet;
		}
		return S_OK;
	}
	int32_t MessageDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t nRet = S_OK;

		nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
		if(0 > nRet)
		{
			return nRet;
		}
		return S_OK;
	}
	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
		sprintf(buf + offset, "{nRoomID=%d}", nRoomID);
		offset = (uint32_t)strlen(buf);
	}
};

//add your message to map
MESSAGEMAP_DECL_BEGIN(from_client)

MESSAGEMAP_DECL_MSG(MSGID_CLRS_OPEN_PEIVATE_REQ, (new COpenPrivateMicReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_CLOSE_PEIVATE_REQ, (new CClosePrivateMicReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SENDP2P_REQ, (new CP2PMicReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_RECVP2P_REQ, (new CP2PAnswerReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_OPERATE_VIDEO_AUDIO_REQ, (new COperateVAReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_LOGIN_REQ, (new CLoginReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_LOGOUT_REQ, (new CLogoutReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_GETUSERLIST_REQ, (new CGetUserListReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SENDTEXT_REQ, (new CSendTextReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_REQUESTSHOW_REQ, (new CRequestShowReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_EXITSHOW_REQ, (new CExitShowReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_PUSH_ON_SHOW_REQ, (new CPushOnShowReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_PULL_OFF_SHOW_REQ, (new CPullOffShowReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_KICK_USER_REQ, (new CKickUserReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_ADD_BLACK_REQ, (new CAddBlackReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SHUT_UP_REQ, (new CShutUPReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_UNLOCK_SHUT_UP_REQ, (new CUnLockShutUPReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_LOCK_IP_REQ, (new CLockIpReq()))

//MESSAGEMAP_DECL_MSG(MSGID_CLRS_ADD_TITLE_REQ, (new CAddTitleReq()))

//MESSAGEMAP_DECL_MSG(MSGID_CLRS_DEL_TITLE_REQ, (new CDelTitleReqFromClient()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SET_ROOM_REQ, (new CSetRoomReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_GETWAITTINGLIST_REQ, (new CGetWaittingListReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_GET_ROOM_REQ, (new CGetRoomInfoReqFromClient()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SET_MIC_REQ, (new CSetMicStatusReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_HEARTBEAT, (new CPlayerHeartBeat()))

MESSAGEMAP_DECL_MSG(MSGID_RSCL_ANSWER_PULL_SHOW_REQ, (new CAnswerPullOnShowReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_UPDATESONGLIST_REQ, (new CUpdateSongListReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_GETSONGLIST_REQ, (new CGetSongListReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_REQUESTSONG_REQ, (new CRequestSongReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_PROCESSORDER_REQ, (new CProcessOrderReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_GETORDERLIST_REQ, (new CGetOrderListReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_FIGHTFORTICKET_REQ, (new CFightForTicketReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRS_SONGVOTE_REQ, (new CSongVoteReq()))

MESSAGEMAP_DECL_MSG(MSGID_CLRM_GETARTISTOFFLINE_REQ, (new CGetArtistListReq()))

MESSAGEMAP_DECL_END()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* FROM_CLIENT_MESSAGE_H_ */
