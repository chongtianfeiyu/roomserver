#include "CTestUtil.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

//uint16_t CTestUtil::m_snConnectionIndex=1234;
//uint32_t CTestUtil::m_snSequence=1;
CTestUtil* CTestUtil::m_pTestUtil=NULL;

CTestUtil* CTestUtil::GetInstance()
{
	if (m_pTestUtil==NULL)
	{
		m_pTestUtil=new CTestUtil;
	}
	return m_pTestUtil;
}
#include <iostream>

void* CTestUtil::GetMsgSent(EntityType entityType,MessageHeadSS &stMessageHead)
{

 return NULL;
}

FRAME_ROOMSERVER_NAMESPACE_END

