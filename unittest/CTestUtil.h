//***********************************************************************
//
//      CPPUNIT test frame
//      51.com Corp
//      Author:wwenjing    Create Date:2010-10-26 10:35:27
//		itemserver��Ԫ�������������õ�һЩ����
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
	////��ҵ�¼
	//CPlayer * PlayerLogin(const int32_t roleID);
	////��ҵǳ�
	//void PlayerLogout(const int32_t nRoleID);

	//�ӷ��Ͷ�����ȡ��Ϣ��������head��body����
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

