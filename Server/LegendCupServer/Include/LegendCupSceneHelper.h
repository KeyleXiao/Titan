/*******************************************************************
** 文件名:	E:\speed\Server\MatchSceneServer\Include\LegendCupSceneHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/19/2015	15:54
** 版  本:	1.0
** 描  述:	
            比赛大厅场景服 相关的常用函数包装
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "ILegendCupScene.h"
#include "ILegendCupSceneService.h"
#include "IServiceContainer.h"

class LegendCupSceneHelper
{
public:
    SERVICE_PTR m_pContainer;
	ILegendCupSceneService*	m_ptr;

	LegendCupSceneHelper() : m_ptr( nullptr ),m_pContainer(0)
	{
		ILegendCupScene* pLegendCupScene = gServerGlobal->getLegendCupScene();
		if (nullptr == pLegendCupScene)
		{
			return;
		}

        m_pContainer = pLegendCupScene->getLegendCupSceneService();
        if (m_pContainer == 0)
        {
            return;
        }

        m_ptr = (ILegendCupSceneService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			MATCH_ERRORLN(__FUNCTION__": failed!can not found ILegendCupSceneService! ");
			return;
		}
	}
};