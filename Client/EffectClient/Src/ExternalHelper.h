/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/12/2015
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#pragma once

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "IEntity.h"

class CExternalHelper
{
public:
	/** 取得实体
	*/
	IEntity *GetEntity(UID uidEntity);

	/** 启用定时器
	*/
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD nCallTimes=INFINITY_CALL);

	/** 销毁定时器
	*/
	bool KillTimer(DWORD dwTimerID, ITimerHandler *pHandler);

	/** 获取配置接口
	*/
	ISchemeEngine *GetSchemeEngine();
};

extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__