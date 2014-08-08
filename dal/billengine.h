/*
 * billengine.h
 *
 *  Created on: 2012-2-28
 *      Author: jimm
 */

#ifndef BILLENGINE_H_
#define BILLENGINE_H_

#include "common/common_typedef.h"
#include "common/common_singleton.h"
#include "frame_typedef.h"
#include "def/server_namespace.h"
#include "public_typedef.h"

FRAME_ROOMSERVER_NAMESPACE_BEGIN

class CBillEngine
{
public:
	CBillEngine();
	virtual ~CBillEngine();

public:
	//生成流水号
	void GenerateTransID(char *szTransID);

protected:
	static int32_t ms_nBillNumber;
};

#define  CREATE_BILLENGINE_INSTANCE    CSingleton<CBillEngine>::CreateInstance
#define  GET_BILLENGINE_INSTANCE       CSingleton<CBillEngine>::GetInstance
#define  DESTROY_BILLENGINE_INSTANCE   CSingleton<CBillEngine>::DestroyInstance

#define g_BillEngine		GET_BILLENGINE_INSTANCE()

FRAME_ROOMSERVER_NAMESPACE_END


#endif /* BILLENGINE_H_ */
