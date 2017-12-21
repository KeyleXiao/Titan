/*******************************************************************
** 文件名:	ServerGlobalHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	

		实体相关的常用函数包装
********************************************************************/


#pragma once

#include "IServerGlobal.h"
#include "IServerGlobalService.h"
#include "IServiceMgr.h"
#include "IFramework.h"


// 获得IServerGlobalService指针
class ServerGlobalHelper
{
public:
	SERVICE_PTR m_pContainer;
	IServerGlobalService * m_ptr;

	ServerGlobalHelper() : m_ptr(0)
	{
        if(!gServerGlobal)
        {
            return;
        }
		SERVICE_ID serviceID = gServerGlobal->getServerGlobalService();
		if (serviceID == INVALID_SERVICE_ID)
		{
			ErrorLn("ServerGlobalHelper() not get SERVICE_ID, serviceID=0");
			return;
		}

		m_pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service(serviceID);
		if (m_pContainer == 0)
		{
			ErrorLn("ServerGlobalHelper() not get SERVICE_PTR, m_pContainer=0");
			return;
		}
		m_ptr = (IServerGlobalService*)m_pContainer->query_interface();
	}
};
