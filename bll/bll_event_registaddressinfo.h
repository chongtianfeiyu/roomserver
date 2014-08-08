/*
 * bll_event_registaddressinfo.h
 *
 *  Created on: 2013-3-5
 *      Author: jimm
 */

#ifndef BLL_EVENT_REGISTADDRESSINFO_H_
#define BLL_EVENT_REGISTADDRESSINFO_H_

#include "def/server_namespace.h"
#include "main_frame.h"
#include "bll_base.h"
#include "mediaserver_message_define.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CRegistAddressInfoMessageEvent : public CBllBase
{
public:
	CRegistAddressInfoMessageEvent()
	{
	}

	virtual ~CRegistAddressInfoMessageEvent()
	{
	}

	//��Ϣ�¼�
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//�Ự�¼�����
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

};

#define	CREATE_REGISTADDRESSINFOEVENT_INSTANCE		CSingleton<CRegistAddressInfoMessageEvent>::CreateInstance
#define	GET_REGISTADDRESSINFOEVENT_INSTANCE			CSingleton<CRegistAddressInfoMessageEvent>::GetInstance
#define	DESTROY_REGISTADDRESSINFOEVENT_INSTANCE		CSingleton<CRegistAddressInfoMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END



#endif /* BLL_EVENT_REGISTADDRESSINFO_H_ */
