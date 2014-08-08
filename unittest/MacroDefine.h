#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include "../ICallback/CallBack.h"
#include "def/server_namespace.h"

#define SET_CALLBACK(callback)	\
	SetCallbackFunction(_ ## callback)

//��������˼�ǣ���һ�����еķǾ�̬������ת���ɾ�̬�������Է���ע�ᵽ�ص������У�����ָ����Ϊʵ��ʱ������ú��������У������Ǿ�̬������
#define CALLBACK_RELATION(Class, Callback)	\
	static void _ ## Callback(Class* _this, void *pData)	\
{														\
	_this->Callback(pData);								\
}

#define  REGISTER_CALLBACK(callback, Class, callback_)	\
	callback;	\
	CALLBACK_RELATION(Class, callback_);
