/*******************************************************************
** 文件名:	speed\Social\MailServer\Src\MailManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "IMailManager.h"
#include <map>

class MailManager:public IMailManager
{
public:
	////////////////////////////////IMailManager//////////////////////////////////////////
	virtual SERVICE_PTR GetMailService(){ return m_MailServices; };
	
	virtual bool		Load();
	virtual void		Release();

private:
	bool CreateMailService();

private:
    SERVICE_PTR m_MailServices;
};

