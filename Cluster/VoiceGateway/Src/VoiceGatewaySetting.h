/*******************************************************************
** 文件名:	VoiceGatewaySetting.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2011-05-16
** 版  本:	1.0
** 描  述:	语音网关设定信息
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __VOICE_SERVER_SETTING_H__
#define __VOICE_SERVER_SETTING_H__

#include "singleton.h"
#include <string>

class VoiceGatewaySetting : public Singleton<VoiceGatewaySetting>
{
public:
	// 读入配置文件
	bool VoiceGatewaySetting::LoadSetting(void)
	{
		Ini ini("VoiceGateway.ini");
		int tracelevel = ini.getInt("Setting","TraceLevel",3);
		int autostart = ini.getInt("Setting","AutoStart",1);
		int autohide  = ini.getInt("Setting","AutoHide",1);
		int enableping= ini.getInt("Setting","EnablePing",1);
		int maxConn	= ini.getInt("Setting","ServerConnMax",1000);
		int serverPort = ini.getInt("Setting","VoiceServerPort",12850);
		string serverIp = ini.getString("Setting","VoiceServerIP", "127.0.0.1");
		int clientBasePort = ini.getInt("Setting","ClientPort",12900);
		int openAudioConnect = ini.getInt("Setting","OpenAudioConnect",1);
		
		m_dwTraceLevel = tracelevel;
		m_bAutoStart   = autostart;
		m_bAutoHide    = autohide;
		m_bEnablePing  = (enableping!=0);
		m_dwMaxConnection = maxConn;
		m_dwServerPort = serverPort;
		m_strServerIp = serverIp;
		m_dwClientBasePort = clientBasePort;
		m_bOpenAudioConnect = ( openAudioConnect == 1 ) ? true : false;

		ErrorLn(_GT("VoiceGatewaySetting::LoadSetting 最大连接数：") << m_dwMaxConnection);

		if( m_bOpenAudioConnect )
		{
			ErrorLn( _GT("VoiceGatewaySetting::LoadSetting 开启语音专用传输通道") );
		}

		return true;
	}

	void VoiceGatewaySetting::SetServerID( DWORD dwServerID )
	{
		m_dwServerID = dwServerID;

		Ini ini("VoiceGateway.ini");

		char szSetting[32] = {0};
		sprintf( szSetting, "Setting%d", dwServerID );
		
		m_dwClientPort = m_dwClientBasePort + dwServerID - 1;

		int clientPort  = ini.getInt(szSetting,"ClientPort", 0);

		if( 0 != clientPort )
		{
			m_dwClientPort = clientPort;				// 客户端监听端口
		}
	};

	VoiceGatewaySetting::VoiceGatewaySetting() : 
		  m_dwClientPort(0)
	    , m_dwClientBasePort(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_bEnablePing(TRUE)
		, m_dwMaxConnection(0)
		, m_dwServerID(0)
		, m_dwServerPort(0)
		, m_bOpenAudioConnect(false)
	{
		LoadSetting();
	}

public:
	bool			m_bOpenAudioConnect;		// 是否打开语音通道
	DWORD			m_dwClientPort;				// 客户端监听端口
	DWORD			m_dwClientBasePort;			// 客户端监听基础端口,默认网关客户端监听端口为:基础端口+网关ID-1
	string			m_strServerIp;				// 语音服务器IP
	DWORD			m_dwServerPort;				// 语音服务器端口
	DWORD           m_dwTraceLevel;				// trace日志级别
	BOOL			m_bAutoStart;				// 自动启动
	BOOL			m_bAutoHide;				// 自动隐藏窗口
	BOOL			m_bIsHide;
	bool			m_bEnablePing;

	DWORD			m_dwMaxConnection;			// 最大连接数

	DWORD			m_dwServerID;				// 本服务器ID
};

#endif//__VOICE_SERVER_SETTING_H__