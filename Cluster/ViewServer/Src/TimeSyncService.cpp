#include "stdafx.h"
#include "TimeSyncService.h"


ITimeSyncService& gTimeSyncService = TimeSyncService::getInstance();
DWORD TimeSyncService::GetTick()
{
	return m_syncClient.GetTick();
}

bool TimeSyncService::Start(const char * szIP, int nUDPPort, int nTCPPort)
{
	if (nUDPPort == 0 && nTCPPort == 0)
	{
		ErrorLn("Sync Time's Port Config Error!! port=0");
		return false;
	}
	m_syncClient.Start(szIP, nUDPPort, nTCPPort, (TimerAxis*)gGlobalServer->getTimerAxis());
	m_syncClient.Getctime();
	return true;
}