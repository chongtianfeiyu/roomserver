/*
 * def_message_id.h
 *
 *  Created on: 2011-12-2
 *      Author: jimm
 */

#ifndef DEF_MESSAGE_ID_H_
#define DEF_MESSAGE_ID_H_

//玩家的心跳
#define MSGID_CLRS_HEARTBEAT					0x00018005
//登陆房间请求
#define MSGID_CLRS_LOGIN_REQ					0x00018007

//登陆房间响应
#define MSGID_RSCL_LOGIN_RESP					0x00020707

//登陆房间通知
#define MSGID_RSCL_LOGIN_NOTI					0x00030707

//登出房间请求
#define MSGID_CLRS_LOGOUT_REQ					0x00018008

//登出房间通知
#define MSGID_RSCL_LOGOUT_NOTI					0x00030708

//获取用户列表请求
#define MSGID_CLRS_GETUSERLIST_REQ				0x00018009

//获取用户列表响应
#define MSGID_RSCL_GETUSERLIST_RESP				0x00020709

//获取麦序列表请求
#define MSGID_CLRS_GETWAITTINGLIST_REQ			0x0001800a

//获取麦序列表响应
#define MSGID_RSCL_GETWAITTINGLIST_RESP			0x0002070a

//发送文本消息请求
#define MSGID_CLRS_SENDTEXT_REQ					0x0001800b

//发送文本消息响应
#define MSGID_RSCL_SENDTEXT_RESP				0x0002070b

//发送文本消息通知
#define MSGID_RSCL_SENDTEXT_NOTI				0x0003070b

//排麦请求
#define MSGID_CLRS_REQUESTSHOW_REQ				0x0001800e

//排麦响应
#define MSGID_RSCL_REQUESTSHOW_RESP				0x0002070e

//排麦通知
#define MSGID_RSCL_REQUESTSHOW_NOTI				0x0003070e

//下麦请求
#define MSGID_CLRS_EXITSHOW_REQ					0x00018010

//下麦通知
#define MSGID_RSCL_EXITSHOW_NOTI				0x00030710

//请求关闭或打开某人的音视频
#define MSGID_CLRS_OPERATE_VIDEO_AUDIO_REQ		0x00018011

//回应关闭或打开某人的音视频
#define MSGID_RSCL_OPERATE_VIDEO_AUDIO_RESP		0x00020711

//关闭或打开某人的音视频的通知
#define MSGID_RSCL_OPERATE_VIDEO_AUDIO_NOTI		0x00030711

//请求踢人
#define MSGID_CLRS_KICK_USER_REQ				0x00018012

//回应踢人
#define MSGID_RSCL_KICK_USER_RESP				0x00020712

//踢人的通知
#define MSGID_RSCL_KICK_USER_NOTI				0x00030712

//加黑请求
#define MSGID_CLRS_ADD_BLACK_REQ				0x00018013

//回应加黑
#define MSGID_RSCL_ADD_BLACK_RESP				0x00020713

//加黑的通知
#define MSGID_RSCL_ADD_BLACK_NOTI				0x00030713

//封IP请求
#define MSGID_CLRS_LOCK_IP_REQ					0x00018014

//回应封IP
#define MSGID_RSCL_LOCK_IP_RESP					0x00020714

//封IP的通知
#define MSGID_RSCL_LOCK_IP_NOTI					0x00030714

//禁言请求
#define MSGID_CLRS_SHUT_UP_REQ					0x00018015

//回应禁言
#define MSGID_RSCL_SHUT_UP_RESP					0x00020715

//禁言的通知
#define MSGID_RSCL_SHUT_UP_NOTI					0x00030715

//解除禁言请求
#define MSGID_CLRS_UNLOCK_SHUT_UP_REQ			0x00018016

//回应解除禁言
#define MSGID_RSCL_UNLOCK_SHUT_UP_RESP			0x00020716

//解除禁言的通知
#define MSGID_RSCL_UNLOCK_SHUT_UP_NOTI			0x00030716

//邀请上麦请求
#define MSGID_CLRS_PUSH_ON_SHOW_REQ				0x00018017

//邀请上麦响应
#define MSGID_RSCL_PUSH_ON_SHOW_RESP			0x00020717

//邀请上麦通知
#define MSGID_RSCL_PUSH_ON_SHOW_NOTI			0x00030717

//邀请下麦请求
#define MSGID_CLRS_PULL_OFF_SHOW_REQ			0x00018018

//邀请下麦响应
#define MSGID_RSCL_PULL_OFF_SHOW_RESP			0x00020718

//邀请下麦通知
#define MSGID_RSCL_PULL_OFF_SHOW_NOTI			0x00030718

//增加管理权限请求
#define MSGID_CLRS_ADD_TITLE_REQ				0x00018019

//增加管理权限响应
#define MSGID_RSCL_ADD_TITLE_RESP				0x00020719

//增加管理权限通知
#define MSGID_RSCL_ADD_TITLE_NOTI				0x00030719

//删除管理权限请求
#define MSGID_CLRS_DEL_TITLE_REQ				0x0001801a

//删除管理权限响应
#define MSGID_RSCL_DEL_TITLE_RESP				0x0002071a

//删除管理权限通知
#define MSGID_RSCL_DEL_TITLE_NOTI				0x0003071a

//打开某人的私麦请求
#define MSGID_CLRS_OPEN_PEIVATE_REQ				0x0001801b

//打开某人的私麦响应
#define MSGID_RSCL_OPEN_PEIVATE_RESP			0x0002071b

//关闭某人的私麦请求
#define MSGID_CLRS_CLOSE_PEIVATE_REQ			0x0001801c

//关闭某人的私麦响应
#define MSGID_RSCL_CLOSE_PEIVATE_RESP			0x0002071c

//请求与某人一对一视频
#define MSGID_CLRS_SENDP2P_REQ					0x0001801d

//一对一视频响应（请求与接受的结果）
#define MSGID_RSCL_SENDP2P_RESP					0x0002071d

//通知对方视频请求（被请求者显示接受请求）
#define MSGID_RSCL_SENDP2P_NOTI					0x0003071d

//一对一视频连接的答复请求
#define MSGID_CLRS_RECVP2P_REQ					0x0001801e

//一对一视频连接的答复回应
#define MSGID_RSCL_RECVP2P_RESP					0x0002071e

//通知请求者请求的最终结果
#define MSGID_RSCL_RECVP2P_NOTI					0x0003071e

//设置房间信息请求
#define MSGID_CLRS_SET_ROOM_REQ					0x00018022

//设置房间信息回应
#define MSGID_RSCL_SET_ROOM_RESP				0x00020722

//通知设置房间信息
#define MSGID_RSCL_SET_ROOM_NOTI				0x00030722

//获取房间信息请求
#define MSGID_CLRS_GET_ROOM_REQ					0x00018023

//获取房间信息回应
#define MSGID_RSCL_GET_ROOM_RESP				0x00020723

//通知进房分配的私麦
#define MSGID_RSCL_SRV_PRIVATE_MIC_NOTI			0x00030728

//通知房间被封
#define MSGID_RSCL_SRV_ROOM_LOCKED_NOTI			0x0003072b

//设置mic请求
#define MSGID_CLRS_SET_MIC_REQ					0x0001802c

//设置房间信息回应
#define MSGID_RSCL_SET_MIC_RESP					0x0002072c

//通知设置房间信息
#define MSGID_RSCL_SET_MIC_NOTI					0x0003072c

//通知重新设置media
#define MSGID_RSCL_RESET_MEDIA_NOTI				0x00030730

//通知玩家的信息改变
#define MSGID_RSCL_USERINFO_CHANGE_NOTI			0x00030724

//应答邀请上麦
#define MSGID_RSCL_ANSWER_PULL_SHOW_REQ			0x00018032

//应答邀请上麦回应
#define MSGID_RSCL_ANSWER_PULL_SHOW_RESP		0x00020732

//更新歌曲列表请求
#define MSGID_CLRS_UPDATESONGLIST_REQ			0x00018039

//更新歌曲列表响应
#define MSGID_RSCL_UPDATESONGLIST_RESP			0x00020739

//获取歌曲列表请求
#define MSGID_CLRS_GETSONGLIST_REQ				0x0001803a

//获取歌曲列表响应
#define MSGID_RSCL_GETSONGLIST_RESP				0x0002073a

//点歌请求
#define MSGID_CLRS_REQUESTSONG_REQ				0x0001803b

//点歌请求响应
#define MSGID_RSCL_REQUESTSONG_RESP				0x0002073b

//点歌请求通知
#define MSGID_RSCL_REQUESTSONG_NOTI				0x0003073b

//处理订单请求
#define MSGID_CLRS_PROCESSORDER_REQ				0x0001803c

//处理订单响应
#define MSGID_RSCL_PROCESSORDER_RESP			0x0002073c

//处理订单通知
#define MSGID_RSCL_PROCESSORDER_NOTI			0x0003073c

//获取订单请求
#define MSGID_CLRS_GETORDERLIST_REQ				0x0001803d

//获取订单响应
#define MSGID_RSCL_GETORDERLIST_RESP			0x0002073d

//当前的歌曲信息通知
#define MSGID_RSCL_CURSONGINFO_NOTI				0x0003073e

//票数更新通知
#define MSGID_RSCL_TICKETCOUNTUPDATE_NOTI		0x0003073f

//抢票请求
#define MSGID_CLRS_FIGHTFORTICKET_REQ			0x00018040

//抢票响应
#define MSGID_RSCL_FIGHTFORTICKET_RESP			0x00020740

//开始欣赏通知
#define MSGID_RSCL_ENJOYING_NOTI				0x00030741

//歌曲评价请求
#define MSGID_CLRS_SONGVOTE_REQ					0x0001803e

//歌曲评价响应
#define MSGID_RSCL_SONGVOTE_RESP				0x0002073e

//歌曲投票结果通知
#define MSGID_RSCL_SONGVOTE_NOTI				0x0003073e

//歌曲投票结果更新通知
#define MSGID_RSCL_SONGVOTEUPDATE_NOTI			0x00030743

//开始等待点歌通知
#define MSGID_RSCL_WAITINGFORPROGRAM_NOTI		0x00030744

//获取主播列表请求
#define  MSGID_CLRM_GETARTISTOFFLINE_REQ  0x00018041

//获取主播列表回应
#define  MSGID_CLRM_GETARTISTOFFLINE_RESP  0x00020741

#endif /* DEF_MESSAGE_ID_H_ */
