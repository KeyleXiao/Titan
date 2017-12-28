/*******************************************************************
** �ļ���:	TaskHelper.h 
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	����
** ��  ��:	7/10/2017
** ��  ��:	1.0
** ��  ��:	������س��ú�����װ
** Ӧ  ��:  	
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