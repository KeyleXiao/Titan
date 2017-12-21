#include "stdafx.h"
#include "ViewMngSetting.h"
#include "ViewSettingDef.h"
#include "GateList.h"
#include "ViewList.h"

ViewMngSetting& gSetting = ViewMngSetting::getInstance();

// 读入配置文件
bool ViewMngSetting::LoadSetting(void)
{
	Ini ini("ViewManagerServer.ini");
	m_strGameName = ini.getString("Setting", "GameName", "TiTan");
	m_strVersion = ini.getString("Setting", "Version", "1.0");
	m_dwTraceLevel = ini.getInt("Setting", "TraceLevel", 3);
	m_bAutoStart = ini.getInt("Setting", "AutoStart", 1);
	m_bAutoHide = ini.getInt("Setting", "AutoHide", 1);
	m_dwMaxNetSpeed = ini.getInt("Setting", "NetSpeedMax", 1000);
	m_wPortForGate = ini.getInt("Setting", "PortForViewGateway", DefaultPort::MNG_FOR_GATE);
	m_wPortForView = ini.getInt("Setting", "PortForViewServer", DefaultPort::MNG_FOR_VIEW);
	m_wPortForGame = ini.getInt("Setting", "PortForGame", 14852);
	m_strRedisIp = ini.getString("Setting", "RedisIp", "127.0.0.1");;
	m_wRedisPort = ini.getInt("Setting", "RedisPort", 6379);
	m_wMinGateCount = ini.getInt("Setting", "MinGateCount", 1);
	m_wMinViewCount = ini.getInt("Setting", "MinViewCount", 1);

	return true;
}

bool ViewMngSetting::CheckReady()
{
	int nCountGate = gGateList.GetReadyCount();
	if (nCountGate < m_wMinGateCount)
		return false;

	int nCountView = gViewList.GetReadyCount();
	if (nCountView < m_wMinViewCount)
		return false;

	SetReady(true);
	return IsReady();
}