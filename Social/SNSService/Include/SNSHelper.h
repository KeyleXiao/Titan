#pragma once

#include "ISocialGlobal.h"
#include "ISNSService.h"
#include "ISNSManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"

class SNSHelper
{
public:
	SERVICE_PTR		m_pService;
	ISNSService*	m_ptr;

	SNSHelper() : m_ptr( nullptr ), m_pService(0)
	{
		ISNSManager* pSNSManager = gSocialGlobal->getSNSManager();
		if (nullptr == pSNSManager)
		{
			return;
		}

		m_pService = m_pService == 0? pSNSManager->getSNSService() : m_pService;
		if (nullptr == m_pService)
		{
			ErrorLn("SNSHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (ISNSService*)m_pService->query_interface();
		if (nullptr == m_ptr)
		{
			ErrorLn("SNSHelper() failed!can not found ISNSService! ");
			return;
		}
	}
};