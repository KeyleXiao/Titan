/*******************************************************************
** 文件名:	Setting.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/21
** 版  本:	1.0
** 描  述:	观战Server配置信息
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include <string>
#include "singleton.h"


class Setting : public Singleton<Setting>
{
public:
	Setting::Setting() :
		m_dwBasePortForView(0)
		, m_dwTraceLevel(0)
		, m_bAutoStart(FALSE)
		, m_bAutoHide(FALSE)
		, m_dwMaxConnection(0)
		, m_dwServerID(0)
		, m_dwMngPort(0)
	{
		LoadSetting();
	}

	// 由本地文件目录名而得到的ID
	inline	DWORD	getServerID() { return m_dwServerID; }
	void			SetServerID( DWORD dwServerID );

	// Mng为我生成的自增ID
	inline	DWORD	GetID() { return m_dwID; }
	inline	void	SetID(DWORD dwID) { m_dwID = dwID; }

private:
	// 读入配置文件
	bool LoadSetting(void);

public:
	DWORD			m_dwMaxConnection;		// 最大连接数
	string			m_strMngIp;				// 观战管理服IP
	WORD			m_dwMngPort;			// 观战管理服端口

	string			m_strRedisIp;			// RedisIP
	WORD			m_wRedisPort;			// Redis端口

	DWORD			m_dwTraceLevel;			// trace日志级别
	BOOL			m_bAutoStart;			// 自动启动
	BOOL			m_bAutoHide;			// 自动隐藏窗口
	CString			m_strWinText;			// 用于标识进程唯一性，同样ID的ViewGateway不能存在多个

private:
	WORD			m_dwBasePortForView;	// 网关上监听ViewServer的端口的基础值
	DWORD			m_dwServerID;			// 本服务器ID
	DWORD			m_dwID;					// Mng为我生成的自增ID
};
extern Setting& gSetting;