/*******************************************************************
** 文件名:	E:\speed\Server\TeamServer\Include\TeamHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/7/2015	10:02
** 版  本:	1.0
** 描  述:	组队相关的常用函数包装
********************************************************************/

#pragma once

#include "ISocialGlobal.h"
#include "ITeamService.h"
#include "ITeamManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"


class TeamHelper
{
public:
	ITeamService*	m_ptr;
    SERVICE_PTR		m_pContainer;

	TeamHelper() : m_ptr( nullptr ), m_pContainer(0)
	{
		ITeamManager* pTeamManager = gSocialGlobal->getTeamManager();
		if (nullptr == pTeamManager)
		{
			return;
		}

		m_pContainer = m_pContainer == 0? pTeamManager->GetTeamService() : m_pContainer;
		if (nullptr == m_pContainer)
		{
			TEAM_ERRORLN("TeamHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (ITeamService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			TEAM_ERRORLN("TeamHelper() failed!can not found ITeamService! ");
			return;
		}
	}
};