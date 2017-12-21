/*******************************************************************
** 文件名:	SocialGlobalHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	

		实体相关的常用函数包装
********************************************************************/


#pragma once

#include "ISocialGlobal.h"
#include "ISocialGlobalService.h"
#include "IServiceMgr.h"
#include "IFramework.h"


// 获得ISocialGlobalService指针
class SocialGlobalHelper
{
public:
	SERVICE_PTR m_pContainer;
	ISocialGlobalService * m_ptr;

	SocialGlobalHelper() : m_ptr(0)
	{
		SERVICE_ID serviceID = gSocialGlobal->getSocialGlobalService();
		if (serviceID == INVALID_SERVICE_ID)
		{
			ErrorLn("SocialGlobalHelper() not get SERVICE_ID, serviceID=0");
			return;
		}

		m_pContainer = gSocialGlobal->getFramework()->get_service_manager()->get_service(serviceID);
		if (m_pContainer == 0)
		{
			ErrorLn("SocialGlobalHelper() not get SERVICE_PTR, m_pContainer=0");
			return;
		}
		m_ptr = (ISocialGlobalService*)m_pContainer->query_interface();
	}
};
