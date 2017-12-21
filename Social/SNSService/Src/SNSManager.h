/*******************************************************************
** 文件名:	SNSManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:  6/12/2016
** 版  本:	1.0
** 描  述:	
			SNS管理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "ISNSManager.h"
#include <map>

class SNSManager:public ISNSManager
{
public:
	////////////////////////////////ISNSManager//////////////////////////////////////////
	virtual SERVICE_PTR getSNSService(){return m_SNSServices;};
	virtual bool		load();
	virtual void		release();

private:
	SERVICE_PTR m_SNSServices;

private:
	bool CreateSNSService();
};

