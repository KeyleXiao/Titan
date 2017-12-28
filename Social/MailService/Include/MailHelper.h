/*******************************************************************
** 文件名:	E:\speed\Server\MailServer\Include\MailHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	邮件相关的常用函数包装
********************************************************************/

#pragma once

#include "ISocialGlobal.h"
#include "IMailService.h"
#include "IMailManager.h"
#include "ISocialGlobalService.h"
#include "SocialGlobalHelper.h"
#include "MailDef.h"


class MailHelper
{
public:
	IMailService*	m_ptr;
    SERVICE_PTR	m_pContainer;

	MailHelper() : m_ptr( nullptr ), m_pContainer(0)
	{
		IMailManager* pMailManager = gSocialGlobal->getMailManager();
		if (nullptr == pMailManager)
		{
			return;
		}

		m_pContainer = m_pContainer == 0? pMailManager->GetMailService() : m_pContainer;
		if (nullptr == m_pContainer)
		{
			MAIL_ERRORLN("MailHelper() failed! can not found SERVICE_PTR!");
			return;
		}

		m_ptr = (IMailService*)m_pContainer->query_interface();
		if (nullptr == m_ptr)
		{
			MAIL_ERRORLN("MailHelper() failed!can not found IMailService! ");
			return;
		}
	}
};