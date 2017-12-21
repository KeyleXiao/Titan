/*******************************************************************
** 文件名:	TimeSync_setting.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	03/02/2009
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __CLUSTER_TIMESYNC_TIMESYNC_TIMESYNC_SETTING_H__
#define __CLUSTER_TIMESYNC_TIMESYNC_TIMESYNC_SETTING_H__


#include "singleton.h"
#include <string>

class CTimeSync_setting : public Singleton<CTimeSync_setting>
{
#ifdef INI_FILE_NAME 
#undef INI_FILE_NAME 
#endif
#define INI_FILE_NAME   "timesync.ini"

public:
	// 读入配置文件
	bool CTimeSync_setting::LoadSetting()
	{
		Ini ini(INI_FILE_NAME);

        // 服务器监听
        m_dwServerUDPPort = ini.getInt("Setting", "ServerUDPPort", 6600);      // 同步UDP端口
        m_dwServerTCPPort = ini.getInt("Setting", "ServerTCPPort", 6601);      // 同步TCP端口
	
		return true;
	}

	// 存储设置
	bool CTimeSync_setting::SaveSetting()
	{
        Ini ini(INI_FILE_NAME);

		ini.setInt("Setting","ServerUDPPort", m_dwServerUDPPort);
		ini.setInt("Setting","ServerTCPPort", m_dwServerTCPPort);

		return true;
	}

    CTimeSync_setting::CTimeSync_setting() : m_dwServerUDPPort(0)
        , m_dwServerTCPPort(0)
	{

	}	

public:
    DWORD			m_dwServerUDPPort;
    DWORD			m_dwServerTCPPort;

};

#endif //__CLUSTER_TIMESYNC_TIMESYNC_TIMESYNC_SETTING_H__