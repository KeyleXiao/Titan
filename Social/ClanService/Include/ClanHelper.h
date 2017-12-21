/*******************************************************************
** 文件名:	ClanHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/5/2016	10:37
** 版  本:	1.0
** 描  述:	帮会对象 相关的常用函数包装
********************************************************************/

#ifndef __CLANHELPER_H__
#define __CLANHELPER_H__

#include "ISocialGlobal.h"
#include "IClanService.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"
#include "IClanManager.h"

class ClanHelper
{
public:
	SERVICE_PTR		m_pContainer;
	IClanService*	m_ptr;

	ClanHelper() : m_ptr( nullptr )
	{
		IClanManager *pClanManager = gSocialGlobal->getClanManager();
		if (pClanManager == NULL)
			return;

		m_pContainer = pClanManager->getClanService();
		if (m_pContainer == NULL)
			return;

		m_ptr = (IClanService*)m_pContainer->query_interface();
	}
};

#endif  // __CLANEVENT_H__