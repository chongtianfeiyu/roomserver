/*
 * to_client_message.h
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#ifndef TO_CLIENT_MESSAGE_H_
#define TO_CLIENT_MESSAGE_H_

#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "../../lightframe/frame_msg_impl.h"
#include "common/common_string.h"
#include "def/dal_def.h"
#include "public_typedef.h"
#include "def/def_message_id.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CLoginResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLoginResp()
	{
		nResult = enmLoginResult_OK;
		strFailReason = "unknown error";
		nKickedLeftTime = 0;
		strRoomName="";
		strRoomSignature="";
		nTelMediaIP=0;
		nTelMediaPort=0;
		nCncMediaIP=0;
		nCncMediaPort=0;
		strHostName="";
		nHost179ID=0;
		nHostGender = enmGender_Unisex;
		nKey = 0;
		nPublicMicCount = 0;
		memset(stRoomUserInfo,0,sizeof(RoomUserInfo)*MaxPublicMicCount);
	}
public:
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	LoginResult						nResult;
	CString<MaxReqFailReasonLength>		strFailReason;
	uint16_t						nKickedLeftTime;//当nResult = enmLoginResult_MaxRoomNum 为可以进入的最大房间数量,locked时为被封时间
	CString<MaxRoomNameLength>		strRoomName;
	CString<MaxRoomSignatureSize>	strRoomSignature;
	uint32_t						nTelMediaIP;
	uint16_t						nTelMediaPort;
	uint32_t						nCncMediaIP;
	uint16_t						nCncMediaPort;
	CString<enmMaxRoleNameLength> 	strHostName; //室主的昵称
	uint32_t 						nHost179ID;   //室主179ID
	Gender 							nHostGender; //室主性别
	uint32_t 						nKey;		//玩家进房得到的key
	uint8_t							nPublicMicCount; //公卖数量
	RoomUserInfo					stRoomUserInfo[MaxPublicMicCount];//公卖玩家信息
	uint8_t							nIsCollected;		//是否收藏
};

class CLoginNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLoginNoti()
	{
		memset(&stRoomUserInfo,0,sizeof(stRoomUserInfo));
	}
public:
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoomUserInfo					stRoomUserInfo;
};

class CLogoutNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLogoutNoti()
	{
		nRoleID = enmInvalidRoleID;
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID							nRoleID;
};

//回应关闭或者打开某人的音视频
class COperateVAResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	COperateVAResp()
	{
		nOperateResult = OPERATERESULT_OK;
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	OperateResult nOperateResult;
};

//关闭或者打开某人的音视频通知
class COperateVANotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	COperateVANotify()
	{
		nOperateAVType = OPERATEAVTYPE_OPEN_VIDEO;
		nSrcRoleID = enmInvalidRoleID;
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	OperateAVType 		nOperateAVType;
	RoleID	  			nSrcRoleID;
};


class CGetUserListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CGetUserListResp()
	{
		nResult = 0;
		strFailReason = "";
		nEndFlag = 0;
		nCount = 0;
//		memset(arrRoomUserInfo,0,sizeof(arrRoomUserInfo));
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	int32_t								nResult;
	CString<MaxReqFailReasonLength> 	strFailReason;
	uint8_t								nEndFlag;
	uint8_t								nCount;
	RoomUserInfo						arrRoomUserInfo[GetRoomUserInfoCountPerOnce];
};

class CSendTextResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSendTextResp()
	{
		nResult = enmTextResult_OK;
		strFailReason = "";
		nLeftTime = 0;
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	int32_t							nResult;
	CString<MaxReqFailReasonLength> strFailReason;
	uint32_t 						nLeftTime;
};

class CSendTextNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSendTextNoti()
	{
		nTextType = enmTextType_Public;
		nSrcID = enmInvalidRoleID;
		nDestID = enmInvalidRoleID;
		strMessage = "";
		nFontSize = 0;
		nFontColor = 0;
		nFontStyle = 0;
		strFontType = "";
		nAutoAnswer = 0;
		nRejectAutoAnswer = 0;
	}
	int32_t		MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t		MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void		Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	TextType					nTextType;
	RoleID						nSrcID;
	RoleID						nDestID;
	CString<MaxTextMessageSize>	strMessage;
	uint8_t						nFontSize;
	uint32_t					nFontColor;
	uint8_t						nFontStyle;
	CString<MaxFontTypeStringSize>	strFontType;
	uint8_t						nAutoAnswer;      //自己是否设置自动回复
	uint8_t						nRejectAutoAnswer; //自己是否设置不再提醒
};

class CRequestShowResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CRequestShowResp()
	{
		nRequestShowResult = enmRequestShowResult_Full;
		nIndex = 0;
		nTime = 0;
		nFreeShowOpened = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RequestShowResult nRequestShowResult;
	uint8_t nIndex;
	uint16_t nTime;
	uint8_t nFreeShowOpened;

};

class CRequestShowNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CRequestShowNoti()
	{
		nRequestShowResult = 0;
		nSrcRoleID = enmInvalidRoleID;
		nIndex = 0;
		nTime = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RequestShowResult nRequestShowResult;
	RoleID nSrcRoleID;
	uint8_t nIndex;
	uint16_t nTime;
};

class CExitShowNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CExitShowNoti()
	{
		nSrcRoleID = 0;
		nExitShowType = enmExitShowType_Other;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	ExitShowType nExitShowType;

};

class CP2PResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CP2PResp()
	{
		nConnectP2PResult = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ConnectP2PResult nConnectP2PResult;
};

class CP2PRequestNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CP2PRequestNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
};

class CP2PAnswerNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CP2PAnswerNoti()
	{
		nResponseP2PType = enmRESPONSEP2PTYPE_ACCEPT;
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ResponseP2PType 	nResponseP2PType;
	RoleID 				nSrcRoleID;      //发起答复的玩家
	RoleID 				nDestRoleID;     //被答复的玩家
};

class CPushOnShowResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CPushOnShowResp()
	{
		nPushResult = enmPushResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	PushResult nPushResult;
};

class CPushOnShowNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CPushOnShowNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
		nNotifyType = enmNotifyType_Connect;
		nIndex = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
	NotifyType nNotifyType;
	uint8_t nIndex;
};

class CPullOffShowResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CPullOffShowResp()
	{
		nPullResult = enmPullResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	PullResult nPullResult;
};

class CPullOffShowNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CPullOffShowNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
};

class CKickUserResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CKickUserResp()
	{
		nKickResult = enmKickResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	KickResult 	nKickResult;
};

class CKickUserNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CKickUserNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nKickAllVisitor = enmKickType_Other;
		nKickRoleID = enmInvalidRoleID;
		strReason = "";
		nTime = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID 		nSrcRoleID;
	KickType	nKickAllVisitor; //为1的时候踢出所有游客
	RoleID 		nKickRoleID;
	CString<MaxTextMessageSize>	strReason;
	uint16_t					nTime;
};

class CAddBlackResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CAddBlackResp()
	{
		nAddBlackResult = enmAddBlackResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	AddBlackResult nAddBlackResult;
};

class CAddBlackNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CAddBlackNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
};

class CShutUpResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CShutUpResp()
	{
		nShutUpResult = enmShutUpResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ShutUpResult nShutUpResult;
};

class CShutUpNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CShutUpNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
		nLeftTime = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
	uint32_t nLeftTime;
};

class CUnLockShutUpResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CUnLockShutUpResp()
	{
		nUnLockShutUpResult = enmUnLockShutUpResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	UnLockShutUpResult nUnLockShutUpResult;
};

class CUnLockShutUpNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CUnLockShutUpNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
};

class CLockIpResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLockIpResp()
	{
		nLockIpResult = enmLockIpResult_OK;;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	LockIpResult nLockIpResult;
};

class CLockIpNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLockIpNoti()
	{
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
		strReason = "";
		nTime = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleID nSrcRoleID;
	RoleID nDestRoleID;
	CString<MaxTextMessageSize> strReason;
	uint16_t nTime;
};

class COpenPeivateResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	COpenPeivateResp()
	{
		nOpenResult = enmOpenResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	OpenResult nOpenResult;
};

class CSrvPrivateMicNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSrvPrivateMicNotify()
	{
		nCount = 0;
		memset(arrPrivateMicRoleID,0,sizeof(arrPrivateMicRoleID));
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	uint16_t  nCount;
	RoleID    arrPrivateMicRoleID[MaxWatchPrivateMicCount];
};

class CAddTitleResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CAddTitleResp()
	{
		nChangeResult = enmChangeResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ChangeResult nChangeResult;
};

class CAddtitleNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CAddtitleNotify()
	{
		nAddRoleRank = enmRoleRank_None;
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleRank  nAddRoleRank;
	RoleID    nSrcRoleID;
	RoleID    nDestRoleID;
};

class CDelTitleRespToClient : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CDelTitleRespToClient()
	{
		nChangeResult = enmChangeResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	ChangeResult nChangeResult;
};

class CDeltitleNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CDeltitleNotify()
	{
		nAddRoleRank = enmRoleRank_None;
		nSrcRoleID = enmInvalidRoleID;
		nDestRoleID = enmInvalidRoleID;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	RoleRank  nAddRoleRank;
	RoleID    nSrcRoleID;
	RoleID    nDestRoleID;
};

class CSetRoomResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSetRoomResp()
	{
		nSetRoomInfoResult = enmSetRoomInfoResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	SetRoomInfoResult nSetRoomInfoResult;
};

class CSetRoomNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSetRoomNotify()
	{
		nSrcRoleID = enmInvalidRoleID;
		nSetCount = 0;
		memset(arrRoomInfoType,0,sizeof(arrRoomInfoType));
		nStringCount = 0;
		for(int32_t i = 0;i<MaxSetRoomTypeCount;i++)
		{
			arrSetString[i] = "";
		}
		nMicTime=0;
		nRoomOptionType = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:

	RoleID    						nSrcRoleID;
	uint8_t							nSetCount;
	RoomInfoType 					arrRoomInfoType[MaxSetRoomTypeCount];
	uint8_t							nStringCount;
	CString<MaxSetRoomTextSize> 	arrSetString[MaxSetRoomTypeCount];
	uint8_t	                        nMicTime;
	RoomOptionType                  nRoomOptionType;
};

class CGetWaittingListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CGetWaittingListResp()
	{
		nWaittingCount = 0;
		for(int32_t i = 0;i<MaxWaittingMicCount;i++)
		{
			arrWaittingList[i] = enmInvalidRoleID;
		}
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	uint8_t 		nWaittingCount;
	RoleID			arrWaittingList[MaxWaittingMicCount];
};

class CGetRoomInfoRespToClient : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CGetRoomInfoRespToClient()
	{
		nGetCount = 0;
		memset(arrRoomInfoType,0,sizeof(arrRoomInfoType));
		nStringCount = 0;
		for(int32_t i = 0;i<MaxGetRoomTypeCount;i++)
		{
			arrSetString[i] = "";
		}
		nMicTime = 0;
		nRoomOptionType = 0;
		nFirstMicStatus = enmMicStatus_Open;
		nSecondMicStatus = enmMicStatus_Open;
		nThirdMicStatus = enmMicStatus_Open;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	uint8_t							nGetCount;
	RoomInfoType 					arrRoomInfoType[MaxGetRoomTypeCount];
	uint8_t							nStringCount;
	CString<MaxSetRoomTextSize> 	arrSetString[MaxGetRoomTypeCount];
	uint8_t	                        nMicTime;
	RoomOptionType                  nRoomOptionType;
	MicStatus 						nFirstMicStatus;
	MicStatus						nSecondMicStatus;
	MicStatus						nThirdMicStatus;
};

class CLockRoomNotifyToClient : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CLockRoomNotifyToClient()
	{
		strReason = "";
		nLockDays = 0;
		nEndTime = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	CString<MaxTextMessageSize> 	strReason;
	uint16_t						nLockDays;
	uint32_t						nEndTime;

};

class CSetMicResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSetMicResp()
	{
		nSetMICResult = enmSetMicResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	SetMICResult nSetMICResult;
};

class CSetMicNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CSetMicNotify()
	{
		nSrcRoleID = enmInvalidRoleID;
		nMicStatus = enmMicStatus_Open;
		nMicIndex = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:

	RoleID    						nSrcRoleID;
	MicStatus 						nMicStatus;
	uint8_t							nMicIndex;

};

class CReSetMediaNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CReSetMediaNotify()
	{
		nRoomID = enmInvalidRoomID;
		nTelMediaIP = 0;
		nTelMediaPort = 0;
		nCncMediaIP = 0;
		nCncMediaPort = 0;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:

	RoomID    						nRoomID;
	uint32_t						nTelMediaIP;
	uint16_t						nTelMediaPort;
	uint32_t						nCncMediaIP;
	uint16_t						nCncMediaPort;

};

class CUpdateUserInfoNotify : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CUpdateUserInfoNotify()
	{
		nRoleID = enmInvalidRoleID;
		nUpdateCount = 0;
		for(int32_t i = 0;i<MaxUserInfoCount;i++)
		{
			nType[i] = enmUserInfoType_Invalid;
		}
		memset(&stRoomUserInfo,0,sizeof(stRoomUserInfo));
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:

	RoleID    						nRoleID;
	uint8_t							nUpdateCount;
	UserInfoType					nType[MaxUserInfoCount];
	RoomUserInfo					stRoomUserInfo;
};

class CAnswerPullOnShowResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	CAnswerPullOnShowResp()
	{
		nAnswerPullResult = enmAnswerPullResult_OK;
	}
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);
public:
	AnswerPullResult nAnswerPullResult;
};

class CUpdateSongListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	UpdateSongListResult		nResult;
	uint16_t					nSongCount;
	CString<MaxSongNameLength>	arrSongName[MaxSongCountPerPlayer];
	uint8_t						arrIsHot[MaxSongCountPerPlayer];
};

class CGetSongListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nResult;
	RoleID						nSongerRoleID;
	uint16_t					nSongCount;
	CString<MaxSongNameLength>	arrSongName[MaxSongCountPerPlayer];
	uint8_t						arrIsHot[MaxSongCountPerPlayer];
};

class CRequestSongResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	RequestSongResult			nResult;
};

class CRequestSongNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	uint32_t					nOrderTime;
	RoleID						nSongerRoleID;
	CString<MaxRoleNameLength>	strSongerName;		//歌手昵称
	RoleID						nRequestRoleID;		//点歌者roleid
	CString<MaxRoleNameLength>	strRequestName;		//点歌玩家昵称
	CString<MaxSongNameLength>	strSongName;
	CString<MaxWishWordsLength>	strWishWords;
	uint16_t					nSongCount;
};

class CProcessOrderResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nResult;
};

class CProcessOrderNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	uint8_t						nIsAccept;
	RoleID						nSongerRoleID;
	CString<MaxRoleNameLength>	strSongerName;		//歌手昵称
	RoleID						nRequestRoleID;		//点歌者roleid
	CString<MaxRoleNameLength>	strRequestName;		//点歌玩家昵称
	CString<MaxSongNameLength>	strSongName;
	CString<MaxWishWordsLength>	strWishWords;
	uint16_t					nSongCount;
};

class CGetOrderListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nResult;
	uint16_t					nOrderCount;
	uint32_t					arrOrderID[MaxOrderListLength];
	uint32_t					arrOrderTime[MaxOrderListLength];
	RoleID						arrSongerRoleID[MaxOrderListLength];
	CString<MaxRoleNameLength>	arrSongerName[MaxOrderListLength];		//歌手昵称
	RoleID						arrRequestRoleID[MaxOrderListLength];	//点歌者roleid
	CString<MaxRoleNameLength>	arrRequestName[MaxOrderListLength];		//点歌玩家昵称
	CString<MaxSongNameLength>	arrSongName[MaxOrderListLength];
	CString<MaxWishWordsLength>	arrWishWords[MaxOrderListLength];
	OrderStatus					arrOrderStatus[MaxOrderListLength];
	uint16_t					arrSongCount[MaxOrderListLength];
};

class CCurSongInfoNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	RoleID						nSongerRoleID;
	CString<MaxRoleNameLength>	strSongerName;		//歌手昵称
	RoleID						nRequestRoleID;		//点歌者roleid
	CString<MaxRoleNameLength>	strRequestName;		//点歌玩家昵称
	CString<MaxSongNameLength>	strSongName;
};

class CTicketCountUpdateNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	uint16_t					nTotalTicketCount;
	uint16_t					nLeftTicketCount;
	uint8_t						nHaveTicket;
	uint8_t						nCanGetTicket;
	uint32_t					nEndLeftTime;
};

class CFightForTicketResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	FightForTicketResult				nResult;
};

class CEnjoyingNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	uint8_t						nHaveTicket;
};

class CSongVoteResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nResult;
};

class CSongVoteUpdateNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
	RoleID						nSongerRoleID;		//歌手roleid
	CString<MaxRoleNameLength>	strSongerName;		//歌手昵称
	RoleID						nRequestRoleID;		//点歌者roleid
	CString<MaxRoleNameLength>	strRequestName;		//点歌玩家昵称
	CString<MaxSongNameLength>	strSongName;		//点的歌曲名
	SongVoteResult				nSongVoteResult;	//投票结果
};

class CWaitingForSongNoti : public IMsgBody
{
	SIZE_INTERFACE();
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset);
	int32_t MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset);
	void Dump(char* buf, const uint32_t size, uint32_t& offset);

public:
	uint32_t					nOrderID;
};


class CGetArtistListResp : public IMsgBody
{
	SIZE_INTERFACE();
public:
	uint16_t      nCount;              //主播列表人数
	RoomUserInfo stRoomUserInfo[MAX_ARTIST_NUM];
public:
	int32_t MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		int32_t nRet = S_OK;
		nRet = CCodeEngine::Encode(buf, size, offset, nCount);
		if(0 > nRet)
		{
			return nRet;
		}
		for(int32_t i = 0 ; i < nCount; ++i)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nRoleID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nGender);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].n179ID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = stRoomUserInfo[i].strRoleName.MessageEncode(buf, size, offset);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nVipLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nUserLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nRoleRank);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nIdentityType);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nClientInfo);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nStatus);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nItemCount);
			if (0 > nRet)
			{
				return nRet;
			}
			if(stRoomUserInfo[i].nItemCount>MaxOnUserItemCount)
			{
				return ENCODE_DECODE_FILED;
			}
			for(int32_t j=0;j<stRoomUserInfo[i].nItemCount;j++)
			{
				nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].arrItemID[j]);
				if (0 > nRet)
				{
					return nRet;
				}
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nMagnateLevel);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		return S_OK;
	}
	int32_t MessageDecode(const uint8_t* buf, const uint32_t size, uint32_t& offset)
	{
		return S_OK;
	}

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
		sprintf(buf + offset, "{nCount=%d, ", nCount);
		offset = (uint32_t)strlen(buf);
		for(int32_t i = 0; i < nCount; ++i)
		{
			stRoomUserInfo[i].Dump(buf, size, offset);
			offset = (uint32_t)strlen(buf);
		}
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
};

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* TO_CLIENT_MESSAGE_H_ */
