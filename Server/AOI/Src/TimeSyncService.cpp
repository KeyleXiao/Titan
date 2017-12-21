/*******************************************************************
** 文件名:	TimeSyncService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			对时服务
********************************************************************/

#include "stdafx.h"
#include "IServerGlobal.h"
#include "ITimerService.h"
#include "TimeSyncService.h"

DWORD TimeSyncService::GetTick()
{
	return m_syncClient.GetTick();
}

bool TimeSyncService::Start(const char * szIP,int nUDPPort,int nTCPPort)
{
    if(nUDPPort == 0 && nTCPPort == 0)
    {
        ErrorLn("Sync Time's Port Config Error!! port=0");
        return false;
    }
	m_syncClient.Start( szIP,nUDPPort,nTCPPort,(TimerAxis*)gServerGlobal->getTimerService()->getAxis() );
	return true;
}
