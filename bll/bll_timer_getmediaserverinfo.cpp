/*
 * bll_timer_getmediaserverinfo.cpp
 *
 *  Created on: 2013-4-1
 *      Author: gaox
 */

#include "bll_timer_getmediaserverinfo.h"
#include "ctl/server_datacenter.h"
#include "bll/bll_base.h"
#include "dal/to_server_message.h"
#include "daemon_message_define.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CGetMediaServerInfo::OnTimerEvent(CFrameTimer *pTimer)
{
	CDaemonStartNotify_Public stRoomServerStartNoti;
	stRoomServerStartNoti.serverType = enmEntityType_Room;
	stRoomServerStartNoti.serverID = g_FrameConfigMgt.GetFrameBaseConfig().GetServerID();

	g_BllBase.SendMessageNotifyToServer(MSGID_DAEMON_START_NOTIFY, &stRoomServerStartNoti, enmInvalidRoomID, enmTransType_Broadcast,
						enmInvalidRoleID, enmEntityType_Media);

	return S_OK;
}

FRAME_ROOMSERVER_NAMESPACE_END
