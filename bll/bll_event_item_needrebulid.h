/*
 *  bll_event_item_needrebulid.h
 *
 *  To do：
 *  Created on: 	2012-5-18
 *  Author: 		luocj
 */

#ifndef BLL_EVENT_ITEM_NEEDREBULID_H_
#define BLL_EVENT_ITEM_NEEDREBULID_H_

#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "global.h"
#include "main_frame.h"
#include "bll_base.h"
#include "ctl/item_rebulid_ctl.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CItemRebulidEvent : public CBllBase
{
public:
	CItemRebulidEvent()
	{
	}

	virtual ~CItemRebulidEvent()
	{
	}

	//消息事件
	virtual int32_t OnMessageEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	//会话事件激活
	virtual int32_t OnSessionEvent(MessageHeadSS * pMsgHead, IMsgBody* pMsgBody,
			CFrameSession* pSession, const uint16_t nOptionLen = 0, const void *pOptionData = NULL);

	int32_t OnSessionTimeOut(CFrameSession *pSession);
private:
	int32_t SendCreatRoomNotice();
	int32_t	GetNeedRebulidPlayer(EntityType nEntityType);
	int32_t StartTimer(EntityType nEntityType);
	int32_t SendEnterRoomNotice(EntityType nEntityType);

	class CServerNeedRebulidSessionData
	{
	public:
		EntityType nEntityType;
	};
};

#define	CREATE_ITEMREBULIDEVENT_INSTANCE		CSingleton<CItemRebulidEvent>::CreateInstance
#define	GET_ITEMREBULIDEVENT_INSTANCE			CSingleton<CItemRebulidEvent>::GetInstance
#define	DESTROY_ITEMREBULIDEVENT_INSTANCE		CSingleton<CItemRebulidEvent>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* BLL_EVENT_ITEM_NEEDREBULID_H_ */
