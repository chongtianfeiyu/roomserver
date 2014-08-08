/*
 * main_server.h
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#ifndef MAIN_SERVER_H_
#define MAIN_SERVER_H_

#include "def/server_namespace.h"
#include "../lightframe/frame_typedef.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN



typedef uint8_t						ServerCommand;		//服务器命令
enum
{
	enmServerCommand_None			= 0x00,				//无效命令
	enmServerCommand_Reload			= 0x01,				//重新加载命令
	enmServerCommand_Terminate		= 0x02,				//退出命令
};


typedef uint8_t						ServerStatus;		//服务器状态
enum
{
	enmServerStatus_None			= 0x00,				//初始状态
	enmServerStatus_Initializing	= 0x01,				//正在初始化
	enmServerStatus_Running			= 0x02,				//运行
	enmServerStatus_Paused			= 0x03,				//暂停
	enmServerStatus_Terminated		= 0x04,				//已终止
};



class CMainServer
{
public:
	CMainServer();
	virtual ~CMainServer();

public:
	//初始化服务器
	static int32_t Initialize();
	//恢复服务器
	static int32_t Resume();
	//销毁服务器
	static int32_t Uninitialize();

	static int32_t PreStart();
public:
	//重新加载配置文件
	static int32_t ReloadXmls();

public:
	//启动服务器
	static int32_t Start();
	//服务器开始执行
	static void Run();
	//重新加载配置文件
	static void Reload();
	//终止服务器
	static void Terminate();

	//获取服务器状态
	static ServerStatus GetServerStatus();
	//设置服务器状态
	static void SetServerStatus(ServerStatus eServerStatus);

protected:



protected:
	static ServerCommand		ms_eServerCommand;		//服务器命令
	static ServerStatus			ms_eServerStatus;		//服务器状态

};


FRAME_ROOMSERVER_NAMESPACE_END

#endif /* MAIN_SERVER_H_ */
