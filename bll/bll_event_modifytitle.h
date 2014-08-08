/*
 *  bll_event_modifytitle.h
 *
 *  To do：
 *  Created on: 	2012-3-5
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_MODIFYTITLE_H_
#define BLL_EVENT_MODIFYTITLE_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "hallserver_message_define.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CModifyTitleMessageEvent : public CBllBase
{
public:
	CModifyTitleMessageEvent()
	{
	}

	virtual ~CModifyTitleMessageEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);

	//保存请求数据到会话中
	class CGetArtistInfoSessionData
	{
	public:
		MessageHeadSS m_sMsgHead;
		uint16_t m_nOptionLen;
		char m_arrOptionData[MaxOptionDataSize];
		CModifyTitleNoti m_pMsgBody;

		CGetArtistInfoSessionData()
		{
			memset(&m_sMsgHead, 0, sizeof(m_sMsgHead));
			m_nOptionLen = 0;
			memset(&m_arrOptionData, 0, sizeof(m_arrOptionData));
			memset(&m_pMsgBody, 0, sizeof(m_pMsgBody));
		}
	};

};

#define	CREATE_MODIFYTITLEEVENT_INSTANCE	CSingleton<CModifyTitleMessageEvent>::CreateInstance
#define	GET_MODIFYTITLEEVENT_INSTANCE		CSingleton<CModifyTitleMessageEvent>::GetInstance
#define	DESTROY_MODIFYTITLEEVENT_INSTANCE	CSingleton<CModifyTitleMessageEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* BLL_EVENT_MODIFYTITLE_H_ */
