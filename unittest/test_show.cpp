/*
 *  test_show.cpp
 *
 *  To do：
 *  Created on: 	2012-3-31
 *  Author: 		luocj
 */

#ifdef _UNITTEST
#include <iostream>
#include "test_show.h"
#include "ctl/server_datacenter.h"
#include "public_typedef.h"
#include "dal/dal_room.h"
#include <string.h>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(CTShow,"CTShow");

void CTShow::setUp(void)
{
}

void CTShow::tearDown(void)
{
	UnRegisterCallback();
	//SetCallbackFunction(0);
}
void CTShow::OnCallBack(char *pStr, void *pData)
{
	if (m_callbackFunction!=NULL && strcmp(pStr,"unittest")==0)
	{
		(this->*m_callbackFunction)(pData);
	}
}
void CTShow::Test1_P(void)
{
	ISNEEDRUNTESTCASE("Test1_P");
	cout<<"Test1ok"<<endl;
}

//测试获取房间的信息
void CTShow::Test_GetValueInfoByType(void)
{
	ISNEEDRUNTESTCASE("Test_GetValueInfoByType");
    //创建一个房间用于数据比较
	RoomID nRoomID = 1111;
	RegionID nRegionID = 1;
	RegionType nRegionType = 1;
	ChannelID nChannelID = 1;
	ChannelType nChannelType = 1;
	RoomOptionType nRoomOptionType = 0x00600220;
	RoleID arrRoleID[10] = {11,22,33,44,55,66,77,88,99,111};
	RoleRank arrRoleRank[10] = {2,4,4,2,2,2,4,4,4,1};
	uint32_t arrTime[10] = {0};
	RoleID arrBlackID[3] = {444,7777,888};
    //创建房间提供测试数据
	CRoom *pRoom = NULL;
	RoomIndex nRoomIndex = enmInvalidRoomIndex;
	g_RoomMgt.CreateRoom(nRoomID,pRoom,nRoomIndex);

	uint32_t nNowTime = (uint32_t)CTimeValue::CurrentTime().Seconds();

	if(pRoom==NULL)
	{
       return;
	}
	pRoom->SetChannelType(nChannelType);
	pRoom->SetChannelID(nChannelID);
	pRoom->SetChannelName("test_channel");
	pRoom->SetRegionType(nRegionType);
	pRoom->SetRegionID(nRegionID);
	pRoom->SetRegionName("test_region");
	pRoom->SetRoomType(0);
	pRoom->SetRoomID(nRoomID);
	pRoom->SetRoomName("test_room");
	pRoom->SetMicroTime(3*60);
	pRoom->SetMaxRealUserLimit(750);
	pRoom->SetMaxShowUserLimit(500);
	pRoom->SetHostID(8888);
	pRoom->SetHostName("test_host");
	pRoom->SetHostGender(1);
	pRoom->SetHost179ID(179);
	pRoom->SetRoomFund(10000);
	pRoom->SetRoomOption(nRoomOptionType);
	pRoom->SetRoomPassWord("123");
	pRoom->SetMemberList(arrRoleID, arrRoleRank,arrTime, arrTime,10);
	pRoom->SetRoomNotice("test_notice");
	pRoom->SetRoomSignature("test_signature");
	pRoom->SetRoomWelcome("test_welcome");
	pRoom->SetBlackList(arrBlackID,3);
	pRoom->AddLockIPList(123456,nNowTime+50*60);
	pRoom->AddProhibitList(77777);
	pRoom->AddKickList(7070,nNowTime+50*60,"sb");
	RoleID nOnPublicMic = 3443;
	pRoom->AddOnPublicMicInIndex(nOnPublicMic,1);
	RoleID nOnWaittMic = 3444;
	pRoom->AddToWaittingMic(nOnWaittMic);
	RoleID nOnPrivateMic = 3445;
	pRoom->AddOnPrivateMic(nOnPrivateMic);

    RoomValueType nRoomValueType = enmRoomValueType_All;
	while(nRoomValueType!=enmRoomValueType_AdminList+1)
	{
		uint8_t* pBegin = NULL;
		uint32_t nBeginOffect = 0;
		uint32_t nEndOffect = 0;
		uint32_t nLastDataSize = 0;
		pRoom->GetValueInfoByType(nRoomValueType,pBegin,nBeginOffect,nEndOffect,nLastDataSize);

		//创建一个房间保存被取出来的内容
		CRoom *pTestRoom = NULL;
		RoomIndex nTestRoomIndex = enmInvalidRoomIndex;
		g_RoomMgt.CreateRoom(nRoomID+1+nRoomValueType,pTestRoom,nTestRoomIndex);

		if(pTestRoom==NULL)
		{
			continue;
		}
		uint8_t* pTestBegin = NULL;
		uint32_t nTestBeginOffect = 0;
		uint32_t nTestEndOffect = 0;
		uint32_t nTestLastDataSize = 0;
		pTestRoom->GetValueInfoByType(nRoomValueType,pTestBegin,nTestBeginOffect,nTestEndOffect,nTestLastDataSize);

		uint32_t nSize = nTestEndOffect - nTestBeginOffect + nTestLastDataSize;
		memcpy(pTestBegin,pBegin,nSize);

		//进行值得判断
		switch(nRoomValueType)
		{
			case enmRoomValueType_All:
			{
			    CPPUNIT_ASSERT(pTestRoom->GetMicroTime()== 3*60);
			}
			break;
			case enmRoomValueType_BlackList:
			{
				CPPUNIT_ASSERT(pTestRoom->IsInBlackList(arrBlackID[0]));
				CPPUNIT_ASSERT(pTestRoom->IsInBlackList(arrBlackID[1]));
				CPPUNIT_ASSERT(pTestRoom->IsInBlackList(arrBlackID[2]));
			}
			break;
			case enmRoomValueType_RoomSetInfo:
			{
				CPPUNIT_ASSERT(pTestRoom->GetMicroTime()== 3*60);
			}
			break;
			case enmRoomValueType_KickKist:
			{
				CPPUNIT_ASSERT(pTestRoom->GetEndKickTime(7070)==nNowTime+50*60);
			}
			break;
			case enmRoomValueType_LockIpList:
			{
              uint32_t nTemp = 0;
              CPPUNIT_ASSERT(pTestRoom->IsInLockIPList(123456,nTemp));
			}
			break;
			case enmRoomValueType_RoomCapacity:
			{
				CPPUNIT_ASSERT(pTestRoom->GetMaxRealUserLimit()==750);
				CPPUNIT_ASSERT(pTestRoom->GetMaxShowUserLimit()==500);
			}
			break;
			case enmRoomValueType_PublicMic:
			{
				CPPUNIT_ASSERT(pTestRoom->AddOnPublicMicInIndex(nOnPublicMic,1)!=S_OK);
			}
			break;
			case enmRoomValueType_AllMicList:
			{
				CPPUNIT_ASSERT(pTestRoom->AddOnPublicMicInIndex(nOnPublicMic,1)!=S_OK);
				CPPUNIT_ASSERT(pTestRoom->AddOnPrivateMic(nOnPrivateMic)==E_RS_INPRIVATEMIC);
			}
			break;
			case enmRoomValueType_ShutUpList:r
			{
				CPPUNIT_ASSERT(pTestRoom->IsInProhibitList(77777));
			}
			break;
			case enmRoomValueType_AdminList:
			{
				for(uint32_t i = 0;i<10;i++)
				{
					CPPUNIT_ASSERT(pTestRoom->GetRoleRank(arrRoleID[i])==arrRoleRank[i]);
				}
			}
			break;
			default:
			break;
		}
		//销毁创建的房间
		g_RoomMgt.DestroyRoom(nRoomID+1+nRoomValueType);
		nRoomValueType++;
	}
	g_RoomMgt.DestroyRoom(nRoomID);
	cout<<"Test_GetValueInfoByType OK"<<endl;
}

FRAME_ROOMSERVER_NAMESPACE_END

#endif
