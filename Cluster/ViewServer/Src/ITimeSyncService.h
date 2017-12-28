#pragma once
#include "STime.h"


struct ITimeSyncService
{
public:
	virtual	DWORD GetTick() = 0;

	// 取服务器的 从1970年1月1日00:00:00到现在总共的秒数
	virtual inline STime GetTime() = 0;

	virtual bool Start(const char * szIP = "127.0.0.1", int nUDPPort = 6600, int nTCPPort = 6601) = 0;

	virtual void release() = 0;
};
extern ITimeSyncService& gTimeSyncService;