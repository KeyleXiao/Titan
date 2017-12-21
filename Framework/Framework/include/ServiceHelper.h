/*******************************************************************
** 文件名:	ServiceHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/10/2014
** 版  本:	1.0
** 描  述:	方便代码书写的辅助类

			
********************************************************************/

#pragma once

#include "IFramework.h"
#include "IServiceMgr.h"
#include "ISceneManager.h"

// 辅助封装从服务ID到接口指针

template<typename INTERFACE>
class ServiceHelper
{
public:
	SERVICE_PTR m_pContainer;
	INTERFACE * m_ptr;

	ServiceHelper(SERVICE_ID id) : m_ptr(0)
	{
		m_pContainer = GetFrameworkProc()->get_service_manager()->get_service(id);
		if ( m_pContainer == 0)
		{
			ErrorLn("ServiceHelper don't obtain the service by serviceid="<<id);
		}
		else
		{
			m_ptr = (INTERFACE*)m_pContainer->query_interface();
		}
	}

	ServiceHelper(SERVICE_PTR ptr)  :m_pContainer(ptr), m_ptr(0)
	{
		if ( m_pContainer!=0 )
		{
			m_ptr = (INTERFACE*)m_pContainer->query_interface();
		}
	}
};
//IGameSceneService 
struct IGameSceneService;

// 获得场景指针
class SceneServiceHelper
{
public:
	SERVICE_PTR m_pContainer;
	IGameSceneService * m_ptr;

	SceneServiceHelper(int sceneID) : m_ptr(0)
	{
		m_pContainer = gServerGlobal->getSceneManager()->getScene(sceneID);
		if ( m_pContainer!=0 )
		{
			m_ptr = (IGameSceneService*)m_pContainer->query_interface();
		}
	}
};