#include "UnitTestMain.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int StartUnitTest()
{
	//CTestCaseUtil testUtil;
	//testUtil.RunTestCase1();

	CppUnit::TextUi::TestRunner runner;
	//CppUnit::Test *suite = CppUnit::TestFactoryRegistry::getRegistry().makeTest();
	// Adds the test to the list of test to run
	//runner.addTest( suite );

	//std::fstream fileRead("vibrancyserver.ini");//open file
	//std::string strLine;
	//while (getline(fileRead,strLine))//read line in file until end
	//{
	//	CppUnit::TestFactoryRegistry &reg= CppUnit::TestFactoryRegistry::getRegistry(strLine.c_str());
	//	runner.addTest(reg.makeTest());
	//}
	//fileRead.close();

	std::ifstream ifs("roomserver.ini");
	std::string strLine;
	while (ifs >> strLine)
	{
		CppUnit::TestFactoryRegistry &reg= CppUnit::TestFactoryRegistry::getRegistry(strLine.c_str());
		runner.addTest(reg.makeTest());
	}

	//CppUnit::TestFactoryRegistry &reg= CppUnit::TestFactoryRegistry::getRegistry("vibrancyserver");
	//runner.addTest(reg.makeTest());
	// Change the default outputter to a compiler error format outputter
	std::ofstream file("roomserver_testresult.log");
	//runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),std::cerr ) );
	runner.setOutputter( new CppUnit::CompilerOutputter( &runner.result(),file ) );

	// Run the tests.
	bool wasSucessful = runner.run();


	return 0;
}

FRAME_ROOMSERVER_NAMESPACE_END

