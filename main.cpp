/*
 * main.cpp
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#endif
#include "common/common_datetime.h"
#include "common/common_codeengine.h"
#include "common/common_api.h"
#include "common/common_errordef.h"
#include "common/common_singleton.h"
#include "def/dal_def.h"
#include "global.h"
#include "main_server.h"
#include "../lightframe/lightframe.h"


using namespace FRAME_NAMESPACE;
using namespace FRAME_ROOMSERVER_NAMESPACE;


#ifdef _UNITTEST
//单元测试头文件
#include "unittest/UnitTestMain.h"
#endif

#ifndef WIN32

void sigusr1_handle( int nSigVal )
{
	CMainServer::Reload();
	signal(SIGUSR1, sigusr1_handle);
}

void sigusr2_handle(int nSigVal)
{
	CMainServer::Terminate();
	signal(SIGUSR2, sigusr2_handle);
}

void ignore_pipe()
{
	struct sigaction sig;

	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	sigaction(SIGPIPE, &sig, NULL);
}

//初始化为守护进程
void InitDaemon(void)
{
	pid_t pid = 0;

	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	setsid();

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	ignore_pipe();

	if ((pid = fork()) != 0)
	{
		exit(0);
	}

	umask(0);
}

#endif



void print_version()
{
	/*
	printf("%s, %s\n", SERVER_NAME, SERVER_VERSION);
#ifdef DEBUG
	printf("only for debug\n");
#else
	printf("for release\n");
#endif
	*/
	char version[128] = { 0 };
	GET_VERSION(version);

	printf("%s, %s\n", SERVER_NAME_STRING, version);
#ifdef DEBUG
	printf("Build for debug\n");
#else
	printf("Build for release\n");
#endif

#ifdef SVNVER
	printf("svn version no:%d\n", SVNVER);
#endif

#ifdef SVNDIR
	printf("source dir in svn:%s\n", SVNDIR);
#endif

	printf("\n");

	print_version_common_lib();
	print_version_frame_lib();
}

void print_help()
{
	printf("directories:\n");
	printf("\t./config: include all config files\n");
	printf("\t./log: include all log file\n");
	printf("config files:\n");
	printf("\t"SERVER_NAME_STRING".xml: main config file of "SERVER_NAME_STRING"\n");
	printf("log files:\n");
	printf("\t"SERVER_NAME_STRING"_yyyymmdd.log\n");
	printf("\t"SERVER_NAME_STRING"_main_s_s_yyyymmdd.log\n");
	printf("\t"SERVER_NAME_STRING"_netio_s_s_yyyymmdd.log\n");
}

int32_t main(int argc, char** argv)
{
	bool bResume = false;
	bool bDaemon = false;

	if(argc <= 1)
	{
		g_bTestMode = true;
	}
	//读取命令行参数
	if (argc > 1)
	{
		if (0 == strcasecmp((const char*)argv[1], "-v"))
		{
			print_version();
			exit(0);
		}

		if (0 == strcasecmp((const char*)argv[1], "-h"))
		{
			print_help();
			exit(0);
		}

		if (0 == strcasecmp((const char*)argv[1], "-rd"))
		{
			bResume = true;
			bDaemon = true;
		}

		if (0 == strcasecmp(argv[1], "-d"))
		{
			bResume = false;
			bDaemon = true;
		}
	}
#ifndef WIN32
	int lock_fd = open((const char *)argv[0], O_RDONLY);
	if (lock_fd < 0)
	{
		exit(1);
	}

	if (flock(lock_fd, LOCK_EX | LOCK_NB) < 0)
	{
		CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Error, SERVER_NAME_STRING" was launched!\n");
		exit(1);
	}
#endif
#ifndef WIN32
	//初始化为守护进程
	if (bDaemon)
	{
		InitDaemon();
	}

	//安装信号处理函数
	signal(SIGUSR1, sigusr1_handle);
	signal(SIGUSR2, sigusr2_handle);
#endif


	int32_t ret = S_OK;

	CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Notice, SERVER_NAME_STRING" is ready to start!\n");

	//初始化服务器
	ret = CMainServer::Initialize();
	if (0 <= ret)
	{
		CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Notice, SERVER_NAME_STRING" is starting!\n");

#ifdef _UNITTEST
		//启动单元测试
		StartUnitTest();
#else

		//启动服务器
		ret = CMainServer::Start();
		if (0 <= ret)
		{
			CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Notice, SERVER_NAME_STRING" start success!\n");

			//主线程进入运行状态
			CMainServer::Run();

			CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Notice, SERVER_NAME_STRING" terminated!\n");
		}
#endif
	}

	//结束服务器
	ret = CMainServer::Uninitialize();
	if (0 > ret)
	{
		exit(1);
	}

	CFrameLogEngine::WriteLog(LOGPREFIX, enmLogLevel_Notice, SERVER_NAME_STRING" exited safe!\n\n\n");

	exit(0);
}
