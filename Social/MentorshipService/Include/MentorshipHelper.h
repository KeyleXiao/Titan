/*******************************************************************
** 文件名:	E:\speed\Server\KinServer\Include\KinHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/28/2016
** 版  本:	1.0
** 描  述:	师徒相关的常用函数包装
********************************************************************/

#pragma once

#include "ISocialGlobal.h"
#include "IMentorshipService.h"
#include "IMentorshipManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"
#include "MentorshipDef.h"


class MentorshipHelper
{
public:
	IMentorshipService*	m_ptr;
    SERVICE_PTR	m_pContainer;

	MentorshipHelper() : m_ptr( nullptr )
	{
		IMentorshipManager* pMentorshipManager = gSocialGlobal->getMentorshipManager();
		if (nullptr == pMentorshipManager)
		{
			return;
		}

		SERVICE_PTR m_pContainer = pMentorshipManager->GetMentorshipService();
		if (nullptr == m_pContainer)
		{
			MENTORSHIP_ERRORLN("MentorshipHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IMentorshipService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			MENTORSHIP_ERRORLN("MentorshipHelper() failed!can not found IMentorshipService! ");
			return;
		}
	}
};