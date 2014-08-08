//***********************************************************************
//
//      CPPUNIT test frame
//      51.com Corp
//      Author:wwenjing    Create Date:2010-10-26 10:35:27
//		itemserver单元测试用例，公用的一些函数
//
//***********************************************************************

#include "MacroDefine.h"
#include "main_frame.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CTestUtil
{
private:
	CTestUtil(){};
	~CTestUtil(){};	

public:
	static CTestUtil *GetInstance();
	////玩家登录
	//CPlayer * PlayerLogin(const int32_t roleID);
	////玩家登出
	//void PlayerLogout(const int32_t nRoleID);

	//从发送队列中取消息，并解析head和body部分
	void* GetMsgSent(EntityType entityType,MessageHeadSS &stMessageHead);
private:
	//
	//static void _SetMsgHeadCS(MessageHeadCS &msg, int32_t roleID);		
	
//public:
//	static uint16_t m_snConnectionIndex;
//	static uint32_t m_snSequence;
private:
	static CTestUtil *m_pTestUtil;
};

FRAME_ROOMSERVER_NAMESPACE_END

