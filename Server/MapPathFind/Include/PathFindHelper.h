/*******************************************************************
** 文件名:	PathFindHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-05-21
** 版  本:	1.0
** 描  述:	

		实体相关的常用函数包装
********************************************************************/


#pragma once

//#include "IEntity.h"
#include "IServerGlobal.h"
#include "ServiceHelper.h"
#include "ISceneLogicService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得寻路指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class PathFindHelper
{
public:
	SERVICE_PTR m_pContainer;
	IPathFindService* m_ptr;

	PathFindHelper(int nSceneID) : m_pContainer(0),m_ptr(0)
	{
		ISceneManager * pSceneManager = gServerGlobal->getSceneManager();
		if (pSceneManager == 0)
		{
			return;
		}
		SERVICE_PTR pGameSceneService = pSceneManager->getScene(nSceneID);
		if (pGameSceneService == 0)
		{
			return;
		}
		IGameSceneService * pScene = (IGameSceneService*)pGameSceneService->query_interface();
		if (pScene == 0)
		{
			return;
		}
        UINT_PTR serviceID = pScene->getLogicServiceID();
		if (serviceID == INVALID_SERVICE_ID)
		{
			return;
		}
		m_pContainer = gServerGlobal->getFramework()->get_service_manager()->get_service( SERVICE_ID(serviceID) );
		if (m_pContainer == 0)
		{
			return;
		}
        ISceneLogicService *pService = (ISceneLogicService*)m_pContainer->query_interface();
        if(pService == 0)
        {
            return;
        }
		m_ptr = pService->getPathFindService();
	}
};
