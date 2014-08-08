/*
 * main_server.cpp
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */


#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#endif
#include <stdlib.h>
#include <time.h>
#include "common/common_api.h"
#include "def/server_namespace.h"
#include "../lightframe/frame_typedef.h"
#include "common/common_errordef.h"
#include "common/common_singleton.h"
#include "def/dal_def.h"
#include "global.h"
#include "main_server.h"
#include "main_frame.h"
#include "ctl/rebot_ctl.h"
#include "config/someconfig.h"

#include "bll/bll_timer_updateinfotohall.h"
#include "bll/bll_timer_send_color.h"
#include "bll/bll_timer_static_player.h"
#include "bll/bll_timer_sta_inroomtime.h"
#include "bll/bll_timer_checkplayer.h"
#include "bll/bll_timer_getmediaserverinfo.h"


FRAME_ROOMSERVER_NAMESPACE_BEGIN

//��ʼ�������������״̬
ServerCommand CMainServer::ms_eServerCommand = enmServerCommand_None;
ServerStatus CMainServer::ms_eServerStatus = enmServerStatus_None;

CMainServer::CMainServer()
{

}

CMainServer::~CMainServer()
{

}

//��ʼ��������
int32_t CMainServer::Initialize()
{
#ifdef WIN32
	//��ʼ��winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	//��ʼ���������
	srand((unsigned)time(NULL));

//	CSimpleLog& logger = GET_SIMPLELOG_INSTANCE();

	//��ʼ��LibFrame
	CREATE_MAINFRAME_INSTANCE();
	int32_t ret = g_Frame.Initialize();
	if(ret < 0)
	{
		return ret;
	}
	ret = PreStart();

	return ret;
}

int32_t CMainServer::PreStart()
{
	//�����գ������ж�ʱ��
	static int32_t i = 0;
	i++;
	if (i>=2)
	{
		WRITE_ERROR_LOG("PreStart had been called more than two times!");
	}

	//ÿ3����һ��
	int32_t nTimerIndex = enmInvalidTimerIndex;
	int32_t ret = S_OK;
//	ret = g_Frame.SetTimer(&g_TestTimer, enmTestTimePeriod * US_PER_SECOND, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
//
//	 g_Frame.OpenOnlinePlayerMgt(&g_OnlineHandler);

	 //todo : add your code to here

//	ret = g_Frame.SetTimer(&GET_UPDATEINFOTOHALL_TIMER_INSTANCE(), enmUpdateInfoToHallPeriod, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CUpdateInfoToHallTimer::OnTimerEvent), &GET_UPDATEINFOTOHALL_TIMER_INSTANCE(), NULL,enmUpdateInfoToHallPeriod , true, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}

//	ret = g_Frame.SetTimer(&GET_SENDCOLOR_TIMER_INSTANCE(), enmSenColorTimeoutPeriod, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CSendColorTimer::OnTimerEvent), &GET_SENDCOLOR_TIMER_INSTANCE(), NULL,enmSenColorTimeoutPeriod , true, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}

	//���ͳ�Ʒ��������Ķ�ʱ��
//	ret = g_Frame.SetTimer(&GET_STATICPLAYERCOUNT_TIMER_INSTANCE(), enmAddStaticPlayerTimeoutPeriod, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CStaticPlayerCountTimer::OnTimerEvent), &GET_STATICPLAYERCOUNT_TIMER_INSTANCE(), NULL,enmAddStaticPlayerTimeoutPeriod , true, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}

	//���ͳ���ڷ��������ʱ��Ķ�ʱ��
//	ret = g_Frame.SetTimer(&GET_STATICINROOMTIME_TIMER_INSTANCE(), g_SomeConfig.GetOnlineTimeStaTimer()*US_PER_SECOND, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CStaticInRoomTimeTimer::OnTimerEvent), &GET_STATICINROOMTIME_TIMER_INSTANCE(), NULL,g_SomeConfig.GetOnlineTimeStaTimer()*US_PER_SECOND , true, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}
	//��Ӽ������Ƿ�ʱ�Ķ�ʱ��
//	ret = g_Frame.SetTimer(&GET_CHECKPLAYER_TIMER_INSTANCE(), enmCheckPlayerTimeoutPeriod, true, nTimerIndex);
//	if (ret < 0)
//	{
//		return ret;
//	}
	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CCheckPlayerTimer::OnTimerEvent), &GET_CHECKPLAYER_TIMER_INSTANCE(), NULL,enmCheckPlayerTimeoutPeriod , true, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}

	ret = g_Frame.CreateTimer(static_cast<TimerProc>(&CGetMediaServerInfo::OnTimerEvent), &GET_GETMEDIASERVERINFO_TIMER_INSTANCE(), NULL, 5 * US_PER_SECOND, false, nTimerIndex);
	if (ret < 0)
	{
		return ret;
	}

//	WRITE_DEBUG_LOG("time %d",g_RebotConfig.GetSendTime());
	return ret;
}

//�ָ�������
int32_t CMainServer::Resume()
{
#ifdef WIN32
	//��ʼ��winsock
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	//��ʼ���������
	srand((unsigned)time(NULL));

	//��ʼ��LibFrame
	g_Frame.Resume();

	return S_OK;
}

//���ٷ�����
int32_t CMainServer::Uninitialize()
{
	//����LibFrame

#ifdef WIN32
	WSACleanup();
#endif

	//g_Frame().Terminate();
	return S_OK;
}


//���¼��������ļ�
int32_t CMainServer::ReloadXmls()
{
	//֪ͨLibFrame���¼��������ļ�
	g_Frame.Reload();
	return S_OK;
}


//����������
int32_t CMainServer::Start()
{
	//����LibFrame
	g_Frame.Start();
	return S_OK;
}

//��������ʼִ��
void CMainServer::Run()
{
	ServerCommand eServerCommand = enmServerCommand_None;

	while (true)
	{
		eServerCommand = ms_eServerCommand;

		if (enmServerCommand_Reload == eServerCommand)
		{
			//WriteLog(LOGPREFIX, enmLogLevel_Notice, "Activity server received reload signal!\n");
			ms_eServerCommand = enmServerCommand_None;
			ReloadXmls() ;
		}
		else if (enmServerCommand_Terminate == eServerCommand)
		{
			//WriteLog(LOGPREFIX, enmLogLevel_Notice, "Activity server received terminate signal!\n");

			//֪ͨLibFrame��������
			g_Frame.Terminate();
			break;
		}

		if (g_bTest)
		{
//			CTestMain::OnTestRequest();
		}
		//TODO:
		Delay(100000);
	}
}

//���¼��������ļ�
void CMainServer::Reload()
{
	ms_eServerCommand = enmServerCommand_Reload;
}

//��ֹ������
void CMainServer::Terminate()
{
	ms_eServerCommand = enmServerCommand_Terminate;
}

//��ȡ������״̬
ServerStatus CMainServer::GetServerStatus()
{
	return ms_eServerStatus;
}

//���÷�����״̬
void CMainServer::SetServerStatus(ServerStatus eServerStatus)
{
	ms_eServerStatus = eServerStatus;
}

FRAME_ROOMSERVER_NAMESPACE_END

