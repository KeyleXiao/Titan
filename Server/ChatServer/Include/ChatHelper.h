/*******************************************************************
** 文件名:	ChatHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	侯森
** 日  期:	2016-07-18
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IServiceContainer.h"
#include "IChatScene.h"
#include "IChatSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得聊天系统指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class ChatHelper
{
public:
	SERVICE_PTR m_pContainer;
	IChatSceneService *m_ptr;

	ChatHelper() : m_pContainer(0),m_ptr(0)
	{
		IChatScene *pChatScene = gServerGlobal->getChatScene();
		if (pChatScene == 0)
		{
			return;
		}

		m_pContainer = pChatScene->getChatSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IChatSceneService*)m_pContainer->query_interface();
	}
};
