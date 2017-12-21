/*******************************************************************
** 文件名:	MailHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:  易耀辉
** 日  期:	2017-2-7
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IMailScene.h"
#include "IMailSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得邮件系统指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class MailHelper
{
public:
	SERVICE_PTR m_pContainer;
	IMailSceneService *m_ptr;

	MailHelper() : m_pContainer(0),m_ptr(0)
	{
		IMailScene *pMailScene = gServerGlobal->getMailScene();
		if (pMailScene == 0)
		{
			return;
		}

		m_pContainer = pMailScene->getMailSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IMailSceneService*)m_pContainer->query_interface();
	}
};
