/*******************************************************************
** 文件名:	MentorshipHelper.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:  易耀辉
** 日  期:	2016-11-29
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IMentorshipScene.h"
#include "IMentorshipSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// 获得师徒系统指针
// @param 用这个辅助对象，可以保证在函数执行过程中m_ptr的值不被释放
//////////////////////////////////////////////////////////////////////////////////////////////////
class MentorshipHelper
{
public:
	SERVICE_PTR m_pContainer;
	IMentorshipSceneService *m_ptr;

	MentorshipHelper() : m_pContainer(0),m_ptr(0)
	{
		IMentorshipScene *pMentorshipScene = gServerGlobal->getMentorshipScene();
		if (pMentorshipScene == 0)
		{
			return;
		}

		m_pContainer = pMentorshipScene->getMentorshipSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IMentorshipSceneService*)m_pContainer->query_interface();
	}
};
