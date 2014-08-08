/*
 *  item_rebulid_ctl.h
 *
 *  To do：
 *  Created on: 	2012-5-18
 *  Author: 		luocj
 */

#ifndef ITEM_REBULID_CTL_H_
#define ITEM_REBULID_CTL_H_

#include "common/common_object.h"
#include "common/common_memory.h"
#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "public_typedef.h"
#include <vector>
#include <algorithm>

using namespace std;

FRAME_ROOMSERVER_NAMESPACE_BEGIN

typedef vector<RoleID>  NeedRebulidRoleVector;

class CItemRebulidCtl
{
public:
	CItemRebulidCtl()
	{
		m_isRebulid = false;
		m_arrItemRoleVector.clear();
		m_arrTaskRoleVector.clear();
	}
	virtual ~CItemRebulidCtl()
	{

	}
	bool IsItemRebulid()
	{
		return  m_isRebulid;
	}
	void SetItemRebulid(EntityType nEntityType)
	{
		if(nEntityType == enmEntityType_Item)
		{
			m_isRebulid = true;
		}
	}
	void SetItemRebulidSuccess(EntityType nEntityType)
	{
		if(nEntityType == enmEntityType_Item)
		{
			m_isRebulid = false;
		}
	}
	//取得需要同步的玩家，并删除
	void GetRebulidRoleID(RoleID arrRoleID[],int32_t nSize,int32_t& nRebulidCount,EntityType nEntityType)
	{
		nRebulidCount = 0;
		if(nEntityType == enmEntityType_Item)
		{
			NeedRebulidRoleVector::iterator itBegin = m_arrItemRoleVector.begin();
			while(itBegin!= m_arrItemRoleVector.end()&&nRebulidCount<nSize)
			{
				arrRoleID[nRebulidCount++] = *itBegin;
				itBegin=m_arrItemRoleVector.erase(itBegin);
			}
		}
		if(nEntityType == enmEntityType_Task)
		{
			NeedRebulidRoleVector::iterator itBegin = m_arrTaskRoleVector.begin();
			while(itBegin!= m_arrTaskRoleVector.end()&&nRebulidCount<nSize)
			{
				arrRoleID[nRebulidCount++] = *itBegin;
				itBegin=m_arrTaskRoleVector.erase(itBegin);
			}
		}

	}

	void SetRebulidRoleID(RoleID arrRoleID[],int32_t nSize,EntityType nEntityType)
	{
		if(nEntityType == enmEntityType_Item)
		{
			m_arrItemRoleVector.clear();
			for(int32_t i =0;i<nSize;i++)
			{
				m_arrItemRoleVector.push_back(arrRoleID[i]);
			}
		}
		else if(nEntityType == enmEntityType_Task)
		{
			m_arrTaskRoleVector.clear();
			for(int32_t i =0;i<nSize;i++)
			{
				m_arrTaskRoleVector.push_back(arrRoleID[i]);
			}
		}

	}
private:
	bool 					m_isRebulid;		//正在恢复
	NeedRebulidRoleVector	m_arrItemRoleVector;	//需要恢复的玩家
	NeedRebulidRoleVector	m_arrTaskRoleVector;	//需要恢复的玩家
};


#define	CREATE_ITEMREBULIDCTL_INSTANCE		CSingleton<CItemRebulidCtl>::CreateInstance
#define	GET_ITEMREBULIDCTL_INSTANCE			CSingleton<CItemRebulidCtl>::GetInstance
#define	DESTROY_ITEMREBULIDCTL_INSTANCE		CSingleton<CItemRebulidCtl>::DestroyInstance

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* ITEM_REBULID_CTL_H_ */
