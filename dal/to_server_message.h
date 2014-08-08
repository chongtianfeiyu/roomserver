/*
 * to_server_message.h
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#ifndef TO_SERVER_MESSAGE_H_
#define TO_SERVER_MESSAGE_H_

#include "def/server_namespace.h"
#include "roomserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN


class CGetRoleInfoReq : public CGetRoleInfoReq_Public
{

};

class CGetRoomInfoReq : public CGetRoomInfoReq_Public
{
};

/*
 * 增加转发的通知
 */
class CAddTransmitUserNotice:public CAddTransmitUserNotice_Public
{
};
/*
 * 删除转发的通知
 */
class CDelTransmitUserNotice:public CDelTransmitUserNotice_Public
{
};

/*
 * 修改转发通知
 */
class CModifyMediaNotice:public CModifyMediaNotice_Public
{
};


//新房门建立
class CCreateRoomNotice : public CCreateRoomNotice_Public
{
};

//新房间销毁
class CDestoryRoomNotice : public CDestoryRoomNotice_Public
{
};


//用户进入房间
class CEnterRoomNotice : public CEnterRoomNotice_Public
{
};


//用户离开房间
class CExitRoomNotice : public CExitRoomNotice_Public
{
};

//上报房间信息给hall
class CUpdateInfoToHallNotice : public CUpdateInfoToHallNotice_Public
{
};

//向hall请求房间的meida服务器信息
class CGetMediaInfoReq : public CGetMediaInfoReq_Public
{
};

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* TO_SERVER_MESSAGE_H_ */
