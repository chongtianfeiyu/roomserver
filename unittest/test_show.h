/*
 *  test_show.h
 *
 *  To do：
 *  Created on: 	2012-3-31
 *  Author: 		luocj
 */
#ifdef _UNITTEST
#ifndef TEST_SHOW_H_
#define TEST_SHOW_H_
#endif
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>

#ifdef _WIN32
#else
#include <sys/time.h>
#include <unistd.h>
#endif

#include "MacroDefine.h"
#include "CTestUtil.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CTShow:public CppUnit::TestCase,public ICallbackListener
{
	CPPUNIT_TEST_SUITE(CTShow);
	//to add other test method here.
	CPPUNIT_TEST( Test1_P);
	CPPUNIT_TEST( Test_GetValueInfoByType);

	CPPUNIT_TEST_SUITE_END();

public:
	CTShow()
	{
		m_callbackFunction=NULL;
		m_nCaseCount = 0;
		memset(m_arrCaseList, 0, sizeof(m_arrCaseList));
		std::ifstream ifs("show.ini");
		while (!ifs.eof())
		{
			ifs >> m_arrCaseList[m_nCaseCount++];
		}
	};

	virtual ~CTShow(void) {};

	//Set up context before run a test.
	virtual void setUp();
	//Clean up after the test run
	virtual void tearDown();

public:
	void Test1_P();
	void Test_GetValueInfoByType();

private:
	void (CTShow::*m_callbackFunction)(void*);
	void SetCallbackFunction(void(CTShow::*pFunc)(void*))
	{
		m_callbackFunction=pFunc;
	}
	void _Callback_Function(void*);

private:
	virtual void OnCallBack(char *pStr, void *pData);

#define MAX_TESTCASE_COUNT 40 //最大测试用例个数
#define MAX_FUNCTIONNAME_LEN 40
protected:
	int m_nCaseCount;
	char m_arrCaseList[MAX_TESTCASE_COUNT][MAX_FUNCTIONNAME_LEN];
private:
#define ISNEEDRUNTESTCASE(casename)		\
	if (!_IsNeedRunCase(casename))	\
	  {								\
	  return;						\
		 }
	bool _IsNeedRunCase(const char* szCaseName)
	{
		for (int32_t i = 0; i < m_nCaseCount; ++i)
		{
			if (strcmp(szCaseName, m_arrCaseList[i]) == 0)
			{
				return true;
			}
		}
		return false;
	}
};

FRAME_ROOMSERVER_NAMESPACE_END

#endif /* TEST_SHOW_H_ */
