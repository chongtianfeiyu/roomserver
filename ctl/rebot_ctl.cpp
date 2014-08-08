/*
 *  rebot_ctl.cpp
 *
 *  To do£º
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#include "rebot_ctl.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

void CRebotCtl::RecoverRoleID(const RoleID nRoleID)
{
	for(uint32_t i =0;i<nRecoverCount;i++)
	{
		if(arrRecoverRoleID[i]==nRoleID)
		{
			return;
		}
	}
	if(nRecoverCount>=MAX_ROLEID_COUNT)
	{
		return;
	}
	arrRecoverRoleID[nRecoverCount] = nRoleID;
	nRecoverCount++;
}
FRAME_ROOMSERVER_NAMESPACE_END
