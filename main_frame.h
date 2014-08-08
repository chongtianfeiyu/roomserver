/*
 * main_frame.h
 *
 *  Created on: 2011-12-12
 *      Author: luocj
 */

#ifndef MAIN_FRAME_H_
#define MAIN_FRAME_H_

#include "common/common_errordef.h"
#include "common/common_singleton.h"
#include "def/server_namespace.h"
#include "frame_logengine.h"
#include "lightframe.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

#define  StaticLogIndex  1

class CMainFrame : public CLightFrame
{
public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	//框架初始话
	int32_t Initialize();
	//恢复框架
	int32_t Resume();
	//销毁框架
	int32_t Terminate();

	//测试程序
	void OnTestRequest();

	void GetRoomArtistReq(uint32_t msgid);
protected:
	void RegistMsg();

	//生成消息头
	static void GenerateMessageHead(uint32_t nMessageID, RoleID nRoleID, EntityType nSourceType,
			EntityType nDestType, MessageHeadSS& stMessageHead, int32_t MessageBodySize);
};

#define	CREATE_MAINFRAME_INSTANCE	CSingleton<CMainFrame>::CreateInstance
#define	GET_MAINFRAME_INSTANCE		CSingleton<CMainFrame>::GetInstance
#define	DESTROY_MAINFRAME_INSTANCE	CSingleton<CMainFrame>::DestroyInstance

#define g_Frame	GET_MAINFRAME_INSTANCE()


#define WRITE_LOG(level, fmt, ...) \
	do{ g_Frame.WriteLog(level, __FILE__,  __LINE__,""fmt"\n", ##__VA_ARGS__); } while (false)

#define WRITE_DEBUG_LOG(fmt, ...)		WRITE_LOG(enmLogLevel_Debug, fmt, ##__VA_ARGS__)
#define WRITE_INFO_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Info, fmt, ##__VA_ARGS__)
#define WRITE_NOTICE_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Notice, fmt, ##__VA_ARGS__)
#define WRITE_WARNING_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Warning, fmt, ##__VA_ARGS__)
#define WRITE_ERROR_LOG(fmt, ...)	WRITE_LOG(enmLogLevel_Error, fmt, ##__VA_ARGS__)

#define _WRITE_LOG(level, fmt, ...)\
		do{ g_Frame.WriteUserLog("player_count_sta",level, __FILE__,  __LINE__,""fmt"\n", ##__VA_ARGS__); } while (false)

#define WRITE_STATIC_DEBUG_LOG(fmt, ...)		_WRITE_LOG(enmLogLevel_Debug, fmt, ##__VA_ARGS__)
#define WRITE_STATIC_INFO_LOG(fmt, ...)	_WRITE_LOG(enmLogLevel_Info, fmt, ##__VA_ARGS__)
#define WRITE_STATIC_NOTICE_LOG(fmt, ...)	_WRITE_LOG(enmLogLevel_Notice, fmt, ##__VA_ARGS__)
#define WRITE_STATIC_WARNING_LOG(fmt, ...)	_WRITE_LOG(enmLogLevel_Warning, fmt, ##__VA_ARGS__)
#define WRITE_STATIC_ERROR_LOG(fmt, ...)	_WRITE_LOG(enmLogLevel_Error, fmt, ##__VA_ARGS__)

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* MAIN_FRAME_H_ */
