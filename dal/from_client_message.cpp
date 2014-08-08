/*
 * from_client_message.cpp
 *
 *  Created on: 2011-12-17
 *      Author: jimm
 */

#include <memory.h>

#include "from_client_message.h"
#include "../../lightframe/frame_errordef.h"
#include "common/common_codeengine.h"
#include "def/server_errordef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

int32_t COpenPrivateMicReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t COpenPrivateMicReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void COpenPrivateMicReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CClosePrivateMicReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CClosePrivateMicReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CClosePrivateMicReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CP2PMicReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CP2PMicReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CP2PMicReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CP2PAnswerReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nResponseP2PType);
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

int32_t CP2PAnswerReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nResponseP2PType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CP2PAnswerReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nResponseP2PType=%d,nDestRoleID=%d}",
			nResponseP2PType,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t COperateVAReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nOperateAVType);
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

int32_t COperateVAReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nOperateAVType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void COperateVAReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOperateAVType=%d,nDestRoleID=%d}",
			nOperateAVType,nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CLoginReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = strRoomPassword.MessageEncode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nClientInfo);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CLoginReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	//	uint16_t len = 0;
	//	nRet = CCodeEngine::Decode(buf, size, offset, len);
	//	if (0 > nRet)
	//	{
	//		return nRet;
	//	}
	//
	//	if(len > MaxRoomPasswordLength)
	//	{
	//		return E_RS_ROOMPASSWORDTOLONG;
	//	}

	nRet = strRoomPassword.MessageDecode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nClientInfo);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;

}

void CLoginReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{strRoomPassword=%s,nClientInfo=%d}", strRoomPassword.GetString(), nClientInfo);
	offset = (uint32_t)strlen(buf);
}

int32_t CLogoutReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

int32_t CLogoutReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CLogoutReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nothing}");
	offset = (uint32_t)strlen(buf);
}

int32_t CGetUserListReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

int32_t CGetUserListReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CGetUserListReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nothing}");
	offset = (uint32_t)strlen(buf);
}

int32_t CSendTextReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nTextType);
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

int32_t CSendTextReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nTextType);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = strMessage.MessageDecode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nFontSize);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nFontColor);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nFontStyle);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = strFontType.MessageDecode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nAutoAnswer);
	if (0 > nRet)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nRejectAutoAnswer);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CSendTextReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nTextType=%d,nDestID=%d,strMessage=%s,"
			"nFontSize=%d,nFontColor=%d,nFontStyle=%d,strFontType=%s}",
			nTextType, nDestID, strMessage.GetString(), nFontSize, nFontColor,
			nFontStyle, strFontType.GetString());
	offset = (uint32_t)strlen(buf);
}

int32_t CRequestShowReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nRequestShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CRequestShowReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRequestShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CRequestShowReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRequestShowType=%d}",
			nRequestShowType);
	offset = (uint32_t)strlen(buf);
}

int32_t CExitShowReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nExitShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;
}

int32_t CExitShowReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nExitShowType);
	if (0 > nRet)
	{
		return nRet;
	}
	return nRet;

}

void CExitShowReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nExitShowType=%d}",nExitShowType);
	offset = (uint32_t)strlen(buf);
}

int32_t CPushOnShowReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nIndex);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CPushOnShowReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nIndex);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CPushOnShowReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestID=%d,nIndex=%d}",
			nDestID,nIndex);
	offset = (uint32_t)strlen(buf);
}

int32_t CPullOffShowReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CPullOffShowReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CPullOffShowReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestID=%d}",
			nDestID);
	offset = (uint32_t)strlen(buf);
}

int32_t CKickUserReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nKickType);
	if (0 > nRet)
	{
		return nRet;
	}
	if (nKickType == enmKickType_Other)
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

int32_t CKickUserReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nKickType);
	if (0 > nRet)
	{
		return nRet;
	}
	if (nKickType == enmKickType_Other)
	{
		nRet = CCodeEngine::Decode(buf, size, offset, nKickRoleID);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = strReason.MessageDecode(buf, size, offset);
		if (0 > nRet)
		{
			return nRet;
		}

		nRet = CCodeEngine::Decode(buf, size, offset, nTime);
		if (0 > nRet)
		{
			return nRet;
		}
	}

	return S_OK;

}

void CKickUserReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nKickType=%d",nKickType);
	offset = (uint32_t)strlen(buf);
	if(nKickType == enmKickType_Other)
	{
		sprintf(buf + offset, ",nKickRoleID=%d,strReason=%s,nTime=%d}", nKickRoleID,strReason.GetString(), nTime);
		offset = (uint32_t)strlen(buf);
	}

}

int32_t CAddBlackReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CAddBlackReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CAddBlackReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CShutUPReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CShutUPReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CShutUPReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CUnLockShutUPReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;
}

int32_t CUnLockShutUPReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (0 > nRet)
	{
		return nRet;
	}
	return S_OK;

}

void CUnLockShutUPReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nDestRoleID=%d}",
			nDestRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CLockIpReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
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

	return S_OK;
}

int32_t CLockIpReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strReason.MessageDecode(buf, size, offset);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nTime);
	if (0 > nRet)
	{
		return nRet;
	}

	return S_OK;

}

void CLockIpReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "nDestRoleID=%d,strReason=%s,nTime=%d}", nDestRoleID,strReason.GetString(), nTime);
	offset = (uint32_t)strlen(buf);
}

int32_t CAddTitleReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CAddTitleReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Decode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CAddTitleReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "nAddRoleRank=%d,nDestRoleID=%d}",nAddRoleRank ,nDestRoleID );
	offset = (uint32_t)strlen(buf);
}

int32_t CDelTitleReqFromClient::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nDestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CDelTitleReqFromClient::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Decode(buf, size, offset, nAddRoleRank);
	if (0 > nRet)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nDestRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CDelTitleReqFromClient::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "nAddRoleRank=%d,nDestRoleID=%d}",nAddRoleRank ,nDestRoleID );
	offset = (uint32_t)strlen(buf);
}

int32_t CSetRoomReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

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

int32_t CSetRoomReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nSetCount);
	if (0 > nRet)
	{
		return nRet;
	}
	if(nSetCount>MaxSetRoomTypeCount)
	{
		return E_RS_SETROOMISTOBIG;
	}
	nStringCount = 0;
	for(int32_t i = 0;i<nSetCount;i++)
	{
		nRet = CCodeEngine::Decode(buf, size, offset, arrRoomInfoType[i]);
		if (nRet < 0)
		{
			return nRet;
		}
		switch (arrRoomInfoType[i])
		{
			case enmRoomInfoType_FREESHOW_TIME:
			{
				nRet = CCodeEngine::Decode(buf, size, offset, nMicTime);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			case enmRoomInfoType_OPTION:
			{

				nRet = CCodeEngine::Decode(buf, size, offset, nRoomOptionType);
				if (nRet < 0)
				{
					return nRet;
				}
			}
			break;
			default:
			{
				nRet = arrSetString[nStringCount].MessageDecode(buf, size, offset);
				if (0 > nRet)
				{
					return nRet;
				}
				nStringCount++;
			}
			break;
		}
	}
	return S_OK;

}

void CSetRoomReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "nSetCount=%d,arrRoomInfoType={",nSetCount );
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

int32_t CGetWaittingListReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

int32_t CGetWaittingListReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CGetWaittingListReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nothing}");
	offset = (uint32_t)strlen(buf);
}

int32_t CGetRoomInfoReqFromClient::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;
}

int32_t CGetRoomInfoReqFromClient::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	return S_OK;

}

void CGetRoomInfoReqFromClient::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nothing}");
	offset = (uint32_t)strlen(buf);
}

int32_t CSetMicStatusReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nMicStatus);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nMicIndex);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CSetMicStatusReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nMicStatus);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nMicIndex);
	if (nRet < 0)
	{
		return nRet;
	}
	return S_OK;

}

void CSetMicStatusReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nMicStatus=%d,nMicIndex=%d}",nMicStatus,nMicIndex);
	offset = (uint32_t)strlen(buf);
}

int32_t CPlayerHeartBeat::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomCount);
	if (nRet < 0)
	{
		return nRet;
	}

	if(nRoomCount>MaxEnterRoomCount)
	{
		return ENCODE_DECODE_FILED;
	}
	for(int32_t i = 0;i<nRoomCount;i++)
	{
		nRet = CCodeEngine::Encode(buf, size, offset, arrRoomID[i] );
		if (nRet < 0)
		{
			return nRet;
		}
	}
	return S_OK;
}

int32_t CPlayerHeartBeat::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nRoomCount);
	if (nRet < 0)
	{
		return nRet;
	}
	if(nRoomCount>MaxEnterRoomCount)
	{
		return ENCODE_DECODE_FILED;
	}
	for(int32_t i = 0;i<nRoomCount;i++)
	{
		nRet = CCodeEngine::Decode(buf, size, offset, arrRoomID[i] );
		if (nRet < 0)
		{
			return nRet;
		}
	}
	return S_OK;

}

void CPlayerHeartBeat::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoleID=%d,nRoomCount=%d arrRoomID={",nRoleID,nRoomCount);
	offset = (uint32_t)strlen(buf);
	for(int32_t i =0 ;i<nRoomCount&&i<MaxEnterRoomCount;i++)
	{
		sprintf(buf + offset, "%d",arrRoomID[i]);
		offset = (uint32_t)strlen(buf);
		if(i+1!=nRoomCount)
		{
			sprintf(buf + offset, ", ");
			offset = (uint32_t)strlen(buf);
		}
	}
	sprintf(buf + offset, "}}");
	offset = (uint32_t)strlen(buf);
}

int32_t CAnswerPullOnShowReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nInviteRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nAnswerType);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nMicIndex);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CAnswerPullOnShowReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nInviteRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nAnswerType);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nMicIndex);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CAnswerPullOnShowReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nInviteRoleID=%d,nAnswerType=%d nMicIndex=%d",nInviteRoleID,nAnswerType,nMicIndex);
	offset = (uint32_t)strlen(buf);
}

int32_t CUpdateSongListReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
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

int32_t CUpdateSongListReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	for(int32_t i = 0; i < nSongCount; ++i)
	{
		nRet = arrSongName[i].MessageDecode(buf, size, offset);
		if (nRet < 0)
		{
			return nRet;
		}

		nRet = CCodeEngine::Decode(buf, size, offset, arrIsHot[i]);
		if (nRet < 0)
		{
			return nRet;
		}
	}

	return S_OK;

}

void CUpdateSongListReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d, nSongCount=%d",nRoomID, nSongCount);
	offset = (uint32_t)strlen(buf);
	for(int32_t i = 0; i < nSongCount - 1; ++i)
	{
		sprintf(buf + offset, ", {{index=%d}, strSongName=%s, IsHot=%d}", i, arrSongName[i].GetString(), arrIsHot[i]);
		offset = (uint32_t)strlen(buf);
	}
	if(nSongCount > 0)
	{
		sprintf(buf + offset, ", {{index=%d}, strSongName=%s, IsHot=%d}}", nSongCount - 1, arrSongName[nSongCount - 1].GetString(), arrIsHot[nSongCount - 1]);
		offset = (uint32_t)strlen(buf);
	}
	else
	{
		sprintf(buf + offset, "}");
		offset = (uint32_t)strlen(buf);
	}
}

int32_t CGetSongListReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CGetSongListReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CGetSongListReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d,nSongerRoleID=%d}",nRoomID,nSongerRoleID);
	offset = (uint32_t)strlen(buf);
}

int32_t CRequestSongReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Encode(buf, size, offset, nSongerRoleID);
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

int32_t CRequestSongReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nSongerRoleID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strSongName.MessageDecode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = strWishWords.MessageDecode(buf, size, offset);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nSongCount);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CRequestSongReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d,nSongerRoleID=%d,strSongName=%s,strWishWords=%s,nSongCount=%d}",
			nRoomID,nSongerRoleID,strSongName.GetString(),strWishWords.GetString(),nSongCount);
	offset = (uint32_t)strlen(buf);
}

int32_t CProcessOrderReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

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

	return S_OK;
}

int32_t CProcessOrderReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	nRet = CCodeEngine::Decode(buf, size, offset, nIsAccept);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CProcessOrderReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d,nOrderID=%d,nIsAccept=%d}",nRoomID,nOrderID,nIsAccept);
	offset = (uint32_t)strlen(buf);
}

int32_t CGetOrderListReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CGetOrderListReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nRoomID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CGetOrderListReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nRoomID=%d}",nRoomID);
	offset = (uint32_t)strlen(buf);
}

int32_t CFightForTicketReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;
}

int32_t CFightForTicketReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CFightForTicketReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%d}",nOrderID);
	offset = (uint32_t)strlen(buf);
}

int32_t CSongVoteReq::MessageEncode(uint8_t* buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;

	nRet = CCodeEngine::Encode(buf, size, offset, nOrderID);
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

int32_t CSongVoteReq::MessageDecode(const uint8_t *buf, const uint32_t size, uint32_t& offset)
{
	int32_t nRet = S_OK;
	nRet = CCodeEngine::Decode(buf, size, offset, nOrderID);
	if (nRet < 0)
	{
		return nRet;
	}
	nRet = CCodeEngine::Decode(buf, size, offset, nSongVoteResult);
	if (nRet < 0)
	{
		return nRet;
	}

	return S_OK;

}

void CSongVoteReq::Dump(char* buf, const uint32_t size, uint32_t& offset)
{
	sprintf(buf + offset, "{nOrderID=%d,nSongVoteResult=%d}",
			nOrderID, nSongVoteResult);
	offset = (uint32_t)strlen(buf);
}

FRAME_ROOMSERVER_NAMESPACE_END

