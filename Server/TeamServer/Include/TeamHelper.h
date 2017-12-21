/*******************************************************************
** 文件名:	E:\speed\Server\TeamServer\Include\TeamHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/7/2015	10:02
** 版  本:	1.0
** 描  述:	组队相关的常用函数包装
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "ITeamSceneService.h"
#include "ITeamScene.h"
#include "IServiceContainer.h"


class TeamHelper
{
public:
	ITeamSceneService*	m_ptr;
    SERVICE_PTR		m_pContainer;

	TeamHelper() : m_ptr( nullptr )
	{
		ITeamScene* pTeamScene = gServerGlobal->getTeamScene();
		if (nullptr == pTeamScene)
		{
			return;
		}

		SERVICE_PTR m_pContainer = pTeamScene->getTeamSceneService();
		if (nullptr == m_pContainer)
		{
			TEAM_ERRORLN("TeamHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (ITeamSceneService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			TEAM_ERRORLN("TeamHelper() failed!can not found ITeamService! ");
			return;
		}
	}
};