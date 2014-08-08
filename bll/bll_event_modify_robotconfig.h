/*
 *  bll_event_modify_robotconfig.h
 *
 *  To do��
 *  Created on: 	2012-5-29
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_MODIFY_ROBOTCONFIG_H_
#define BLL_EVENT_MODIFY_ROBOTCONFIG_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CModifyRobotConfigEvent : public CBllBase
{
public:
	CModifyRobotConfigEvent()
	{
	}

	virtual ~CModifyRobotConfigEvent()
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

#define	CREATE_MODIFYROBOTCONFIGEVENT_INSTANCE	    CSingleton<CModifyRobotConfigEvent>::CreateInstance
#define	GET_MODIFYROBOTCONFIGEVENT_INSTANCE		CSingleton<CModifyRobotConfigEvent>::GetInstance
#define	DESTROY_MODIFYROBOTCONFIGEVENT_INSTANCE	CSingleton<CModifyRobotConfigEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END
#endif /* BLL_EVENT_MODIFY_ROBOTCONFIG_H_ */
