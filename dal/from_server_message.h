/*
 * from_server_message.h
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#ifndef FROM_SERVER_MESSAGE_H_
#define FROM_SERVER_MESSAGE_H_

#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "common/common_string.h"
#include "def/dal_def.h"
#include "public_typedef.h"
#include "roomserver_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN


class CGetRoleInfoResp : public CGetRoleInfoResp_Public
{
};

class CGetRoomInfoResp : public CGetRoomInfoResp_Public
{
};

class CGetMediaInfoResp : public CGetMediaInfoResp_Public
{
};

class CSetTitleResp : public CSetTitleResp_Public
{
};

class CDelTitleResp : public CDelTitleResp_Public
{
};
//add your message to map
MESSAGEMAP_DECL_BEGIN(from_server)

MESSAGEMAP_DECL_MSG(MSGID_HSRS_GETMEIDAINFO_RESP, (new CGetMediaInfoResp()))
MESSAGEMAP_DECL_MSG(MSGID_DBRS_GETROLEINFO_RESP, (new CGetRoleInfoResp()))
MESSAGEMAP_DECL_MSG(MSGID_DBRS_GETROOMINFO_RESP, (new CGetRoomInfoResp()))
MESSAGEMAP_DECL_MSG(MSGID_DBRS_DEL_TITLE_RESP, (new CDelTitleResp()))
//MESSAGEMAP_DECL_MSG(MSGID_DBRS_SET_TITLE_RESP, (new CSetTitleResp()))
MESSAGEMAP_DECL_MSG(MSGID_DBRS_SET_ROOM_RESP, (new CSetRoomAttrResp()))
MESSAGEMAP_DECL_MSG(MSGID_HSRS_GET_ROLE_GLOBALINFO_RESP, (new CGetRoleGlobalInfoResp()))
MESSAGEMAP_DECL_MSG(MSGID_ISRS_GET_ITEMINFO_RESP, (new CGetItemInfoResp()))
MESSAGEMAP_DECL_MSG(MSGID_DBRS_ADD_BLACK_RESP, (new CAddPlayerToBlackResp()))

MESSAGEMAP_DECL_END()

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* FROM_SERVER_MESSAGE_H_ */
