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

	TaskHelper() : m_ptr(nullptr)
	{
		ITaskManager* pTaskManager = gSocialGlobal->getTaskManager();
		if (nullptr == pTaskManager)
		{
			return;
		}

		m_pContainer = pTaskManager->GetTaskService();
		if (nullptr == m_pContainer)
		{
			return;
		}

		m_ptr = (ITaskStorageService*)m_pContainer->query_interface();
	}
};