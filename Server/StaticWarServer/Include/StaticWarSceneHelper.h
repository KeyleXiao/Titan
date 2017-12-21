/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Include\StaticWarSceneHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	15:54
** 版  本:	1.0
** 描  述:	
            所有战场数据服务 相关的常用函数包装
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IStaticWarScene.h"
#include "IStaticWarSceneService.h"
#include "IServiceContainer.h"

class StaticWarSceneHelper
{
public:
    SERVICE_PTR m_pContainer;
	IStaticWarSceneService*	m_ptr;

	StaticWarSceneHelper() : m_ptr( nullptr ),m_pContainer(0)
	{
		IStaticWarScene* pStaticWarScene = gServerGlobal->getStaticWarScene();
		if (nullptr == pStaticWarScene)
		{
			return;
		}

        m_pContainer = pStaticWarScene->getStaticWarSceneService();
        if (m_pContainer == 0)
        {
            return;
        }

        m_ptr = (IStaticWarSceneService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			MATCH_ERRORLN(__FUNCTION__": failed!can not found IStaticWarSceneService! ");
			return;
		}
	}
};