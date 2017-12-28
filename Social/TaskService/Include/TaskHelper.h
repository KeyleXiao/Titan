/*******************************************************************
** 文件名:	TaskHelper.h 
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	周坤
** 日  期:	7/10/2017
** 版  本:	1.0
** 描  述:	任务相关常用函数包装
** 应  用:  	
********************************************************************/

#pragma once
#include "ISocialGlobal.h"
#include "ISocialGlobalService.h"
#include "ITaskStorageService.h"
#include "IServiceContainer.h"
#include "ITaskManager.h"

class TaskHelper
{
public:
	ITaskStorageService*	m_ptr;
	SERVICE_PTR				m_pContainer;

	TaskHelper() : m_ptr(nullptr), m_pContainer(0)
	{
		ITaskManager* pTaskManager = gSocialGlobal->getTaskManager();
		if (nullptr == pTaskManager)
		{
			return;
		}

		m_pContainer = m_pContainer == 0? pTaskManager->GetTaskService() : m_pContainer;
		if (nullptr == m_pContainer)
		{
			return;
		}

		m_ptr = (ITaskStorageService*)m_pContainer->query_interface();
	}
};