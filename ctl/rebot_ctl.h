/*
 *  rebot_ctl.h
 *
 *  To do：
 *  Created on: 	2012-2-15
 *  Author: 		luocj
 */

#ifndef REBOT_CTL_H_
#define REBOT_CTL_H_

#include "common/common_api.h"
#include "config/rebotconfig.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN
#define MAX_ROLEID_COUNT			10000
#define SEND_TO_PUBLICMIC_PERCENT   50
#define HAS_MIC_PERCENT   50

#define PURPLE_PERCENT      1
#define PURPLE_RED   		2
#define PURPLE_REGISTER     9

#define MAX_MAGNATELEVEL    5    //最高的财富等级
class CRebotCtl
{
public:
	CRebotCtl()
	{
		nGetRoleIDCount = 0;
		nRecoverCount = 0;
		for(uint32_t i = 0 ;i < MAX_ROLEID_COUNT;i++)
		{
			arrRecoverRoleID[i] = enmInvalidRoleID;
		}
		nGet179IDInfoIndex = 0;
		nGet179IDCount = 0;
		nGetNameCount = 0;
	}
	virtual ~CRebotCtl()
	{

	}
    void RecoverRoleID(const RoleID nRoleID);
	//分配机器人的RoleID
	RoleID  GetRebotRoleID()
	{
		RoleID nRetRoleID = enmInvalidRoleID;
		if(nRecoverCount > 0)
		{
			nRetRoleID = arrRecoverRoleID[0];
			nRecoverCount--;
			arrRecoverRoleID[0] = arrRecoverRoleID[nRecoverCount];
			arrRecoverRoleID[nRecoverCount] = enmInvalidRoleID;
			return nRetRoleID;
		}
		uint32_t nRoleIDBegin = g_RebotConfig.GetRoleIDInfo().nRoleIDBegin;
		uint32_t nRoleIDEnd = g_RebotConfig.GetRoleIDInfo().nRoleIDEnd;
		nRetRoleID = nRoleIDBegin+nGetRoleIDCount;
		if(nGetRoleIDCount == nRoleIDEnd-nRoleIDBegin)
		{
			nGetRoleIDCount = 0;
		}
		else
		{
			nGetRoleIDCount++;
		}
		return nRetRoleID;
	}
    //分配机器人的RoleName
    const char* GetRebotRoleName()
	{
        const char *retName = g_RebotConfig.GetRoleName(nGetNameCount);
        if(nGetNameCount == (g_RebotConfig.GetNameCount()-1))
        {
        	nGetNameCount = 0;
        }
        else
        {
        	nGetNameCount++;
        }
        return retName;
	}
    //获取179ID
    uint32_t GetRebot179ID()
    {
    	C179IDInfo st179IDInfo = g_RebotConfig.Get179IDInfo(nGet179IDInfoIndex);
    	uint32_t nRet179ID = 0;
        uint32_t n179IDBegin = st179IDInfo.n179IDBegin;
        uint32_t n179IDEnd = st179IDInfo.n179IDEnd;
        nRet179ID = n179IDBegin + nGet179IDCount;
        if(nGet179IDCount == (n179IDEnd-n179IDBegin))
        {
        	nGet179IDCount = 0;
        	if(nGet179IDInfoIndex == (g_RebotConfig.Get179IDsCount()-1))
			{
				nGet179IDInfoIndex = 0;
			}
			else
			{
				nGet179IDInfoIndex++;
			}
        }
        else
        {
        	nGet179IDCount++;
        }
        return nRet179ID;
    }

    VipLevel GetRebootVip()
    {
    	int32_t nRange = 100;
    	int32_t nRandom = Random2((PURPLE_PERCENT + PURPLE_RED + PURPLE_REGISTER)*nRange);
		if(nRandom < 0)
		{
			nRandom = -nRandom;
		}
		if(nRandom < PURPLE_PERCENT * nRange)
		{
			return enmVipLevel_PURPLE;
		}
		else if(nRandom < (PURPLE_RED + PURPLE_PERCENT) * nRange)
		{
			return enmVipLevel_RED;
		}
		return enmVipLevel_REGISTER;
    }

    int32_t GetColorType()
	{
    	uint32_t nColorBegin = g_RebotConfig.GetColorInfo().nColorBegin;
    	uint32_t nColorEnd = g_RebotConfig.GetColorInfo().nColorEnd;
    	uint32_t nColorCount = nColorEnd-nColorBegin+1;
		int32_t nRandom = Random2((nColorCount)*100);
		if(nRandom < 0)
		{
			nRandom = -nRandom;
		}
		if(nRandom == (int32_t)nColorCount*100)
		{
			nRandom = nRandom-1;
		}
		return nColorBegin+nRandom/100;
	}

    bool IsSendToPublicMic()
    {
    	int32_t nRang = 1000;
    	int32_t nRandom = Random2(nRang);
		if(nRandom < 0)
		{
			nRandom = -nRandom;
		}
		if(nRandom < nRang*SEND_TO_PUBLICMIC_PERCENT/100)
		{
			return true;
		}
		return false;
    }
    bool IsHasMic()
    {
    	int32_t nRang = 1000;
    	int32_t nRandom = Random2(nRang);
		if(nRandom < 0)
		{
			nRandom = -nRandom;
		}
		if(nRandom < g_RebotConfig.GetMicRand())
		{
			return true;
		}
		return false;
    }

    MagnateLevel GetMagnateLevel(VipLevel nVipLevel = enmVipLevel_NONE)
    {
    	int32_t nRang = 10000;
		int32_t nRandom = Random2(nRang);
		if(nRandom < 0)
		{
			nRandom = -nRandom;
		}
    	if(nVipLevel == enmVipLevel_REGISTER)
    	{
    		if(nRandom < 7000)
    		{
    			return enmMagnateLevel_0;
    		}
    		if(nRandom < 9000)
			{
				return enmMagnateLevel_1;
			}
			if(nRandom < 10000)
			{
				return enmMagnateLevel_2;
			}
    	}
    	if(nVipLevel == enmVipLevel_RED)
		{
			if(nRandom < 8000)
			{
				return enmMagnateLevel_1;
			}
			if(nRandom < 9500)
			{
				return enmMagnateLevel_2;
			}
			if(nRandom < 10000)
			{
				return enmMagnateLevel_3;
			}
		}
		if(nVipLevel == enmVipLevel_PURPLE)
		{
			if(nRandom < 5500)
			{
				return enmMagnateLevel_1;
			}
			if(nRandom < 8000)
			{
				return enmMagnateLevel_2;
			}
			if(nRandom < 9000)
			{
				return enmMagnateLevel_3;
			}
			if(nRandom < 9500)
			{
				return enmMagnateLevel_4;
			}
			if(nRandom < 10000)
			{
				return enmMagnateLevel_5;
			}
		}
		return enmMagnateLevel_2;
    }
private:
	uint32_t	nGetRoleIDCount;				//从非回收站得到RoleID的次数
	uint32_t	nRecoverCount;					//回收的个数
	RoleID  	arrRecoverRoleID[MAX_ROLEID_COUNT];  //被回收的RoleID
	uint32_t	nGet179IDInfoIndex;				 //在那个端获取179ID
	uint32_t	nGet179IDCount;                  //在某个号码段获取179ID的次数
	uint32_t	nGetNameCount;                  //获取RoleName的次数
};

#define	CREATE_REBOTCTL_INSTANCE	CSingleton<CRebotCtl>::CreateInstance
#define	GET_REBOTCTL_INSTANCE		CSingleton<CRebotCtl>::GetInstance
#define	DESTROY_REBOTCTL_INSTANCE	CSingleton<CRebotCtl>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* REBOT_CTL_H_ */
