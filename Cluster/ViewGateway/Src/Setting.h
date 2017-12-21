/*******************************************************************
** 文件名:	ViewGatewaySetting.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	观战网关配置信息
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "singleton.h"
#include <string>

class Setting : public Singleton<Setting>
{
public:
	Setting::Setting() :
		m_dwClientPort(0)
		, m_dwBasePortForClient(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_dwMaxConnection(0)
		, m_wServerID(0)
		, m_dwServerPort(0)
		, m_dwBasePortForView(0)
		, m_dwViewPort(0)
	{
		LoadSetting();
	}

	// 由本地文件目录名而得到的ID
	void			SetServerID( WORD dwServerID );
	inline WORD		getServerID() { return m_wServerID; }

	// Mng为我生成的自增ID
	inline DWORD	GetID() { return m_dwID; }
	void			SetID(DWORD dwID) { m_dwID = dwID; }

private:
	// 读入配置文件
	bool LoadSetting(void);

public:
	WORD			m_dwClientPort;				// 客户端监听端口
	WORD			m_dwViewPort;				// ViewServer监听端口
	DWORD			m_dwMaxConnection;			// 最大连接数
	string			m_strServerIp;				// 观战管理服IP
	WORD			m_dwServerPort;				// 观战管理服端口

	DWORD			m_dwTraceLevel;				// trace日志级别
	BOOL			m_bAutoStart;				// 自动启动
	BOOL			m_bAutoHide;				// 自动隐藏窗口
	CString			m_strWinText;				// 用于标识进程唯一性，同样ID的ViewGateway不能存在多个

private:
	WORD			m_dwBasePortForClient;		// 网关上监听客户端的端口的基础值,		默认监听端口为:基础端口+网关ID-1
	WORD			m_dwBasePortForView;		// 网关上监听ViewServer的端口的基础值，	默认监听端口为:基础端口+网关ID-1
	WORD			m_wServerID;				// 本服务器ID
	DWORD			m_dwID;						// Mng为我生成的自增ID
};
extern Setting& gSetting;