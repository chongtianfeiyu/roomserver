/*
 * dal_def.h
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#ifndef DAL_DEF_H_
#define DAL_DEF_H_

#include "server_namespace.h"
#include "../../lightframe/frame_typedef.h"
#include "common/common_list.h"
#include "common/common_string.h"
#include "public_typedef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define SERVER_NAME_STRING		"roomserver"
#define SERVER_VERSION			"Version: 1.0.build1, Release Date: 2011-12-12"

#define MaxTokenBuffSize		2048
//�Ự���Ͷ���
enum
{
	enmSessionType_Invalid = 0x00,
	enmSessionType_GetRoleInfo = 0x01, //��ȡ��ɫ��Ϣ
	enmSessionType_GetRoomInfo = 0x02, //��ȡ������Ϣ
	enmSessionType_GetMediaInfo = 0x03, //��ȡý���������Ϣ
	enmSessionType_SetTitle	    = 0x04, //���ù���Ȩ��
	enmSessionType_DelTitle     = 0x05, //ɾ������Ȩ��
	enmSessionType_SetRoom     	= 0x06, //���÷�������
	enmSessionType_GetEnterCount  = 0x07, //��ȡĳ�����ڽ��뷿�������
	enmSessionType_GetItemInfo  = 0x08, //��ȡ���item��Ϣ
	enmSessionType_AddToBlack  = 0x08, //�����ҵ�������

	enmSessionType_RebulidItemDaly = 0x09,

	enmSessionType_ReGetMediaDaly = 0x0a,
	enmSessionType_UpdateUserAsset = 0x0b,	//�û����۳��ʲ�
	enmSessionType_GetSongList = 0x0c,		//��ȡ�赥
	enmSessionType_ClearRoomOrderInfo = 0x0d,	//�����䶩����Ϣ
	enmSessionType_FightForTicket = 0x0e,	//��Ʊ��ʱ��
	enmSessionType_EnjoyProgram = 0x0f,		//���ͽ�Ŀ��ʱ��
	enmSessionType_Voteing = 0x10,			//���۶�ʱ��
	enmSessionType_ShowResult = 0x11,		//չʾͶƱ�����ʱ��
	enmSessionType_GetRoomArtist = 0x12,     //��ȡ���������б�
	enmSessionType_GetArtistInfo = 0x13,     //��ȡ������Ϣ
};

//��ʱ�����Ͷ���
enum
{

};
//�����˷��Ͳ����Ķ�ʱ����ʱʱ�������룩
#define RobotSendColorTimeOutInterval			2
//�����˷��Ͳ����Ķ�ʱ����ʱʱ�������룩
#define StaticPlayerTimeOutInterval			5*60
//��ʱ����ʱʱ�䶨��
enum
{
	enmUpdateInfoToHallPeriod				= 1 * US_PER_MINUTE,		//��hall�ϱ���Ϣ����
	enmGetRoleInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//��ȡ�û���Ϣ��ʱʱ��
	enmGetRoomInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//��ȡ������Ϣ��ʱʱ��
	enmGetMediaInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//��ȡý����Ϣ��ʱʱ��
	enmSetTitleTimeoutPeriod 				= 3 * US_PER_SECOND,			//���ù���Ȩ����Ϣ��ʱʱ��
	enmDelTitleTimeoutPeriod   				= 3 * US_PER_SECOND,			//ɾ������Ȩ����Ϣ��ʱʱ��
	enmSetRoomTimeoutPeriod   				= 3 * US_PER_SECOND,			//���÷�����Ϣ��ʱʱ��
	enmGetEnterCountTimeoutPeriod   		= 5 * US_PER_SECOND,			//��ȡĳ�����ڽ��뷿���������ʱʱ��
	enmSenColorTimeoutPeriod   				= RobotSendColorTimeOutInterval * US_PER_SECOND,			//���Ͳ���
	enmGetItemInfoTimeoutPeriod   			= 5 * US_PER_SECOND,			//��ȡ������Ϣ��ʱʱ��
	enmAddBlackTimeoutPeriod   				= 3 * US_PER_SECOND,			///�����ҵ���������ʱʱ��
	enmAddStaticPlayerTimeoutPeriod   		= StaticPlayerTimeOutInterval * US_PER_SECOND,			///ͳ������
	enmCheckPlayerTimeoutPeriod   			= 10 * US_PER_MINUTE,			//�������Ƿ����
	enmRebulidItemDelayPeriod   			= 100 * US_PER_MS,			//�ָ�item��delayʱ��
	enmReGetMediaDelayPeriod 				= 1 * US_PER_MINUTE,
	enmUpdateUserAssetTimeoutPeriod			= 5 * US_PER_SECOND,			//�û����۳��ʲ���ʱʱ��
	enmGetSongListTimeoutPeriod				= 5 * US_PER_SECOND,			//��ȡ�赥��ʱʱ��
	enmClearRoomOrderInfoPeriod				= 10 * US_PER_MINUTE,			//�������������ֶ�����Ϣ��ʱ��

};

typedef uint32_t LoginResult;
enum
{
	enmLoginResult_OK 						= 0x00,
	enmLoginResult_NotExist 				= 0x01,
	enmLoginResult_RoomFull 				= 0x02,
	enmLoginResult_BlackList 				= 0x03,
	enmLoginResult_Kicked 					= 0x04,
	enmLoginResult_NeedPassword 			= 0x05,
	enmLoginResult_PasswordWrong 			= 0x06,
	enmLoginResult_MaxRoomNum 				= 0x07,
	enmLoginResult_AlreadInRoom 			= 0x08,
	enmLoginResult_Timeout 					= 0x09,
	enmLoginResult_Other 					= 0x0a,
	enmLoginResult_IPLocked 				= 0x0b,
	enmLoginResult_RoomCloseed 				= 0x0c,
	enmLoginResult_RoomLocked 				= 0x0d,
	enmLoginResult_CreateRoomFailed 		= 0x0e,
	enmLoginResult_CreateSessionFailed 		= 0x10,
	enmLoginResult_DecodeConnUinFailed 		= 0x11,
	enmLoginResult_GetItemTimeout 			= 0x12,
	enmLoginResult_GetItemError 			= 0x13,
	enmLoginResult_OpenRoomToBig 			= 0x14,        //�û��򿪷��䳬����������������
	enmLoginResult_Unknown 					= 0xff,
};

//��Ӧ�ͻ��˵Ĵ򿪡��ر�����Ƶ
typedef uint32_t OperateResult;
enum
{
	OPERATERESULT_OK = 0x00,
	OPERATERESULT_NOT_ON_SHOW = 0x01,// ��������
	OPERATERESULT_UNKNOWN = 0xff,
};

#define GetRoomUserInfoCountPerOnce		100

#define MaxUserInfoCount 20
class RoomUserInfo
{
public:
	RoleID							nRoleID;
	Gender							nGender;
	uint32_t						n179ID;
	CString<MaxRoleNameLength>		strRoleName;
	VipLevel						nVipLevel;
	UserLevel						nUserLevel;
	RoleRank						nRoleRank;
	IdentityType                    nIdentityType;
	uint16_t						nClientInfo;
	uint16_t						nStatus;
	uint16_t						nItemCount;
	ItemID                          arrItemID[MaxOnUserItemCount];
	MagnateLevel 					nMagnateLevel;
//	uint16_t						nGiftStarCount;
//	ItemID                          arrGiftStarItemID[MaxGiftStarCount];

	void Dump(char* buf, const uint32_t size, uint32_t& offset)
	{
		sprintf(buf + offset, "stRoomUserInfo={nRoleID=%d,nGender=%d,n179ID=%d,strRoleName=%s,nVipLevel=%d,"
				"nUserLevel=%d,nRoleRank=%d,nIdentityType=%d,nClientInfo=%d,nStatus=%d,nItemCount=%d,arrItemID={",
				nRoleID, nGender, n179ID, strRoleName.GetString(),
				nVipLevel, nUserLevel, nRoleRank, nIdentityType,nClientInfo, nStatus,
				nItemCount);
		offset = (uint32_t)strlen(buf);

		for(int32_t i=0;i < nItemCount;i++)
		{
			if(i == 0)
			{
				sprintf(buf + offset, "{%d}", arrItemID[i]);
			}
			else
			{
				sprintf(buf + offset, "{,%d}", arrItemID[i]);
			}
			offset = (uint32_t)strlen(buf);
		}
		sprintf(buf + offset, "{,nMagnateLevel=%d}", nMagnateLevel);
		offset = (uint32_t)strlen(buf);
		sprintf(buf + offset, "}}");
		offset = (uint32_t)strlen(buf);
	}
	RoomUserInfo& operator=(const RoomUserInfo& ob)
	{
		nRoleID = ob.nRoleID;
		nGender = ob.nGender;
		n179ID  = ob.n179ID;
		strRoleName = ob.strRoleName;
		nVipLevel = ob.nVipLevel;
		nRoleRank = ob.nRoleRank;
		nIdentityType = ob.nIdentityType;
		nClientInfo = ob.nClientInfo;
		nStatus = ob.nStatus;
		nItemCount = ob.nItemCount;
		nMagnateLevel = ob.nMagnateLevel;
		return *this;
	}
};

typedef uint8_t	TextType;
enum
{
	enmTextType_Public				= 0x00,			//����
	enmTextType_Private				= 0x01,			//˽��
	enmTextType_Temp_Notice			= 0x02,			//��ʱ����
	enmTextType_Notice				= 0x03,			//����
};

typedef uint32_t TextResult;
enum
{
	enmTextResult_OK				= 0x00,
	enmTextResult_TooFast			= 0x01,			//������Ϣ̫��
	enmTextResult_PublicClosed		= 0x02,			//�����ر���
	enmTextResult_ColorClosed		= 0x03,			//�����ر���
	enmTextResult_InProhibited		= 0x04,			//�ڽ����б���
	enmTextResult_NoPermission		= 0x05,			//û��Ȩ��
	enmTextResult_NotInRoom			= 0x06,			//��Ҳ��ڴ˷���
	enmTextResult_Other				= 0x10,
	enmTextResult_VipCanPublicChat	= 0x11,			//vip���Ͽ��Թ���
	enmTextResult_RegCanPublicChat	= 0x12,			//ע���û����ϲſ��Թ���
	enmTextResult_Unknown			= 0xff,			//δ֪
};

typedef uint8_t RequestShowType;
enum
{
	enmRequestShowType_Any 		= 0x00, //����
	enmRequestShowType_Public 	= 0x01, //�Ź���
	enmRequestShowType_Private 	= 0x02, //��˽��
};

typedef uint32_t RequestShowResult;
enum
{
	enmRequestShowResult_Need_Select 				= 0x00, //��Ҫѡ������˽��
	enmRequestShowResult_Full 						= 0x01, //������
	enmRequestShowResult_Public 					= 0x02, //���Ϲ���+index+time
	enmRequestShowResult_Waittinglist 				= 0x03, //�����������+freeshow_opened
	enmRequestShowResult_Private 					= 0x04, //����˽��
	enmRequestShowResult_Private_Closed 			= 0x05, //����˽���ѹر�
	enmRequestShowResult_Fall 						= 0x06, //����ʧ��
	enmRequestShowResult_IsRowShowed 				= 0x07,  //�Ѿ��Ĺ���
	enmRequestShowResult_IsOnMicInOhterRoom 		= 0x08,  //��������������
	enmRequestShowResult_Wait_But_OnMic 			= 0x09,  //�ڱ������򵽹�����ʱ�򣬷���������������
	enmRequestShowResult_WaittingInOtherRoomMic  	= 0x0a //�����������������󣬱��Ž�����
};

typedef uint32_t ConnectP2PResult;
enum
{
	enmConnectP2PResult_OK					= 0x00,	 //�ȴ��Է�ͬ��
	enmConnectP2PResult_SELF_ON_PUBLIC		= 0x01,	 //�Լ��Ѿ��ڹ�����
	enmConnectP2PResult_SELF_ON_PRIVATE	    = 0x02,	 //�Լ��Ѿ���˽����
	enmConnectP2PResult_SELF_ON_P2P		    = 0x03,	//�Լ��Ѿ���v1��
	enmConnectP2PResult_OTHER_ON_PUBLIC	    = 0x04,	//�Է��Ѿ��ڹ�����
	enmConnectP2PResult_OTHER_ON_PRIVATE	= 0x05,	//�Է��Ѿ���˽����
	enmConnectP2PResult_OTHER_ON_P2P		= 0x06,	//�Է��Ѿ���v1��
	enmConnectP2PResult_OTHER_Need_Vip		= 0x07,	//�Է�ֻ����VIP���ϵ�
	enmConnectP2PResult_OTHER_Need_King		= 0x08,	//�Է�ֻ���ܻʹ����ϵ�
	enmConnectP2PResult_OTHER_Close			= 0x09,	//�Է��ر�һ��һ
	enmConnectP2PResult_SELF_NOT_VIP		= 0x10,	//�Լ�����vip
	enmConnectP2PResult_UNKNOWN			    = 0xff,
};
typedef uint32_t PushResult;
enum
{
	enmPushResult_OK					= 0x00,	//�����ɹ�
	enmPushResult_No_Permission			= 0x01,	//Ȩ�޲���
	enmPushResult_IS_ONMIC				= 0x02,	//�Ѿ�������
	enmPushResult_Mic_IsLocked			= 0x03,	//���󱻹�
	enmPushResult_Need_Answer			= 0x04,	//��ҪӦ��
	enmPushResult_UNKNOWN				= 0xff

};

typedef uint32_t PullResult;
enum
{
	enmPullResult_OK					= 0x00,	    	//�����ɹ�
	enmPullResult_No_Permission			= 0x01,	     	//Ȩ�޲���
	enmPullResult_IS_NOT_ONMIC			= 0x02,			//��������
	enmPullResult_UNKNOWN				= 0xff

};

typedef uint8_t KickType;
enum
{
	enmKickType_Other						= 0x00,
	enmKickType_KickVisitor					= 0x01,
	enmKickType_KickedForModifyCapacity		= 0x02,
};

typedef uint32_t KickResult;
enum
{
	enmKickResult_OK					= 0x00,
	enmKickResult_NO_Permission			= 0x01,	//Ȩ�޲���
	enmKickResult_NO_Permission_King	= 0x02,	//Ȩ�޲���
	enmKickResult_Umknown				= 0xff,
};

typedef uint32_t AddBlackResult;
enum
{
	enmAddBlackResult_OK			 		= 0x00,
	enmAddBlackResult_NO_Permission			= 0x01,	//Ȩ�޲���
	enmAddBlackResult_NO_Permission_King	= 0x02,	//Ȩ�޲���
	enmAddBlackResult_Umknown				= 0xff,
};

typedef uint32_t ShutUpResult;
enum
{
	enmShutUpResult_OK					= 0x00,
	enmShutUpResult_NO_Permission		= 0x01,	//Ȩ�޲���
	enmShutUpResult_NO_Permission_King	= 0x02,	//Ȩ�޲���
	enmShutUpResult_Umknown				= 0xff,
};

typedef uint32_t UnLockShutUpResult;
enum
{
	enmUnLockShutUpResult_OK				= 0x00,
	enmUnLockShutUpResult_NO_Permission		= 0x01,	//Ȩ�޲���
	enmUnLockShutUpResult_Umknown			= 0xff,
};

typedef uint32_t LockIpResult;
enum
{
	enmLockIpResult_OK					= 0x00,
	enmLockIpResult_NO_Permission		= 0x01,	//Ȩ�޲���
	enmLockIpResult_NO_Permission_King	= 0x02,	//Ȩ�޲���
	enmLockIpResult_Umknown				= 0xff,
};

typedef uint32_t OpenResult;

enum
{
	enmOpenResult_OK				= 0x00,
	enmOpenResult_NOT_OPEN			= 0x01,	//�Է�˽��û�д�
	enmOpenResult_NO_PERMISSION		= 0x02,	//û��Ȩ�ޣ�����ɫvip�����ϣ�
	enmOpenResult_Is_Full			= 0x03,	//�ﵽ���Դ򿪵�����
	enmOpenResult_Is_Opend			= 0x04,	//�Ѿ��򿪹���
	enmOpenResult_Is_Self			= 0x05,	//�򿪵�Ϊ�Լ�
	enmOpenResult_UNKNOWN			= 0xff,
};

typedef uint32_t ChangeResult;

enum
{
	enmChangeResult_OK				= 0x00,
	enmChangeResult_NO_PERMISSION	= 0x01,	//û��Ȩ��
	enmChangeResult_Is_Full			= 0x02,	//�����õĹ�������
	enmChangeResult_Is_AdminInOther	= 0x03,	//�������ķ����й���Ȩ��
	enmChangeResult_UNKNOWN			= 0xff,

};

typedef uint32_t SetRoomInfoResult;
enum
{
	enmSetRoomInfoResult_OK				= 0x00,
	enmSetRoomInfoResult_NO_PERMISSION	= 0x01,
	enmSetRoomInfoResult_UNKNOWN		= 0xff,
};

typedef uint8_t FontStyleType;
enum
{
	enmFontStyleType_NONE			= 0x00,
	enmFontStyleType_BOLD			= 0x01,	//����
	enmFontStyleType_UNDERLINE		= 0x02,	//�»���
	enmFontStyleType_ITALICS		= 0x04, 	//б��
	enmFontStyleType_COLORTEXT		= 0x08,	//����
};

typedef uint32_t SetMICResult;
enum
{
	enmSetMicResult_OK				= 0x00,
	enmSetMicResult_NO_PERMISSION	= 0x01,
	enmSetMicResult_UNKNOWN			= 0xff,
};

typedef uint8_t MicStatus;
enum
{
	enmMicStatus_Open		= 0x00,
	enmMicStatus_Close		= 0x01,
};
typedef uint8_t ExitShowType;
enum
{
	enmExitShowType_P2P		= 0x00,
	enmExitShowType_Other	= 0x01,
};

typedef uint8_t NotifyType;
enum
{
	enmNotifyType_NeedAnswer		=	0x00,	//��ҪӦ��
	enmNotifyType_Connect			=	0x01,	//�������
	enmNotifyType_Reject			=	0x02,	//�ܾ�
};

typedef uint32_t AnswerPullResult;
enum
{
	enmAnswerPullResult_OK 				= 0x00, //�����ɹ�
	enmAnswerPullResult_IS_ONMIC 		= 0x01, //�Ѿ�������
	enmAnswerPullResult_MIC_ISLOCKED 	= 0x02, //���󱻹�
	enmAnswerPullResult_MIC_HASPLAYER 	= 0x03, //����������
	enmAnswerPullResult_UNKNOWN 		= 0xff,
};
typedef uint8_t AnswerType;
enum
{
	enmAnswerType_AGREE 		= 0x00, //ͬ��
	enmAnswerType_REJECT 		= 0x01, //�ܾ�
};

//��������������Ķ�������
#define MaxOrderListLength		200

typedef uint32_t	UpdateSongListResult;
enum
{
	enmUpdateSongListResult_OK				= 0x00,		//�ɹ�
	enmUpdateSongListResult_Full			= 0x01,   	//�赥����
	enmUpdateSongListResult_HotFull			= 0x02,   	//���Ƹ�����
	enmUpdateSongListResult_NoPermission	= 0x03,	 	//û��Ȩ��(��������..)
	enmUpdateSongListResult_Unknown			= 0xff,
};

typedef uint32_t	RequestSongResult;
enum
{
	enmRequestSongResult_OK					= 0x00,		//�ɹ�
	enmRequestSongResult_MoneyNotEnough		= 0x01,		//Ǯ����
	enmRequestSongResult_NoPermission		= 0x02,	 	//û��Ȩ��(��������..)
	enmRequestSongResult_Timeout			= 0x03,		//��ʱ
	enmRequestSongResult_Unknown			= 0x04,		//δ֪����
};

typedef uint32_t	FightForTicketResult;
enum
{
	enmFightForTicketResult_OK					= 0x00,		//�ɹ�
	enmFightForTicketResult_NoLeftTicket		= 0x01,		//û��Ʊ��
	enmFightForTicketResult_NoPermission		= 0x02,	 	//û��Ȩ��(����������ߣ�����������������Ʊ)
	enmFightForTicketResult_Timeout				= 0x03,		//��ʱ
	enmFightForTicketResult_Unknown				= 0x04,		//δ֪����
};

enum
{
	enmOffline_stat                              = 0x00,      //������
	enmOnline_stat                               = 0x01,      //����
};
FRAME_ROOMSERVER_NAMESPACE_END

#endif /* DAL_DEF_H_ */
