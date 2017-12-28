#include "stdafx.h"
#include "TimeSyncService.h"


ITimeSyncService& gTimeSyncService = TimeSyncService::getInstance();

TimeSyncService::~TimeSyncService()
{
	safeDelete(m_pSyncClient);
}
DWORD TimeSyncService::GetTick()
{
	return m_pSyncClient->GetTick();
}

bool TimeSyncService::Start(const char * szIP, int nUDPPort, int nTCPPort)
{
	if (nUDPPort == 0 && nTCPPort == 0)
	{
		ErrorLn("Sync Time's Port Config Error!! port=0");
		return false;
	}

	if (m_pSyncClient == nullptr)
		m_pSyncClient = new TimeSyncClient;

	m_pSyncClient->Start(szIP, nUDPPort, nTCPPort, (TimerAxis*)gGlobalServer->getTimerAxis());
	m_pSyncClient->Getctime();
	return true;
}

void TimeSyncService::Release()
{
	safeDelete(m_pSyncClient);
}