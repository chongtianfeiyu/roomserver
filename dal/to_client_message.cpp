/*
 * to_client_message.cpp
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#include <memory.h>

//#include "def_message_body.h"
#include "to_client_message.h"
#include "../../lightframe/frame_errordef.h"
#include "common/common_codeengine.h"
#include "def/server_errordef.h"
#include <arpa/inet.h>

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t CLoginResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}

	if(nResult > 0)
	{
		nRet = strFailReason.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}

		if(nResult == enmLoginResult_Kicked||nResult == enmLoginResult_IPLocked||nResult == enmLoginResult_MaxRoomNum || nResult == enmLoginResult_RoomLocked)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, nKickedLeftTime);
			if (0 > nRet)
			{
				return nRet;
			}
		}
	}
	else
	{
		nRet = strRoomName.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = strRoomSignature.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nTelMediaIP);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nTelMediaPort);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nCncMediaIP);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nCncMediaPort);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = strHostName.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nHost179ID);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nHostGender);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nKey);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nPublicMicCount);
		if (0 > nRet)
		{
			return nRet;
		}
		if(nPublicMicCount > MaxPublicMicCount)
		{
			return ENCODE_DECODE_FILED;
		}
		for(int32_t i = 0;i<nPublicMicCount;i++)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nRoleID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nGender);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].n179ID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = stRoomUserInfo[i].strRoleName.MessageEncode(buf, size, offset);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nVipLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nUserLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nRoleRank);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nIdentityType);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nClientInfo);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nStatus);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nItemCount);
			if (0 > nRet)
			{
				return nRet;
			}
			if(stRoomUserInfo[i].nItemCount>MaxOnUserItemCount)
			{
				return ENCODE_DECODE_FILED;
			}
			for(int32_t j=0;j<stRoomUserInfo[i].nItemCount;j++)
			{
				nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].arrItemID[j]);
				if (0 > nRet)
				{
					return nRet;
				}
			}
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo[i].nMagnateLevel);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nIsCollected);
		if (0 > nRet)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CLoginResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void CLoginResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d,", nResult);
	offset = (uint32_t)strlen(buf);
	if(nResult > 0)
	{
		if(nResult == enmLoginResult_Kicked||nResult == enmLoginResult_IPLocked||nResult == enmLoginResult_MaxRoomNum || nResult == enmLoginResult_RoomLocked)
		{
			sprintf(buf + offset, "strFailReason=%s, nKickedLeftTime=%d}", strFailReason.GetString(), nKickedLeftTime);
		}
		else
		{
			sprintf(buf + offset, "strFailReason=%s}", strFailReason.GetString());
		}
	}
	else
	{
		char strTelIP[16]={'\0'};
		char strCncIP[16]={'\0'};
		sprintf(strTelIP,"%s",inet_ntoa_f(nTelMediaIP));
		sprintf(strCncIP,"%s",inet_ntoa_f(nCncMediaIP));

		sprintf(buf + offset, "strRoomName=%s,strRoomSignature=%s,nTelMediaIP=%s,"
				"nTelMediaPort=%d,nCncMediaIP=%s,nCncMediaPort=%d,strHostName=%s,nHost179ID=%d,nHostGender=%d,nKey=%d,",
				strRoomName.GetString(), strRoomSignature.GetString(),
				strTelIP, nTelMediaPort, strCncIP, nCncMediaPort,strHostName.GetString(),nHost179ID,nHostGender,nKey);
	}
	offset = (uint32_t)strlen(buf);

	sprintf(buf + offset, "nPublicMicCount=%d,", nPublicMicCount);
	offset = (uint32_t)strlen(buf);

	for(int32_t i = 0; i < nPublicMicCount && i < MaxPublicMicCount;i++)
	{
		stRoomUserInfo[i].Dump(buf,size,offset);
		offset = (uint32_t)strlen(buf);
		if(i+1 >= nPublicMicCount || i+1 >= MaxPublicMicCount)
		{
			sprintf(buf + offset, "},");
		}
		else
		{
			sprintf(buf + offset, ",");
		}
		offset = (uint32_t)strlen(buf);
	}
	sprintf(buf + offset, "nIsCollected=%d}", nIsCollected);
	offset = (uint32_t)strlen(buf);
}

int32_t CLoginNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nGender);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.n179ID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = stRoomUserInfo.strRoleName.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nVipLevel);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nUserLevel);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nIdentityType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nClientInfo);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nStatus);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nItemCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if(stRoomUserInfo.nItemCount>MaxOnUserItemCount)
	{
		return ENCODE_DECODE_FILED;
	}
	for(int32_t i=0;i<stRoomUserInfo.nItemCount;i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.arrItemID[i]);
		if (0 > nRet)
		{
			return nRet;
		}
	}
	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nMagnateLevel);
	if (0 > nRet)
	{
		return nRet;
	}
//	nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nGiftStarCount);
//	if (0 > nRet)
//	{
//		return nRet;
//	}
//	if(stRoomUserInfo.nGiftStarCount>MaxGiftStarCount)
//	{
//		return ENCODE_DECODE_FILED;
//	}
//	for(int32_t i=0;i<stRoomUserInfo.nGiftStarCount;i++)
//	{
//		nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.arrGiftStarItemID[i]);
//		if (0 > nRet)
//		{
//			return nRet;
//		}
//	}

	return S_OK;
}

int32_t CLoginNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void CLoginNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	stRoomUserInfo.Dump(buf,size,offset);
}

int32_t CLogoutNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CLogoutNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void CLogoutNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoleID=%d}", nRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t COperateVAResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nOperateResult);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t COperateVAResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void COperateVAResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOperateResult=%d}", nOperateResult);
	offset = (uint32_t)strlen(buf);
}

int32_t COperateVANotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nOperateAVType);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t COperateVANotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void COperateVANotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOperateAVType=%d,nSrcRoleID=%d}", nOperateAVType,nSrcRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CGetUserListResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}

	if(nResult > 0)
	{
		nRet = strFailReason.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}
	}
	else
	{
		nRet = CCodeEngine::Encode(buf, size, offset, nEndFlag);
		if (0 > nRet)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, nCount);
		if (0 > nRet)
		{
			return nRet;
		}

		for(int32_t i = 0; i < nCount; ++i)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nRoleID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nGender);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].n179ID);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = arrRoomUserInfo[i].strRoleName.MessageEncode(buf, size, offset);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nVipLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nUserLevel);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nRoleRank);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nIdentityType);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nClientInfo);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nStatus);
			if (0 > nRet)
			{
				return nRet;
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nItemCount);
			if (0 > nRet)
			{
				return nRet;
			}
			if(arrRoomUserInfo[i].nItemCount>MaxOnUserItemCount)
			{
				return ENCODE_DECODE_FILED;
			}
			for(int32_t j=0;j<arrRoomUserInfo[i].nItemCount;j++)
			{
				nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].arrItemID[j]);
				if (0 > nRet)
				{
					return nRet;
				}
			}
			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nMagnateLevel);
			if (0 > nRet)
			{
				return nRet;
			}
//			nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].nGiftStarCount);
//			if (0 > nRet)
//			{
//				return nRet;
//			}
//			if(arrRoomUserInfo[i].nGiftStarCount>MaxGiftStarCount)
//			{
//				return ENCODE_DECODE_FILED;
//			}
//			for(int32_t j=0;j<arrRoomUserInfo[i].nGiftStarCount;j++)
//			{
//				nRet = CCodeEngine::Encode(buf, size, offset, arrRoomUserInfo[i].arrGiftStarItemID[j]);
//				if (0 > nRet)
//				{
//					return nRet;
//				}
//			}
		}
	}

	return S_OK;
}

int32_t CGetUserListResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void CGetUserListResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d,", nResult);
	offset = (uint32_t)strlen(buf);

	if(nResult > 0)
	{
		sprintf(buf + offset, "strFailReason=%s}", strFailReason.GetString());
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, "nEndFlag=%d,nCount=%d,arrRoomUserInfo={", nEndFlag, nCount);
		offset = (uint32_t)strlen(buf);

//		int32_t i = 0;
//		for(; i < nCount - 1; ++i)
//		{
//			sprintf(buf + offset, "{nRoleID=%d,nGender=%d,n179ID=%d,strRoleName=%s,nVipLevel=%d,"
//					"nUserLevel=%d,nRoleRank=%d,nIdentityType=%d,nClientInfo=%d,nStatus=%d,nItemCount=%d",
//					arrRoomUserInfo[i].nRoleID, arrRoomUserInfo[i].nGender,arrRoomUserInfo[i].n179ID, arrRoomUserInfo[i].strRoleName.GetString(),
//					arrRoomUserInfo[i].nVipLevel, arrRoomUserInfo[i].nUserLevel, arrRoomUserInfo[i].nRoleRank,
//					arrRoomUserInfo[i].nIdentityType, arrRoomUserInfo[i].nClientInfo, arrRoomUserInfo[i].nStatus,
//					arrRoomUserInfo[i].nItemCount);
//			offset = (uint32_t)strlen(buf);
//			for(int32_t j = 0;j<arrRoomUserInfo[i].nItemCount;j++)
//			{
//				sprintf(buf + offset, "0x%8X",
//						arrRoomUserInfo[i].arrItemID[j]);
//				offset = (uint32_t)strlen(buf);
//				if(j+1!=arrRoomUserInfo[i].nItemCount)
//				{
//					sprintf(buf + offset, ",");
//					offset = (uint32_t)strlen(buf);
//				}
//			}
//			sprintf(buf + offset, "}");
//			offset = (uint32_t)strlen(buf);
//		}

//		sprintf(buf + offset, "{nRoleID=%d,nGender=%d,n179ID=%d,strRoleName=%s,nVipLevel=%d,"
//				"nUserLevel=%d,nRoleRank=%d,nIdentityType=%d,nClientInfo=%d,nStatus=%d,nItemCount=%d",
//				arrRoomUserInfo[i].nRoleID, arrRoomUserInfo[i].nGender,arrRoomUserInfo[i].n179ID, arrRoomUserInfo[i].strRoleName.GetString(),
//				arrRoomUserInfo[i].nVipLevel, arrRoomUserInfo[i].nUserLevel, arrRoomUserInfo[i].nRoleRank,
//				arrRoomUserInfo[i].nIdentityType, arrRoomUserInfo[i].nClientInfo, arrRoomUserInfo[i].nStatus,
//				arrRoomUserInfo[i].nItemCount);
//		offset = (uint32_t)strlen(buf);
//		for(int32_t j = 0;j<arrRoomUserInfo[i].nItemCount;j++)
//		{
//			sprintf(buf + offset, "0x%8X",
//					arrRoomUserInfo[i].arrItemID[j]);
//			offset = (uint32_t)strlen(buf);
//			if(j+1!=arrRoomUserInfo[i].nItemCount)
//			{
//				sprintf(buf + offset, ",");
//				offset = (uint32_t)strlen(buf);
//			}
//		}
//		sprintf(buf + offset, "}");
//		offset = (uint32_t)strlen(buf);
		sprintf(buf + offset, "}}");
		offset = (uint32_t)strlen(buf);
	}
}

int32_t CSendTextResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nResult > 0)
	{
		nRet = strFailReason.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nLeftTime);
		if (0 > nRet)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CSendTextResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

void CSendTextResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d", nResult);
	offset = (uint32_t)strlen(buf);

	if(nResult > 0)
	{
		sprintf(buf + offset, ",strFailReason=%s ,nLeftTime=%d}", strFailReason.GetString(),nLeftTime);
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
}

int32_t CSendTextNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nTextType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = strMessage.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nFontSize);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nFontColor);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nFontStyle);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = strFontType.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nAutoAnswer);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nRejectAutoAnswer);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CSendTextNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CSendTextNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nTextType=%d,nSrcID=%d,nDestID=%d,strMessage=%s,"
			"nFontSize=%d,nFontColor=%d,nFontStyle=%d,strFontType=%s}",
			nTextType, nSrcID, nDestID, strMessage.GetString(), nFontSize, nFontColor,
			nFontStyle, strFontType.GetString());
	offset = (uint32_t)strlen(buf);
}

int32_t CRequestShowResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRequestShowResult);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nRequestShowResult == enmRequestShowResult_Waittinglist)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, nFreeShowOpened);
		if (0 > nRet)
		{
			return nRet;
		}
	}
	else if(nRequestShowResult == enmRequestShowResult_Public)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, nIndex);
		if (0 > nRet)
		{
			return nRet;
		}
		nRet = CCodeEngine::Encode(buf, size, offset, nTime);
		if (0 > nRet)
		{
			return nRet;
		}
	}

	return nRet;
}

int32_t CRequestShowResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CRequestShowResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRequestShowResult=%d,nIndex=%d,nTime=%d,nFreeShowOpened=%d}",
			nRequestShowResult, nIndex, nTime, nFreeShowOpened);
	offset = (uint32_t)strlen(buf);
}

int32_t CRequestShowNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRequestShowResult);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nIndex);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nTime);
	if (0 > nRet)
	{
		return nRet;
	}

	return nRet;
}

int32_t CRequestShowNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CRequestShowNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRequestShowResult=%d,nSrcRoleID=%d,nIndex=%d,nTime=%d}",
			nRequestShowResult,nSrcRoleID,nIndex, nTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CExitShowNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nExitShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CExitShowNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nExitShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;

}

void CExitShowNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d, nExitShowType=%d}",
			nSrcRoleID,nExitShowType);
	offset = (uint32_t)strlen(buf);
}

int32_t CP2PResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nConnectP2PResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CP2PResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CP2PResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nConnectP2PResult=%d}",
			nConnectP2PResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CP2PRequestNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CP2PRequestNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CP2PRequestNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d}",
			nSrcRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CP2PAnswerNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResponseP2PType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CP2PAnswerNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CP2PAnswerNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResponseP2PType=%d,nSrcRoleID=%d,nDestRoleID=%d}",
			nResponseP2PType,nSrcRoleID,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CPushOnShowResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nPushResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CPushOnShowResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CPushOnShowResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nPushResult=%d}",
			nPushResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CPushOnShowNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nNotifyType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nIndex);
	if (0 > nRet)
	{
		return nRet;
	}

	return nRet;
}

int32_t CPushOnShowNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CPushOnShowNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d,nNotifyType=%d,nIndex=%d}",
			nSrcRoleID,nDestRoleID,nNotifyType,nIndex);
	offset = (uint32_t)strlen(buf);
}

int32_t CPullOffShowResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nPullResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CPullOffShowResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CPullOffShowResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nPullResult=%d}",
			nPullResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CPullOffShowNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	return nRet;
}

int32_t CPullOffShowNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CPullOffShowNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d}",
			nSrcRoleID,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CKickUserResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nKickResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CKickUserResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CKickUserResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nKickResult=%d}",nKickResult);
	offset = (uint32_t) strlen(buf);
}

int32_t CKickUserNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nKickAllVisitor);
	if (0 > nRet)
	{
		return nRet;
	}
	if (nKickAllVisitor == enmKickType_Other)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, nKickRoleID);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = strReason.MessageEncode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, nTime);
		if (0 > nRet)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CKickUserNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CKickUserNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nKickAllVisitor=%d",nSrcRoleID,nKickAllVisitor);

	offset = (uint32_t) strlen(buf);
	if(nKickAllVisitor==enmKickType_Other)
	{
		sprintf(buf + offset, ",nKickRoleID=%d,strReason=%s,nTime=%d}", nKickRoleID,strReason.GetString(), nTime);
		offset = (uint32_t)strlen(buf);
	}

}

int32_t CAddBlackResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nAddBlackResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CAddBlackResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CAddBlackResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nAddBlackResult=%d}",
			nAddBlackResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CAddBlackNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CAddBlackNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CAddBlackNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d}",
			nSrcRoleID,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CShutUpResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nShutUpResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CShutUpResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CShutUpResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nShutUpResult=%d}",
			nShutUpResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CShutUpNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nLeftTime);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CShutUpNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CShutUpNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d,nLeftTime=%d}",
			nSrcRoleID,nDestRoleID,nLeftTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CUnLockShutUpResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nUnLockShutUpResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CUnLockShutUpResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CUnLockShutUpResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nUnLockShutUpResult=%d}",
			nUnLockShutUpResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CUnLockShutUpNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CUnLockShutUpNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CUnLockShutUpNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d}",
			nSrcRoleID,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CLockIpResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nLockIpResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CLockIpResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CLockIpResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nLockIpResult=%d}",
			nLockIpResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CLockIpNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = strReason.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nTime);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CLockIpNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CLockIpNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nDestRoleID=%d,strReason=%s,nTime=%d}",
			nSrcRoleID,nDestRoleID,strReason.GetString(),nTime);
	offset = (uint32_t)strlen(buf);
}

int32_t COpenPeivateResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nOpenResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t COpenPeivateResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void COpenPeivateResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOpenResult=%d}",
			nOpenResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CSrvPrivateMicNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if (MaxWatchPrivateMicCount < nCount)
	{
		return E_RS_WATCHPRIVATEMICISTOBIG;
	}
	for (uint32_t i = 0; i < nCount && i < MaxWatchPrivateMicCount; i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrPrivateMicRoleID[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CSrvPrivateMicNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CSrvPrivateMicNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nCount=%d,arrPrivateMicRoleID={",nCount);

	offset = (uint32_t) strlen(buf);
	for (uint32_t i = 0; i < nCount && i < MaxWatchPrivateMicCount; ++i) {
		sprintf(buf + offset, "%d", arrPrivateMicRoleID[i]);
		offset = (uint32_t) strlen(buf);
		if (i < uint32_t(nCount - 1)) {
			sprintf(buf + offset, ", ");
		}
		offset = (uint32_t) strlen(buf);
	}
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}

int32_t CAddTitleResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nChangeResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CAddTitleResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CAddTitleResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nChangeResult=%d}",
			nChangeResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CAddtitleNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CAddtitleNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CAddtitleNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nAddRoleRank=%d,nSrcRoleID=%d,nDestRoleID=%d",nAddRoleRank,nSrcRoleID,nDestRoleID);

	offset = (uint32_t) strlen(buf);
}

int32_t CDelTitleRespToClient::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nChangeResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CDelTitleRespToClient::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CDelTitleRespToClient::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nChangeResult=%d}",
			nChangeResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CDeltitleNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CDeltitleNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CDeltitleNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nAddRoleRank=%d,nSrcRoleID=%d,nDestRoleID=%d",nAddRoleRank,nSrcRoleID,nDestRoleID);

	offset = (uint32_t) strlen(buf);
}
int32_t CSetRoomResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSetRoomInfoResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CSetRoomResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CSetRoomResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSetRoomInfoResult=%d}",
			nSetRoomInfoResult);
	offset = (uint32_t)strlen(buf);
}
int32_t CSetRoomNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSetCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nSetCount>MaxSetRoomTypeCount)
	{
		return E_RS_SETROOMISTOBIG;
	}
	uint8_t nStringCountTemp = nStringCount;
	for(int32_t i = 0;i<nSetCount;i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrRoomInfoType[i]);
		if (nRet < 0)
		{
			return nRet;
		}
		switch (arrRoomInfoType[i])
		{
			case enmRoomInfoType_FREESHOW_TIME:
			{
				nRet = CCodeEngine::Encode(buf, size, offset, nMicTime);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_OPTION:
			{

				nRet = CCodeEngine::Encode(buf, size, offset, nRoomOptionType);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			default:
			{
				nRet = arrSetString[nStringCount-nStringCountTemp].MessageEncode(buf, size, offset);
				if (0 > nRet)
				{
					return nRet;
				}
				nStringCountTemp--;
			}
			break;
		}
	}

	return S_OK;
}

int32_t CSetRoomNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CSetRoomNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{

	sprintf(buf + offset, "nSrcRoleID=%d,nSetCount=%d,arrRoomInfoType={",nSrcRoleID,nSetCount );
	offset = (uint32_t)strlen(buf);
	for(int32_t i = 0;i<nSetCount;i++)
	{
		sprintf(buf + offset, "%d",arrRoomInfoType[i]);
		offset = (uint32_t)strlen(buf);
		if(i+1<nSetCount)
		{
			sprintf(buf + offset, ",");
		}
		offset = (uint32_t)strlen(buf);
	}
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}

int32_t CGetWaittingListResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nWaittingCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if (MaxWaittingMicCount < nWaittingCount)
	{
		return E_RS_WATCHPRIVATEMICISTOBIG;
	}
	for (uint32_t i = 0; i < nWaittingCount && i < MaxWaittingMicCount; i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrWaittingList[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CGetWaittingListResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{

	return S_OK;

}

void CGetWaittingListResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nWaittingCount=%d,arrWaittingList={",nWaittingCount);

	offset = (uint32_t) strlen(buf);
	for (uint32_t i = 0; i < nWaittingCount && i < MaxWaittingMicCount; ++i) {
		sprintf(buf + offset, "%d", arrWaittingList[i]);
		offset = (uint32_t) strlen(buf);
		if (i < (uint32_t)(nWaittingCount - 1)) {
			sprintf(buf + offset, ", ");
		}
		offset = (uint32_t) strlen(buf);
	}
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}
int32_t CGetRoomInfoRespToClient::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nGetCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nGetCount>MaxGetRoomTypeCount)
	{
		return E_RS_SETROOMISTOBIG;
	}
	uint8_t nStringCountTemp = nStringCount;
	for(int32_t i = 0;i<nGetCount;i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrRoomInfoType[i]);
		if (nRet < 0)
		{
			return nRet;
		}
		switch (arrRoomInfoType[i])
		{
			case enmRoomInfoType_FREESHOW_TIME:
			{
				nRet = CCodeEngine::Encode(buf, size, offset, nMicTime);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_OPTION:
			{

				nRet = CCodeEngine::Encode(buf, size, offset, nRoomOptionType);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_Mic_First_Status:
			{

				nRet = CCodeEngine::Encode(buf, size, offset, nFirstMicStatus);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_Mic_Second_Status:
			{

				nRet = CCodeEngine::Encode(buf, size, offset, nSecondMicStatus);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_Mic_Third_Status:
			{

				nRet = CCodeEngine::Encode(buf, size, offset, nThirdMicStatus);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			default:
			{
				nRet = arrSetString[nStringCount-nStringCountTemp].MessageEncode(buf, size, offset);
				if (0 > nRet)
				{
					return nRet;
				}
				nStringCountTemp--;
			}
			break;
		}
	}

	return S_OK;
}

int32_t CGetRoomInfoRespToClient::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CGetRoomInfoRespToClient::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "nGetCount=%d,arrRoomInfoType={",nGetCount );
	offset = (uint32_t)strlen(buf);
	for(int32_t i = 0;i<nGetCount;i++)
	{
		sprintf(buf + offset, "%d",arrRoomInfoType[i]);
		offset = (uint32_t)strlen(buf);
		if(i+1<nGetCount)
		{
			sprintf(buf + offset, ",");
		}
		offset = (uint32_t)strlen(buf);
	}
	sprintf(buf + offset, "nFirstMicStatus=%d,nSecondMicStatus=%d,nThirdMicStatus=%d",nFirstMicStatus,nSecondMicStatus,nThirdMicStatus);
	offset = (uint32_t)strlen(buf);
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}

int32_t CLockRoomNotifyToClient::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = strReason.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nLockDays);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nEndTime);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CLockRoomNotifyToClient::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CLockRoomNotifyToClient::Dump(char* buf, const uint32_t size, uint32_t& offset)
{

	sprintf(buf + offset, "strReason=%s,nLockDays=%d,nEndTime=%d",strReason.GetString(),nLockDays ,nEndTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CSetMicResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSetMICResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CSetMicResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CSetMicResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSetMICResult=%d}",
			nSetMICResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CSetMicNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nSrcRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nMicStatus);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nMicIndex);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CSetMicNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CSetMicNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nSrcRoleID=%d,nMicStatus=%d,nMicIndex=%d}",
			nSrcRoleID,nMicStatus,nMicIndex);
	offset = (uint32_t)strlen(buf);
}

int32_t CReSetMediaNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nTelMediaIP);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nTelMediaPort);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nCncMediaIP);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nCncMediaPort);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CReSetMediaNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CReSetMediaNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d,nTelMediaIP=%s,nTelMediaPort=%d,nCncMediaIP=%s,nCncMediaPort=%d}",
			nRoomID,inet_ntoa_f(nTelMediaIP),nTelMediaPort,inet_ntoa_f(nCncMediaIP),nCncMediaPort);
	offset = (uint32_t)strlen(buf);
}


int32_t CUpdateUserInfoNotify::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Encode(buf, size, offset, nUpdateCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nUpdateCount>MaxUserInfoCount)
	{
		return -1;
	}
	for(int32_t i = 0;i<nUpdateCount;i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, nType[i]);
		if (0 > nRet)
		{
			return nRet;
		}
		if(nType[i] == enmUserInfoType_Name)
		{
			nRet = stRoomUserInfo.strRoleName.MessageEncode(buf, size, offset);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		else if(nType[i] == enmUserInfoType_179ID)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.n179ID);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		else if(nType[i] == enmUserInfoType_Gender)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nGender);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		else if(nType[i] == enmUserInfoType_VIP_Level)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nVipLevel);
			if (0 > nRet)
			{
				return nRet;
			}
		}
		else if(nType[i] == enmUserInfoType_Magnate_Level)
		{
			nRet = CCodeEngine::Encode(buf, size, offset, stRoomUserInfo.nMagnateLevel);
			if (0 > nRet)
			{
				return nRet;
			}
		}

	}
	return S_OK;
}

int32_t CUpdateUserInfoNotify::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CUpdateUserInfoNotify::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoleID=%d,nUpdateCount=%d,arrUpdateInfo{",
			nRoleID,nUpdateCount);
	offset = (uint32_t)strlen(buf);

	if(nUpdateCount>MaxUserInfoCount)
	{
		return ;
	}
	for(int32_t i = 0;i<nUpdateCount;i++)
	{
		sprintf(buf + offset, "nType=%d",nType[i]);
		offset = (uint32_t)strlen(buf);

		if(nType[i] == enmUserInfoType_Name)
		{
			sprintf(buf + offset, ", value=%s, ",stRoomUserInfo.strRoleName.GetString());
		}
		else if(nType[i] == enmUserInfoType_179ID)
		{
			sprintf(buf + offset, ",value=%d,",stRoomUserInfo.n179ID);
		}
		else if(nType[i] == enmUserInfoType_Gender)
		{
			sprintf(buf + offset, ",value=%d,",stRoomUserInfo.nGender);
		}
		else if(nType[i] == enmUserInfoType_VIP_Level)
		{
			sprintf(buf + offset, ",value=%d,",stRoomUserInfo.nVipLevel);
		}
		else if(nType[i] == enmUserInfoType_Magnate_Level)
		{
			sprintf(buf + offset, ",value=%d,",stRoomUserInfo.nMagnateLevel);
		}
		offset = (uint32_t)strlen(buf);
	}
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}

int32_t CAnswerPullOnShowResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nAnswerPullResult);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CAnswerPullOnShowResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CAnswerPullOnShowResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nAnswerPullResult=%d}",
			nAnswerPullResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CUpdateSongListResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}

	if(nResult != enmUpdateSongListResult_OK)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	for(int32_t i = 0; i < nSongCount; ++i)
	{
		nRet = arrSongName[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrIsHot[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}
	return S_OK;
}

int32_t CUpdateSongListResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CUpdateSongListResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d",nResult);
	offset = (uint32_t)strlen(buf);

	if(nResult != enmUpdateSongListResult_OK)
	{
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, ", nSongCount=%d, SongList={", nSongCount);
		offset = (uint32_t)strlen(buf);

		for(int32_t i = 0; i < nSongCount - 1; ++i)
		{
			sprintf(buf + offset, "index=%d, {strSongName=%s, IsHot=%d}, ", i, arrSongName[i].GetString(), arrIsHot[i]);
			offset = (uint32_t)strlen(buf);
		}
		if(nSongCount > 0)
		{
			sprintf(buf + offset, "index=%d, {strSongName=%s, IsHot=%d}}", nSongCount - 1, arrSongName[nSongCount - 1].GetString(), arrIsHot[nSongCount - 1]);
			offset = (uint32_t)strlen(buf);
		}
		else
		{
			sprintf(buf + offset, "}");
			offset = (uint32_t)strlen(buf);
		}
	}
}

int32_t CGetSongListResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}

	if(nResult != S_OK)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	for(int32_t i = 0; i < nSongCount; ++i)
	{
		nRet = arrSongName[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrIsHot[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}
	return S_OK;
}

int32_t CGetSongListResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CGetSongListResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d",nResult);
	offset = (uint32_t)strlen(buf);

	if(nResult != S_OK)
	{
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, ", nSongerRoleID=%d, nSongCount=%d, SongList={", nSongerRoleID, nSongCount);
		offset = (uint32_t)strlen(buf);

		for(int32_t i = 0; i < nSongCount - 1; ++i)
		{
			sprintf(buf + offset, "index=%d, {strSongName=%s, IsHot=%d}, ", i, arrSongName[i].GetString(), arrIsHot[i]);
			offset = (uint32_t)strlen(buf);
		}
		if(nSongCount > 0)
		{
			sprintf(buf + offset, "index=%d, {strSongName=%s, IsHot=%d}}", nSongCount - 1, arrSongName[nSongCount - 1].GetString(), arrIsHot[nSongCount - 1]);
			offset = (uint32_t)strlen(buf);
		}
		else
		{
			sprintf(buf + offset, "}");
			offset = (uint32_t)strlen(buf);
		}
	}
}

int32_t CRequestSongResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CRequestSongResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CRequestSongResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%d}",nResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CRequestSongNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderTime);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongerName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nRequestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strRequestName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strWishWords.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CRequestSongNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CRequestSongNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nOrderTime=%u,nSongerRoleID=%d,strSongerName=%s,"
			"nRequestRoleID=%d,strRequestName=%s,strSongName=%s,strWishWords=%s,nSongCount=%d}",
			nOrderID,nOrderTime, nSongerRoleID, strSongerName.GetString(), nRequestRoleID,
			strRequestName.GetString(), strSongName.GetString(), strWishWords.GetString(), nSongCount);
	offset = (uint32_t)strlen(buf);
}

int32_t CProcessOrderResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CProcessOrderResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CProcessOrderResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%u}", nResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CProcessOrderNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nIsAccept);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongerName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nRequestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strRequestName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strWishWords.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CProcessOrderNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CProcessOrderNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nIsAccept=%d,nSongerRoleID=%d,strSongerName=%s,"
			"nRequestRoleID=%d,strRequestName=%s,strSongName=%s,strWishWords=%s,nSongCount=%d}",
			nOrderID, nIsAccept, nSongerRoleID, strSongerName.GetString(), nRequestRoleID,
			strRequestName.GetString(), strSongName.GetString(), strWishWords.GetString(), nSongCount);
	offset = (uint32_t)strlen(buf);
}

int32_t CGetOrderListResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (nRet < 0)
	{
		return nRet;
	}

	if(nResult != S_OK)
	{
		return nRet;
	}

	if(nOrderCount > MaxOrderListLength)
	{
		nOrderCount = MaxOrderListLength;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderCount);
	if (nRet < 0)
	{
		return nRet;
	}

	for(int32_t i = 0; i < nOrderCount; ++i)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrOrderID[i]);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrOrderTime[i]);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrSongerRoleID[i]);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = arrSongerName[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrRequestRoleID[i]);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = arrRequestName[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = arrSongName[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = arrWishWords[i].MessageEncode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrOrderStatus[i]);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Encode(buf, size, offset, arrSongCount[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}

	return S_OK;
}

int32_t CGetOrderListResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CGetOrderListResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%u", nResult);
	offset = (uint32_t)strlen(buf);

	if(nResult != S_OK)
	{
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, ", nOrderCount=%d, SongList={", nOrderCount);
		offset = (uint32_t)strlen(buf);

//		for(int32_t i = 0; i < nOrderCount - 1; ++i)
//		{
//			sprintf(buf + offset, "index=%d,{nOrderID=%u,nOrderTime=%u,nSongerRoleID=%d,strSongerName=%s,"
//					"nRequestRoleID=%d,strRequestName=%s,strSongName=%s,strWishWords=%s, nOrderStatus=%d}, ",
//					i, arrOrderID[i],arrOrderTime[i], arrSongerRoleID[i], arrSongerName[i].GetString(),
//					arrRequestRoleID[i], arrRequestName[i].GetString(), arrSongName[i].GetString(),
//					arrWishWords[i].GetString(), arrOrderStatus[i]);
//			offset = (uint32_t)strlen(buf);
//		}
//		if(nOrderCount > 0)
//		{
//			sprintf(buf + offset, "index=%d,{nOrderID=%u,nOrderTime=%u,nSongerRoleID=%d,strSongerName=%s,"
//					"nRequestRoleID=%d,strRequestName=%s,strSongName=%s,strWishWords=%s, nOrderStatus=%d}}",
//					nOrderCount - 1, arrOrderID[nOrderCount - 1],arrOrderTime[nOrderCount - 1], arrSongerRoleID[nOrderCount - 1],
//					arrSongerName[nOrderCount - 1].GetString(), arrRequestRoleID[nOrderCount - 1],
//					arrRequestName[nOrderCount - 1].GetString(), arrSongName[nOrderCount - 1].GetString(),
//					arrWishWords[nOrderCount - 1].GetString(), arrOrderStatus[nOrderCount - 1]);
//			offset = (uint32_t)strlen(buf);
//		}
//		else
		{
			sprintf(buf + offset, "}}");
			offset = (uint32_t)strlen(buf);
		}
	}
}

int32_t CCurSongInfoNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongerName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nRequestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strRequestName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CCurSongInfoNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CCurSongInfoNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nSongerRoleID=%d,strSongerName=%s,"
			"nRequestRoleID=%d,strRequestName=%s,strSongName=%s}",
			nOrderID, nSongerRoleID, strSongerName.GetString(), nRequestRoleID,
			strRequestName.GetString(), strSongName.GetString());
	offset = (uint32_t)strlen(buf);
}

int32_t CTicketCountUpdateNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nTotalTicketCount);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nLeftTicketCount);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nHaveTicket);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nCanGetTicket);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nEndLeftTime);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CTicketCountUpdateNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CTicketCountUpdateNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nTotalTicketCount=%d,nLeftTicketCount=%d,nHaveTicket=%d,"
			"nCanGetTicket=%d,nEndLeftTime=%u}",
			nOrderID, nTotalTicketCount, nLeftTicketCount, nHaveTicket, nCanGetTicket, nEndLeftTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CFightForTicketResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CFightForTicketResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CFightForTicketResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%u}", nResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CEnjoyingNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nHaveTicket);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CEnjoyingNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CEnjoyingNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nHaveTicket=%d}", nOrderID, nHaveTicket);
	offset = (uint32_t)strlen(buf);
}

int32_t CSongVoteResp::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nResult);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CSongVoteResp::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CSongVoteResp::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResult=%u}", nResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CSongVoteUpdateNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongerName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nRequestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strRequestName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongName.MessageEncode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongVoteResult);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CSongVoteUpdateNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CSongVoteUpdateNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u,nSongerRoleID=%d,strSongerName=%s,nRequestRoleID=%d,"
			"strRequestName=%s,strSongName=%s,nSongVoteResult=%d}",
			nOrderID, nSongerRoleID, strSongerName.GetString(), nRequestRoleID,
			strRequestName.GetString(), strSongName.GetString(), nSongVoteResult);
	offset = (uint32_t)strlen(buf);
}

int32_t CWaitingForSongNoti::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CWaitingForSongNoti::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	return nRet;

}

void CWaitingForSongNoti::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%u}", nOrderID);
	offset = (uint32_t)strlen(buf);
}


FRAME_ROOMSERVER_NAMESPACE_END

