/*******************************************************************
** 文件名:	E:\speed\Social\MentorshipServer\Src\MentorshipManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/24/2016	15:40
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "IMentorshipManager.h"
#include <map>

class MentorshipManager:public IMentorshipManager
{
public:
	////////////////////////////////IMentorshipManager//////////////////////////////////////////
	virtual SERVICE_PTR GetMentorshipService(){ return m_MentorshipServices; };
	
	virtual bool		Load();
	virtual void		Release();

private:
	bool CreateMentorshipService();

private:
    SERVICE_PTR m_MentorshipServices;
};

