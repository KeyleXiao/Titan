#pragma once
#include "TimeSyncClient.h"
#include "ITimeSyncService.h"


class TimeSyncService : public Singleton<TimeSyncService>, public ITimeSyncService
{
public:
	virtual	DWORD GetTick();

	// ȡ�������� ��1970��1��1��00:00:00�������ܹ�������
	virtual	inline STime GetTime() { return m_syncClient.GetTime(); }

	virtual	bool Start(const char * szIP = "127.0.0.1", int nUDPPort = 6600, int nTCPPort = 6601);

	virtual	void release() { delete this; }

private:
	TimeSyncClient m_syncClient;
};
