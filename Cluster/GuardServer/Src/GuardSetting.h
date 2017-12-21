/*******************************************************************
** 文件名:	GuardSetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器设定信息
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __GUARD_SETTING_H__
#define __GUARD_SETTING_H__

#include "singleton.h"
#include <string>



// 配置
class GuardSetting : public Singleton<GuardSetting>
{
	// 配置文件名称
	#define DATAGUARD_INI_FILENAME			"DataGuardServer.ini"


public:
	// 读入配置文件
	bool GuardSetting::LoadSetting(void)
	{
		Ini ini(DATAGUARD_INI_FILENAME);

		m_dwServerPort	= ini.getInt("Setting", "ServerPort",	5168);
		m_dwTraceLevel	= ini.getInt("Setting", "TraceLevel",	3);
		m_bAutoStart	= ini.getInt("Setting", "AutoStart",	1);
		m_bAutoHide		= ini.getInt("Setting", "AutoHide",		1);
		m_bEnablePing	= (ini.getInt("Setting", "EnablePing",	1) != 0);

		return true;
	}

	// 存储设置
	bool GuardSetting::SaveSetting(void)
	{
		Ini ini(DATAGUARD_INI_FILENAME);

		ini.setInt("Setting","ServerPort",m_dwServerPort);
		ini.setInt("Setting","TraceLevel",m_dwTraceLevel);
		ini.setInt("Setting","AutoStart",m_bAutoStart);
		ini.setInt("Setting","AutoHide",m_bAutoHide);
		ini.setInt("Setting","EnablePing",m_bEnablePing);

		return true;
	}

	GuardSetting::GuardSetting()
		: m_dwServerPort(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_bEnablePing(TRUE)
		{

		}

public:
	DWORD			m_dwServerPort;				// 服务器监听端口
	DWORD           m_dwTraceLevel;				// trace日志级别
	BOOL			m_bAutoStart;				// 自动启动
	BOOL			m_bAutoHide;				// 自动隐藏窗口
	bool			m_bEnablePing;
};


#endif	//__GUARD_SETTING_H__