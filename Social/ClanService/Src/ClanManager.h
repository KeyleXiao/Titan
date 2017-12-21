/*******************************************************************
** 文件名:	ClanManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	17:19
** 版  本:	1.0
** 描  述:	帮会管理
********************************************************************/

#ifndef __CLANMANAGER_H__
#define __CLANMANAGER_H__

#include "IServiceContainer.h"
#include "IClanManager.h"

class ClanManager:public IClanManager
{
public:
	////////////////////////////////IClanManager//////////////////////////////////////////
	virtual SERVICE_PTR getClanService(){return m_ClanServices;};
	virtual bool		load();
	virtual void		release();

private:
	SERVICE_PTR m_ClanServices;

private:
	bool createClanService();
};

#endif // __CLANMANAGER_H__