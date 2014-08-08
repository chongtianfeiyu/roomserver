#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#include "../ICallback/CallBack.h"
#include "def/server_namespace.h"

#define SET_CALLBACK(callback)	\
	SetCallbackFunction(_ ## callback)

//这个宏的意思是：将一个类中的非静态函数，转换成静态函数，以方便注册到回调函数中（函数指针作为实参时，如果该函数在类中，必须是静态方法）
#define CALLBACK_RELATION(Class, Callback)	\
	static void _ ## Callback(Class* _this, void *pData)	\
{														\
	_this->Callback(pData);								\
}

#define  REGISTER_CALLBACK(callback, Class, callback_)	\
	callback;	\
	CALLBACK_RELATION(Class, callback_);
