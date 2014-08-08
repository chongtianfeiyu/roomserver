/*
 * billengine.cpp
 *
 *  Created on: 2012-2-28
 *      Author: jimm
 */

#include "common/common_def.h"
#include "common/common_datetime.h"
#include "common/common_api.h"
#include "billengine.h"
#include "frame_configmgt.h"
#include "public_typedef.h"
#include "dal/dal_player.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

/*
* 玩家物品更新账单宏定义
*/

int32_t  CBillEngine::ms_nBillNumber = 0;

CBillEngine::CBillEngine()
{
}

CBillEngine::~CBillEngine()
{
}

void CBillEngine::GenerateTransID(char *szTransID)
{
	CDateTime dt = CDateTime::CurrentDateTime();
	CFrameBaseConfig stFrameConfig = g_FrameConfigMgt.GetFrameBaseConfig();
	sprintf(szTransID, "Z%02dTP%02dID%03dTM%08XSQ%04X", stFrameConfig.GetZoneID(), enmEntityType_Item,  stFrameConfig.GetServerID(), (uint32_t)dt.Seconds(), ++ms_nBillNumber);
}

FRAME_ROOMSERVER_NAMESPACE_END

