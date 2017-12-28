#pragma once

#include "ISocialGlobal.h"
#include "ILegendCupService.h"
#include "ILegendCupManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"

class LegendCupHelper
{
public:
	SERVICE_PTR		m_pService;
	ILegendCupService*	m_ptr;

	LegendCupHelper() : m_ptr( nullptr ), m_pService(0)
	{
		ILegendCupManager* pLegendCupManager = gSocialGlobal->getLegendCupManager();
		if (nullptr == pLegendCupManager)
		{
			return;
		}

		m_pService = m_pService == 0? pLegendCupManager->getLegendCupService() : m_pService;
		if (nullptr == m_pService)
		{
			ErrorLn(__FUNCTION__": failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (ILegendCupService*)m_pService->query_interface();
		if (nullptr == m_ptr)
		{
			ErrorLn(__FUNCTION__": failed!can not found ILegendCupService! ");
			return;
		}
	}
};