/*******************************************************************
** 文件名:	ITimeSyncService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			对时服务
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "buffer.h"

struct ITimeSyncService
{
	virtual DWORD GetTick() = 0;
};
