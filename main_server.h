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



typedef uint8_t						ServerCommand;		//����������
enum
{
	enmServerCommand_None			= 0x00,				//��Ч����
	enmServerCommand_Reload			= 0x01,				//���¼�������
	enmServerCommand_Terminate		= 0x02,				//�˳�����
};


typedef uint8_t						ServerStatus;		//������״̬
enum
{
	enmServerStatus_None			= 0x00,				//��ʼ״̬
	enmServerStatus_Initializing	= 0x01,				//���ڳ�ʼ��
	enmServerStatus_Running			= 0x02,				//����
	enmServerStatus_Paused			= 0x03,				//��ͣ
	enmServerStatus_Terminated		= 0x04,				//����ֹ
};



class CMainServer
{
public:
	CMainServer();
	virtual ~CMainServer();

public:
	//��ʼ��������
	static int32_t Initialize();
	//�ָ�������
	static int32_t Resume();
	//���ٷ�����
	static int32_t Uninitialize();

	static int32_t PreStart();
public:
	//���¼��������ļ�
	static int32_t ReloadXmls();

public:
	//����������
	static int32_t Start();
	//��������ʼִ��
	static void Run();
	//���¼��������ļ�
	static void Reload();
	//��ֹ������
	static void Terminate();

	//��ȡ������״̬
	static ServerStatus GetServerStatus();
	//���÷�����״̬
	static void SetServerStatus(ServerStatus eServerStatus);

protected:



protected:
	static ServerCommand		ms_eServerCommand;		//����������
	static ServerStatus			ms_eServerStatus;		//������״̬

};


FRAME_ROOMSERVER_NAMESPACE_END

#endif /* MAIN_SERVER_H_ */
