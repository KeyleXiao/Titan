/*******************************************************************
** 文件名:	TimeSyncService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			对时服务
********************************************************************/

#pragma once

#include "ITimeSyncService.h"
#include "TimeSyncClient.h"
#include "IServerGlobal.h"
#include "IFramework.h"

class TimeSyncService : public ITimeSyncService
{
public:
	virtual DWORD GetTick();

	bool TimeSyncService::Start(const char * szIP,int nUDPPort,int nTCPPort);


	void release() { gServerGlobal->getFramework()->remove_service_timesync();  delete this; }

private:
	TimeSyncClient m_syncClient;
};
