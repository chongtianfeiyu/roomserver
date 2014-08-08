/*
 * bll_event_text.h
 *
 *  Created on: 2011-12-24
 *      Author: jimm
 */

#ifndef BLL_EVENT_TEXT_H_
#define BLL_EVENT_TEXT_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "dal/from_client_message.h"
#include "dal/to_client_message.h"
#include "dal/dal_room.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CTextMessageEvent : public CBllBase
{
public:
	CTextMessageEvent()
	{
	}

	virtual ~CTextMessageEvent()
	{
	}

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//�Ự�¼�����
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	int32_t VerifyPlayer(MessageHeadSS * pMsgHead, CSendTextReq *pSendTextMsg, CRoom *pRoom, CPlayer *pPlayer,
			PlayerIndex nPlayerIndex, CSendTextResp &stSendTextResp, CSendTextNoti &stSendTextNoti);

private:
};

#define	CREATE_TEXTEVENT_INSTANCE	CSingleton<CTextMessageEvent>::CreateInstance
#define	GET_TEXTEVENT_INSTANCE		CSingleton<CTextMessageEvent>::GetInstance
#define	DESTROY_TEXTEVENT_INSTANCE	CSingleton<CTextMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_TEXT_H_ */
