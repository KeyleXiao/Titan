#pragma once

#include "ISocialGlobal.h"
#include "IOrderService.h"
#include "IOrderManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"

class OrderHelper
{
public:
	SERVICE_PTR		m_pService;
	IOrderService*	m_ptr;

	OrderHelper() : m_ptr( nullptr )
	{
		IOrderManager* pOrderManager = gSocialGlobal->getOrderManager();
		if (nullptr == pOrderManager)
		{
			return;
		}

		m_pService = m_pService == 0? pOrderManager->getOrderService() : m_pService;
		if (nullptr == m_pService)
		{
			ErrorLn(__FUNCTION__": failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IOrderService*)m_pService->query_interface();
		if (nullptr == m_ptr)
		{
			ErrorLn(__FUNCTION__": failed!can not found IOrderService! ");
			return;
		}
	}
};