/*******************************************************************
** 文件名:	E:\speed\Server\MatchServer\Include\MatchHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	4/7/2015	10:02
** 版  本:	1.0
** 描  述:	
            比赛大厅服 相关的常用函数包装
********************************************************************/

#pragma once

#include "ISocialGlobal.h"
#include "IMatchService.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"
#include "IMatchManager.h"

class MatchHelper
{
public:
	IMatchService*	m_ptr;
	SERVICE_PTR		m_pService;
	MatchHelper() : m_ptr( nullptr ), m_pService(0)
	{
		//ISocialGlobalService * pSocialGlobalService = SocialGlobalHelper().m_ptr;
		//if (nullptr == pSocialGlobalService)
		//{
		//	MATCH_ERRORLN("MatchHelper() failed! can not found ISocialGlobalService!");
		//	return;
		//}

		IMatchManager* pMatchManager = gSocialGlobal->getMatchManager();
		if (nullptr == pMatchManager)
		{
			return;
		}

		m_pService = m_pService == 0? pMatchManager->getMatchService() : m_pService;
		if (nullptr == m_pService)
		{
			MATCH_ERRORLN("MatchHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IMatchService*)m_pService->query_interface();
		if (nullptr == m_ptr)
		{
			MATCH_ERRORLN("MatchHelper() failed!can not found IMatchService! ");
			return;
		}
	}
};