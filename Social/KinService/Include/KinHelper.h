/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Include\KinHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/7/2015	10:02
** 版  本:	1.0
** 描  述:	战队相关的常用函数包装
********************************************************************/

#pragma once

#include "ISocialGlobal.h"
#include "IKinService.h"
#include "IKinManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"


class KinHelper
{
public:
	IKinService*	m_ptr;
    SERVICE_PTR	m_pContainer;

	KinHelper() : m_ptr( nullptr )
	{
		IKinManager* pKinManager = gSocialGlobal->getKinManager();
		if (nullptr == pKinManager)
		{
			return;
		}

		m_pContainer = m_pContainer == 0? pKinManager->GetKinService() : m_pContainer;
		if (nullptr == m_pContainer)
		{
			MATCH_ERRORLN("KinHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IKinService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			MATCH_ERRORLN("KinHelper() failed!can not found IKinService! ");
			return;
		}
	}
};