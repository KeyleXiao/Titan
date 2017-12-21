/*******************************************************************
** 文件名:	E:\speed\Social\LegendCupServer\Src\LegendCupManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/18/2015	14:47
** 版  本:	1.0
** 描  述:	杯赛服务管理类
			
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "ILegendCupManager.h"
#include <map>

class LegendCupManager:public ILegendCupManager
{


public:
	////////////////////////////////ILegendCupManager//////////////////////////////////////////
	virtual SERVICE_PTR getLegendCupService(){return m_LegendCupServices;};
	virtual bool		load();
	virtual void		release();

	

private:
	SERVICE_PTR m_LegendCupServices;

private:
	bool CreateLegendCupService();
};