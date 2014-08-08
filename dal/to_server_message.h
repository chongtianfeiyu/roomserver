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
 * ����ת����֪ͨ
 */
class CAddTransmitUserNotice:public CAddTransmitUserNotice_Public
{
};
/*
 * ɾ��ת����֪ͨ
 */
class CDelTransmitUserNotice:public CDelTransmitUserNotice_Public
{
};

/*
 * �޸�ת��֪ͨ
 */
class CModifyMediaNotice:public CModifyMediaNotice_Public
{
};


//�·��Ž���
class CCreateRoomNotice : public CCreateRoomNotice_Public
{
};

//�·�������
class CDestoryRoomNotice : public CDestoryRoomNotice_Public
{
};


//�û����뷿��
class CEnterRoomNotice : public CEnterRoomNotice_Public
{
};


//�û��뿪����
class CExitRoomNotice : public CExitRoomNotice_Public
{
};

//�ϱ�������Ϣ��hall
class CUpdateInfoToHallNotice : public CUpdateInfoToHallNotice_Public
{
};

//��hall���󷿼��meida��������Ϣ
class CGetMediaInfoReq : public CGetMediaInfoReq_Public
{
};

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* TO_SERVER_MESSAGE_H_ */
