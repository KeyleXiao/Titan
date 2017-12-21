#include "stdafx.h"
#include "Setting.h"
#include "ViewSettingDef.h"

Setting& gSetting = Setting::getInstance();

// 读入配置文件
#define SETTING_FILE_NAME "ViewGateway.ini"
bool Setting::LoadSetting(void)
{
	Ini ini(SETTING_FILE_NAME);
	m_dwTraceLevel = ini.getInt("Setting", "TraceLevel", 3);
	m_bAutoStart = ini.getInt("Setting", "AutoStart", 1);
	m_bAutoHide = ini.getInt("Setting", "AutoHide", 1);
	m_dwMaxConnection = ini.getInt("Setting", "ConnMax", 1000);
	m_dwServerPort = ini.getInt("Setting", "PortForViewGateway", DefaultPort::MNG_FOR_GATE);
	m_strServerIp = ini.getString("Setting", "ViewManagerIP", "127.0.0.1");
	m_dwBasePortForClient = ini.getInt("Setting", "GatewayPortForClient", 14900);
	m_dwBasePortForView = ini.getInt("Setting", "GatewayPortForView", DefaultPort::GATE_FOR_VIEW);

	ErrorLn(_GT("Setting::LoadSetting 最大连接数：") << m_dwMaxConnection);

	return true;
}

void Setting::SetServerID(WORD dwServerID)
{
	m_wServerID = dwServerID;
	m_strWinText.Format(_T("观战网关[%d]"), m_wServerID);
 
	m_dwClientPort = m_dwBasePortForClient + dwServerID - 1;
	m_dwViewPort = m_dwBasePortForView + dwServerID - 1;

	Ini ini(SETTING_FILE_NAME);
	char szSetting[32] = { 0 };
	sprintf(szSetting, "Setting%d", dwServerID);

	const int clientPort = ini.getInt(szSetting, "ClientPort", 0);
	if (0 != clientPort)
	{
		// 客户端监听端口：如果专门配置了，就用之
		m_dwClientPort = clientPort;
	}

	const int viewPort = ini.getInt(szSetting, "ViewPort", 0);
	if (0 != viewPort)
	{
		// ViewServer监听端口：如果专门配置了，就用之
		m_dwViewPort = viewPort;
	}
}