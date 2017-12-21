/*******************************************************************
** 文件名:	IActionway.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTPSERVICE接口

********************************************************************/

#pragma once

#ifndef __I_ACTION_WAY_H__
#define __I_ACTION_WAY_H__



#include "idbhttpserviceprotocol.h"

using namespace DBHttp;

/*
@brief：该文件是针对
*/

struct IActionway
{
	/*
	@brief: 此接口是针对玩家的调用
	*/
	virtual void OnExectue(DWORD pdbid, SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams) = 0;

	/*
	@brief: 此接口不是针对玩家的调用
	*/
	virtual void OnGodExectue(SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams) = 0;
};


#endif // __I_ACTION_WAY_H__
