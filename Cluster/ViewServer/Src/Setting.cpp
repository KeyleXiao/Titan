#include "stdafx.h"
#include "Setting.h"
#include "ViewSettingDef.h"

Setting& gSetting = Setting::getInstance();

// 读入配置文件
#define SETTING_FILE_NAME "ViewServer.ini"
bool Setting::LoadSetting(void)
{
	Ini ini(SETTING_FILE_NAME);
	m_dwTraceLevel = ini.getInt("Setting", "TraceLevel", 3);
	m_bAutoStart = ini.getInt("Setting", "AutoStart", 1);
	m_bAutoHide = ini.getInt("Setting", "AutoHide", 1);
	m_dwMaxConnection = ini.getInt("Setting", "ConnMax", 1000);
	m_strMngIp = ini.getString("Setting", "ViewManagerIP", "127.0.0.1");
	m_dwMngPort = ini.getInt("Setting", "PortForViewServer", DefaultPort::MNG_FOR_VIEW);
	m_dwBasePortForView = ini.getInt("Setting", "GatewayPortForView", DefaultPort::GATE_FOR_VIEW);
	m_strRedisIp = ini.getString("Setting", "RedisIp", "127.0.0.1");;
	m_wRedisPort = ini.getInt("Setting", "RedisPort", 6379);

	ErrorLn(_GT("Setting::LoadSetting 最大连接数：") << m_dwMaxConnection);

	return true;
}

void Setting::SetServerID(DWORD dwServerID)
{
	m_dwServerID = dwServerID;
	m_strWinText.Format(_T("观战Server[%d]"), m_dwServerID);
}
