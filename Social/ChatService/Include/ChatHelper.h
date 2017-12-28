#pragma once

#include "ISocialGlobal.h"
#include "IChatService.h"
#include "IChatManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"

class ChatHelper
{
public:
	SERVICE_PTR		m_pService;
	IChatService*	m_ptr;

	ChatHelper() : m_ptr( nullptr ), m_pService(0)
	{
		IChatManager* pChatManager = gSocialGlobal->getChatManager();
		if (nullptr == pChatManager)
		{
			return;
		}

		m_pService = m_pService == 0? pChatManager->getChatService() : m_pService;
		if (nullptr == m_pService)
		{
			CHAT_ERRORLN("ChatHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IChatService*)m_pService->query_interface();
		if (nullptr == m_ptr)
		{
			CHAT_ERRORLN("ChatHelper() failed!can not found IChatService! ");
			return;
		}
	}
};