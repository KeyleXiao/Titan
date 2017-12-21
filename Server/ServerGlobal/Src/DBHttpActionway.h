/*******************************************************************
** 文件名:	IActionway.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2010 - All Rights Reserved
** 创建人:	吕国锋（Richmond）
** 日  期:	09/13/2010
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IActionway.h"
#include "EntityHelper.h"


/*
@brief：DIDA
*/
struct DBHttpActionNotity : public IActionway 
{
	virtual void OnExectue(DWORD pdbid, SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams)
	{
		if (INVALID_PDBID == pdbid)
			return;

		if (value.size() > 7000)
		{
			WarningLn("DBHttpService：DB请求内容太长了（最大7000字节）! len="<<value.c_str());
			return ;
		}

        ActorHelper helper(pdbid);
        if (nullptr == helper.m_ptr)
            return;

        //switch(msg.sub)
        //{

        //default: break;
        //}
	}

	virtual void OnGodExectue(SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams)
	{
	}
};
