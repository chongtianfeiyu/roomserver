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
//会话类型定义
enum
{
	enmSessionType_Invalid = 0x00,
	enmSessionType_GetRoleInfo = 0x01, //获取角色信息
	enmSessionType_GetRoomInfo = 0x02, //获取房间信息
	enmSessionType_GetMediaInfo = 0x03, //获取媒体服务器信息
	enmSessionType_SetTitle	    = 0x04, //设置管理权限
	enmSessionType_DelTitle     = 0x05, //删除管理权限
	enmSessionType_SetRoom     	= 0x06, //设置房间属性
	enmSessionType_GetEnterCount  = 0x07, //获取某人现在进入房间的数量
	enmSessionType_GetItemInfo  = 0x08, //获取玩家item信息
	enmSessionType_AddToBlack  = 0x08, //添加玩家到黑名单

	enmSessionType_RebulidItemDaly = 0x09,

	enmSessionType_ReGetMediaDaly = 0x0a,
	enmSessionType_UpdateUserAsset = 0x0b,	//用户点歌扣除资产
	enmSessionType_GetSongList = 0x0c,		//获取歌单
	enmSessionType_ClearRoomOrderInfo = 0x0d,	//清理房间订单信息
	enmSessionType_FightForTicket = 0x0e,	//抢票定时器
	enmSessionType_EnjoyProgram = 0x0f,		//欣赏节目定时器
	enmSessionType_Voteing = 0x10,			//评价定时器
	enmSessionType_ShowResult = 0x11,		//展示投票结果定时器
	enmSessionType_GetRoomArtist = 0x12,     //获取房间主播列表
	enmSessionType_GetArtistInfo = 0x13,     //获取主播信息
};

//定时器类型定义
enum
{

};
//机器人发送彩条的定时器超时时间间隔（秒）
#define RobotSendColorTimeOutInterval			2
//机器人发送彩条的定时器超时时间间隔（秒）
#define StaticPlayerTimeOutInterval			5*60
//定时器超时时间定义
enum
{
	enmUpdateInfoToHallPeriod				= 1 * US_PER_MINUTE,		//向hall上报信息周期
	enmGetRoleInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//获取用户信息超时时间
	enmGetRoomInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//获取房间信息超时时间
	enmGetMediaInfoTimeoutPeriod 			= 5 * US_PER_SECOND,			//获取媒体信息超时时间
	enmSetTitleTimeoutPeriod 				= 3 * US_PER_SECOND,			//设置管理权限信息超时时间
	enmDelTitleTimeoutPeriod   				= 3 * US_PER_SECOND,			//删除管理权限信息超时时间
	enmSetRoomTimeoutPeriod   				= 3 * US_PER_SECOND,			//设置房间信息超时时间
	enmGetEnterCountTimeoutPeriod   		= 5 * US_PER_SECOND,			//获取某人现在进入房间的数量超时时间
	enmSenColorTimeoutPeriod   				= RobotSendColorTimeOutInterval * US_PER_SECOND,			//发送彩条
	enmGetItemInfoTimeoutPeriod   			= 5 * US_PER_SECOND,			//获取道具信息超时时间
	enmAddBlackTimeoutPeriod   				= 3 * US_PER_SECOND,			///添加玩家到黑名单超时时间
	enmAddStaticPlayerTimeoutPeriod   		= StaticPlayerTimeOutInterval * US_PER_SECOND,			///统计人数
	enmCheckPlayerTimeoutPeriod   			= 10 * US_PER_MINUTE,			//检查玩家是否掉线
	enmRebulidItemDelayPeriod   			= 100 * US_PER_MS,			//恢复item的delay时间
	enmReGetMediaDelayPeriod 				= 1 * US_PER_MINUTE,
	enmUpdateUserAssetTimeoutPeriod			= 5 * US_PER_SECOND,			//用户点歌扣除资产超时时间
	enmGetSongListTimeoutPeriod				= 5 * US_PER_SECOND,			//获取歌单超时时间
	enmClearRoomOrderInfoPeriod				= 10 * US_PER_MINUTE,			//清除房间下麦歌手订单信息的时间

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
	enmLoginResult_OpenRoomToBig 			= 0x14,        //用户打开房间超过服务器最大的限制
	enmLoginResult_Unknown 					= 0xff,
};

//回应客户端的打开、关闭音视频
typedef uint32_t OperateResult;
enum
{
	OPERATERESULT_OK = 0x00,
	OPERATERESULT_NOT_ON_SHOW = 0x01,// 不在麦上
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
	enmTextType_Public				= 0x00,			//公屏
	enmTextType_Private				= 0x01,			//私屏
	enmTextType_Temp_Notice			= 0x02,			//临时公告
	enmTextType_Notice				= 0x03,			//公告
};

typedef uint32_t TextResult;
enum
{
	enmTextResult_OK				= 0x00,
	enmTextResult_TooFast			= 0x01,			//发送消息太快
	enmTextResult_PublicClosed		= 0x02,			//公屏关闭了
	enmTextResult_ColorClosed		= 0x03,			//彩条关闭了
	enmTextResult_InProhibited		= 0x04,			//在禁言列表中
	enmTextResult_NoPermission		= 0x05,			//没有权限
	enmTextResult_NotInRoom			= 0x06,			//玩家不在此房间
	enmTextResult_Other				= 0x10,
	enmTextResult_VipCanPublicChat	= 0x11,			//vip以上可以公聊
	enmTextResult_RegCanPublicChat	= 0x12,			//注册用户以上才可以公聊
	enmTextResult_Unknown			= 0xff,			//未知
};

typedef uint8_t RequestShowType;
enum
{
	enmRequestShowType_Any 		= 0x00, //排麦
	enmRequestShowType_Public 	= 0x01, //排公麦
	enmRequestShowType_Private 	= 0x02, //排私麦
};

typedef uint32_t RequestShowResult;
enum
{
	enmRequestShowResult_Need_Select 				= 0x00, //需要选择公麦还是私麦
	enmRequestShowResult_Full 						= 0x01, //麦已满
	enmRequestShowResult_Public 					= 0x02, //排上公麦+index+time
	enmRequestShowResult_Waittinglist 				= 0x03, //进入排麦队列+freeshow_opened
	enmRequestShowResult_Private 					= 0x04, //排上私麦
	enmRequestShowResult_Private_Closed 			= 0x05, //房间私麦已关闭
	enmRequestShowResult_Fall 						= 0x06, //排麦失败
	enmRequestShowResult_IsRowShowed 				= 0x07,  //已经拍过卖
	enmRequestShowResult_IsOnMicInOhterRoom 		= 0x08,  //在其他房间上麦
	enmRequestShowResult_Wait_But_OnMic 			= 0x09,  //在被从麦序到公卖的时候，发现在其他的麦上
	enmRequestShowResult_WaittingInOtherRoomMic  	= 0x0a //由于在其他房间上麦，被排进麦序
};

typedef uint32_t ConnectP2PResult;
enum
{
	enmConnectP2PResult_OK					= 0x00,	 //等待对方同意
	enmConnectP2PResult_SELF_ON_PUBLIC		= 0x01,	 //自己已经在公麦中
	enmConnectP2PResult_SELF_ON_PRIVATE	    = 0x02,	 //自己已经在私麦中
	enmConnectP2PResult_SELF_ON_P2P		    = 0x03,	//自己已经在v1中
	enmConnectP2PResult_OTHER_ON_PUBLIC	    = 0x04,	//对方已经在公麦中
	enmConnectP2PResult_OTHER_ON_PRIVATE	= 0x05,	//对方已经在私麦中
	enmConnectP2PResult_OTHER_ON_P2P		= 0x06,	//对方已经在v1中
	enmConnectP2PResult_OTHER_Need_Vip		= 0x07,	//对方只接受VIP以上的
	enmConnectP2PResult_OTHER_Need_King		= 0x08,	//对方只接受皇冠以上的
	enmConnectP2PResult_OTHER_Close			= 0x09,	//对方关闭一对一
	enmConnectP2PResult_SELF_NOT_VIP		= 0x10,	//自己不是vip
	enmConnectP2PResult_UNKNOWN			    = 0xff,
};
typedef uint32_t PushResult;
enum
{
	enmPushResult_OK					= 0x00,	//操作成功
	enmPushResult_No_Permission			= 0x01,	//权限不足
	enmPushResult_IS_ONMIC				= 0x02,	//已经在麦上
	enmPushResult_Mic_IsLocked			= 0x03,	//此麦被关
	enmPushResult_Need_Answer			= 0x04,	//需要应答
	enmPushResult_UNKNOWN				= 0xff

};

typedef uint32_t PullResult;
enum
{
	enmPullResult_OK					= 0x00,	    	//操作成功
	enmPullResult_No_Permission			= 0x01,	     	//权限不足
	enmPullResult_IS_NOT_ONMIC			= 0x02,			//不在麦上
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
	enmKickResult_NO_Permission			= 0x01,	//权限不足
	enmKickResult_NO_Permission_King	= 0x02,	//权限不足
	enmKickResult_Umknown				= 0xff,
};

typedef uint32_t AddBlackResult;
enum
{
	enmAddBlackResult_OK			 		= 0x00,
	enmAddBlackResult_NO_Permission			= 0x01,	//权限不足
	enmAddBlackResult_NO_Permission_King	= 0x02,	//权限不足
	enmAddBlackResult_Umknown				= 0xff,
};

typedef uint32_t ShutUpResult;
enum
{
	enmShutUpResult_OK					= 0x00,
	enmShutUpResult_NO_Permission		= 0x01,	//权限不足
	enmShutUpResult_NO_Permission_King	= 0x02,	//权限不足
	enmShutUpResult_Umknown				= 0xff,
};

typedef uint32_t UnLockShutUpResult;
enum
{
	enmUnLockShutUpResult_OK				= 0x00,
	enmUnLockShutUpResult_NO_Permission		= 0x01,	//权限不足
	enmUnLockShutUpResult_Umknown			= 0xff,
};

typedef uint32_t LockIpResult;
enum
{
	enmLockIpResult_OK					= 0x00,
	enmLockIpResult_NO_Permission		= 0x01,	//权限不足
	enmLockIpResult_NO_Permission_King	= 0x02,	//权限不足
	enmLockIpResult_Umknown				= 0xff,
};

typedef uint32_t OpenResult;

enum
{
	enmOpenResult_OK				= 0x00,
	enmOpenResult_NOT_OPEN			= 0x01,	//对方私麦没有打开
	enmOpenResult_NO_PERMISSION		= 0x02,	//没有权限（非紫色vip及以上）
	enmOpenResult_Is_Full			= 0x03,	//达到可以打开的上限
	enmOpenResult_Is_Opend			= 0x04,	//已经打开过了
	enmOpenResult_Is_Self			= 0x05,	//打开的为自己
	enmOpenResult_UNKNOWN			= 0xff,
};

typedef uint32_t ChangeResult;

enum
{
	enmChangeResult_OK				= 0x00,
	enmChangeResult_NO_PERMISSION	= 0x01,	//没有权限
	enmChangeResult_Is_Full			= 0x02,	//被设置的管理已满
	enmChangeResult_Is_AdminInOther	= 0x03,	//在其他的房间有管理权限
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
	enmFontStyleType_BOLD			= 0x01,	//粗体
	enmFontStyleType_UNDERLINE		= 0x02,	//下划线
	enmFontStyleType_ITALICS		= 0x04, 	//斜体
	enmFontStyleType_COLORTEXT		= 0x08,	//彩条
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
	enmNotifyType_NeedAnswer		=	0x00,	//需要应答
	enmNotifyType_Connect			=	0x01,	//完成连接
	enmNotifyType_Reject			=	0x02,	//拒绝
};

typedef uint32_t AnswerPullResult;
enum
{
	enmAnswerPullResult_OK 				= 0x00, //操作成功
	enmAnswerPullResult_IS_ONMIC 		= 0x01, //已经在麦上
	enmAnswerPullResult_MIC_ISLOCKED 	= 0x02, //此麦被关
	enmAnswerPullResult_MIC_HASPLAYER 	= 0x03, //此麦上有人
	enmAnswerPullResult_UNKNOWN 		= 0xff,
};
typedef uint8_t AnswerType;
enum
{
	enmAnswerType_AGREE 		= 0x00, //同意
	enmAnswerType_REJECT 		= 0x01, //拒绝
};

//房间所保存的最大的订单长度
#define MaxOrderListLength		200

typedef uint32_t	UpdateSongListResult;
enum
{
	enmUpdateSongListResult_OK				= 0x00,		//成功
	enmUpdateSongListResult_Full			= 0x01,   	//歌单已满
	enmUpdateSongListResult_HotFull			= 0x02,   	//热推歌已满
	enmUpdateSongListResult_NoPermission	= 0x03,	 	//没有权限(不是主播..)
	enmUpdateSongListResult_Unknown			= 0xff,
};

typedef uint32_t	RequestSongResult;
enum
{
	enmRequestSongResult_OK					= 0x00,		//成功
	enmRequestSongResult_MoneyNotEnough		= 0x01,		//钱不够
	enmRequestSongResult_NoPermission		= 0x02,	 	//没有权限(不是主播..)
	enmRequestSongResult_Timeout			= 0x03,		//超时
	enmRequestSongResult_Unknown			= 0x04,		//未知错误
};

typedef uint32_t	FightForTicketResult;
enum
{
	enmFightForTicketResult_OK					= 0x00,		//成功
	enmFightForTicketResult_NoLeftTicket		= 0x01,		//没有票了
	enmFightForTicketResult_NoPermission		= 0x02,	 	//没有权限(室主，点歌者，被点主播不可以抢票)
	enmFightForTicketResult_Timeout				= 0x03,		//超时
	enmFightForTicketResult_Unknown				= 0x04,		//未知错误
};

enum
{
	enmOffline_stat                              = 0x00,      //不在线
	enmOnline_stat                               = 0x01,      //在线
};
FRAME_ROOMSERVER_NAMESPACE_END

#endif /* DAL_DEF_H_ */
