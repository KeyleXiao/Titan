/*******************************************************************
** 文件名:	ServerUpdateVerSetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-5-17
** 版  本:	1.0
** 描  述:	服务器更新版本设定信息
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __ServerUpdateVer_SETTING_H__
#define __ServerUpdateVer_SETTING_H__

#include "ManagerFun.h"
#include <time.h>
#include <string>

class ServerUpdateVerSetting
{
public:
	// 读入配置文件
	bool ServerUpdateVerSetting::LoadSetting(const char * szFileName)
	{
		if (szFileName==NULL)
		{
			return false;
		}
		m_strFileName = szFileName;
		Ini ini(szFileName);
		m_dwServerVer  = ini.getInt("Setting","ver",0);
		m_dwServerType  = ini.getInt("Setting","type",0);
		m_dwUpdateTime  = ini.getInt("Setting","date",0);
		m_strTypeName	= ini.getString("Setting","typename","");

		return true;
	}

	// 存储设置
	bool ServerUpdateVerSetting::SaveSetting(void)
	{
		if (m_strFileName.size()<2)
		{
			return false;
		}
		Ini ini(m_strFileName.c_str());
		ini.setInt("Setting","ver",m_dwServerVer);
		ini.setInt("Setting","type",m_dwServerVer);
		ini.setString("Setting","typename",m_strTypeName.c_str());
		ini.setInt("Setting","date",m_dwUpdateTime);
		ini.setString("Setting","datestr",GetTimeString(m_dwUpdateTime));

		return true;
	}

	ServerUpdateVerSetting::ServerUpdateVerSetting() : m_strFileName(""),m_strTypeName(""),m_dwServerType(0),m_dwUpdateTime(0),m_dwServerVer(0){}

public:
	std::string		m_strFileName;				// 配置文件名
	std::string		m_strTypeName;				// 类型名称
	DWORD			m_dwServerVer;				// 服务器更新包版本号
	DWORD			m_dwServerType;				// 服务器更新包类型
	DWORD			m_dwUpdateTime;				// 更新时间
};

#endif//__ServerUpdateVer_SETTING_H__