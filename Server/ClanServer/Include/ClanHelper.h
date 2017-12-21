/*******************************************************************
** 文件名:	ClanHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	侯森
** 日  期:	2015-05-31
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IClanScene.h"
#include "IClanSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得帮会系统指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class ClanHelper
{
public:
	SERVICE_PTR m_pContainer;
	IClanSceneService *m_ptr;

	ClanHelper() : m_pContainer(0),m_ptr(0)
	{
		IClanScene *pClanScene = gServerGlobal->getClanScene();
		if (pClanScene == 0)
		{
			return;
		}

		m_pContainer = pClanScene->getClanSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IClanSceneService*)m_pContainer->query_interface();
	}
};
